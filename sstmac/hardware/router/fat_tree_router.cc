/**
Copyright 2009-2018 National Technology and Engineering Solutions of Sandia, 
LLC (NTESS).  Under the terms of Contract DE-NA-0003525, the U.S.  Government 
retains certain rights in this software.

Sandia National Laboratories is a multimission laboratory managed and operated
by National Technology and Engineering Solutions of Sandia, LLC., a wholly 
owned subsidiary of Honeywell International, Inc., for the U.S. Department of 
Energy's National Nuclear Security Administration under contract DE-NA0003525.

Copyright (c) 2009-2018, NTESS

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Questions? Contact sst-macro-help@sandia.gov
*/

#include <sstmac/hardware/router/fat_tree_router.h>
#include <sstmac/hardware/switch/network_switch.h>
#include <sstmac/hardware/topology/fat_tree.h>
#include <sprockit/util.h>
#include <sprockit/sim_parameters.h>
#include <cmath>

using namespace std;

#define ftree_rter_debug(...) \
  rter_debug("fat tree: %s", sprockit::printf(__VA_ARGS__).c_str())

namespace sstmac {
namespace hw {

fat_tree_router::fat_tree_router(
    sprockit::sim_parameters* params,
    topology *top,
    network_switch *netsw) :
  router(params, top, netsw)
{
  ft_ = safe_cast(fat_tree, top);
  if (my_addr_ >= (ft_->num_leaf_switches() + ft_->num_agg_switches())){
    my_row_ = 2;
    num_up_ports_ = 0;
  } else if (my_addr_ >= (ft_->num_leaf_switches())){
    my_row_ = 1;
    num_up_ports_ = ft_->up_ports_per_agg_switch();
    first_up_port_ = ft_->first_up_port(my_addr_);
    my_tree_ = ft_->subtree(my_addr_);
    up_next_ = my_addr_ % num_up_ports_;
  } else {
    my_row_ = 0;
    my_tree_ = ft_->subtree(my_addr_);
    num_up_ports_ = ft_->up_ports_per_leaf_switch();
    first_up_port_ = ft_->first_up_port(my_addr_);
    up_next_ = my_addr_ % num_up_ports_;
  }

  if (my_row_ == 2){
    down_routes_.resize(ft_->num_agg_subtrees());
    std::vector<topology::connection> conns;
    ft_->connected_outports(my_addr_, conns);
    for (topology::connection& conn : conns){
      int subtree = (conn.dst - ft_->num_leaf_switches()) / ft_->agg_switches_per_subtree();
      down_routes_[subtree].push_back(conn.src_outport);
    }
  } else if (my_row_ == 1){
    down_routes_.resize(ft_->leaf_switches_per_subtree());
    std::vector<topology::connection> conns;
    ft_->connected_outports(my_addr_, conns);
    for (topology::connection& conn : conns){
      if (conn.dst < my_addr_){
        int leaf = conn.dst % ft_->leaf_switches_per_subtree();
        down_routes_[leaf].push_back(conn.src_outport);
      }
    }
  }

  down_rotaters_.resize(down_routes_.size());
  for (int i=0; i < down_routes_.size(); ++i){
    //scatter across switches
    down_rotaters_[i] = my_addr_ % down_routes_[i].size();
  }
}

void
fat_tree_router::route(packet* pkt) {

  int output_port;
  packet::path& path = pkt->current_path();
  switch_id dst = find_ejection_site(pkt->toaddr(), path);

  // already there
  if (dst == my_addr_){
    path.vc = 0;
    rter_debug("Ejecting %s from switch %d on port %d",
               pkt->to_string().c_str(), dst, path.outport());
  } else { // have to route
    int dst_tree = ft_->subtree(dst);
    if (my_row_ == 0){ //leat switch - going up
      //definitely have to go up since we didn't eject
      output_port = get_up_port();
      path.set_outport(output_port);
      path.vc = 0;
      rter_debug("fat_tree: routing up to get to s=%d through l=1 from s=%d,l=0",
                int(dst), int(my_addr_));
    } else if (my_row_ == 2){     // definitely have to go down
      output_port = get_down_port(dst_tree);
      path.set_outport(output_port);
      path.vc = 0;
      rter_debug("fat_tree: routing down to get to s=%d through l=1 from s=%d,l=2",
                int(dst), int(my_addr_));
    } else if (my_row_ == 1){ // aggregator level, can go either way
      // in the right tree, going down
      if (dst_tree == my_tree_) {
        int dst_leaf = dst % ft_->leaf_switches_per_subtree();
        output_port = get_down_port(dst_leaf);
        path.set_outport(output_port);
        path.vc = 0;
        rter_debug("fat_tree: routing down to get to s=%d,l=0 from s=%d,l=1",
                  int(dst), int(my_addr_));
      } else { //nope, have to go to core to hop over to other tree
        output_port = get_up_port();
        path.set_outport(output_port);
        path.vc = 0;
        rter_debug("fat_tree: routing up to get to s=%d through l=2 from s=%d,l=1",
                  int(dst), int(my_addr_));
      }
    } else {
        spkt_abort_printf("Got bad level=%d on switch %d", my_row_, my_addr_);
    }
    rter_debug("Routing %s to switch %d on port %d",
               pkt->to_string().c_str(), int(dst), path.outport());
  }
}

// up is easy -- any "up" port goes up
int
fat_tree_router::get_up_port() {
  int port = first_up_port_ + up_next_;
  up_next_ = (up_next_ + 1) % num_up_ports_;
  return port;
}

int
fat_tree_router::get_down_port(int path)
{
  auto& routes = down_routes_[path];
  int port = routes[down_rotaters_[path]];
  int nroutes = routes.size();
  down_rotaters_[path] = (down_rotaters_[path]+1) % nroutes;
  return port;
}


}
}
