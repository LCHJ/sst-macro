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

#include <sstmac/hardware/pisces/pisces_crossbar.h>
#include <sstmac/hardware/pisces/pisces_stats.h>
#include <sstmac/hardware/pisces/pisces_tiled_switch.h>
#include <sstmac/hardware/topology/structured_topology.h>
#include <sstmac/common/stats/stat_global_int.h>
#include <sstmac/common/event_callback.h>
#include <sstmac/common/runtime.h>
#include <iomanip>
#include <sprockit/util.h>

#define PRINT_FINISH_DETAILS 0

RegisterNamespaces("bytes_sent");

namespace sstmac {
namespace hw {

pisces_crossbar::pisces_crossbar(
  sprockit::sim_parameters* params,
  event_scheduler* parent) :
  pisces_NtoM_queue(params, parent)
{
}

pisces_demuxer::pisces_demuxer(
  sprockit::sim_parameters* params,
  event_scheduler* parent) :
  pisces_NtoM_queue(params, parent)
{
}

pisces_muxer::pisces_muxer(
  sprockit::sim_parameters* params,
  event_scheduler* parent) :
  pisces_NtoM_queue(params, parent)
{

}

pisces_NtoM_queue::
pisces_NtoM_queue(sprockit::sim_parameters* params, event_scheduler* parent)
  : pisces_sender(params, parent),
    credit_handler_(nullptr),
    payload_handler_(nullptr),
    tile_id_("")
{
  num_vc_ = params->get_int_param("num_vc");
  arb_ = pisces_bandwidth_arbitrator::factory::get_param("arbitrator", params);
}

event_handler*
pisces_NtoM_queue::credit_handler()
{
  if (!credit_handler_){
    credit_handler_ = new_handler(this, &pisces_NtoM_queue::handle_credit);
  }
  return credit_handler_;
}

event_handler*
pisces_NtoM_queue::payload_handler()
{
  if (!payload_handler_){
    payload_handler_ = new_handler(this, &pisces_NtoM_queue::handle_payload);
  }
  return payload_handler_;
}

pisces_NtoM_queue::~pisces_NtoM_queue()
{
  if (arb_) delete arb_;
  if (credit_handler_) delete credit_handler_;
  for (auto& inp : inputs_){
    if (inp.link) delete inp.link;
  }
  for (auto& out : outputs_){
    if (out.link) delete out.link;
  }
}

void
pisces_NtoM_queue::deadlock_check()
{
#if !SSTMAC_INTEGRATED_SST_CORE
  for (int i=0; i < queues_.size(); ++i){
    payload_queue& queue = queues_[i];
    pisces_payload* pkt = queue.front();
    while (pkt){
      deadlocked_channels_[pkt->outport()].insert(pkt->next_vc());
      event_link* output = output_link(pkt);
      if (output){
        int vc = update_vc_ ? pkt->next_vc() : pkt->vc();
        std::cerr << "Starting deadlock check on " << to_string()
                  << " on queue " << i
                  << " going to " << output->to_string()
                  << " outport=" << pkt->outport()
                  << " vc=" << vc
                  << " for message " << pkt->to_string()
                  << std::endl;
        output->deadlock_check(pkt);
      }
      queue.pop(1000000);
      pkt = queue.front();
    }
  }
#endif
}

void
pisces_NtoM_queue::build_blocked_messages()
{
  for (int i=0; i < queues_.size(); ++i){
    payload_queue& queue = queues_[i];
    pisces_payload* pkt = queue.pop(1000000);
    while (pkt){
      blocked_messages_[pkt->next_local_inport()][pkt->vc()].push_back(pkt);
      pkt = queue.pop(10000000);
    }
  }
}

void
pisces_NtoM_queue::deadlock_check(event* ev)
{
#if !SSTMAC_INTEGRATED_SST_CORE
  if (blocked_messages_.empty()){
    build_blocked_messages();
  }

  pisces_payload* payload = safe_cast(pisces_payload, ev);
  int inport = payload->next_local_inport();
  int vc = update_vc_ ? payload->next_vc() : payload->vc();
  std::set<int>& deadlocked_vcs = deadlocked_channels_[payload->outport()];
  if (deadlocked_vcs.find(vc) != deadlocked_vcs.end()){
    spkt_throw_printf(sprockit::value_error,
      "found deadlock:\n%s", to_string().c_str());
  }

  deadlocked_channels_[payload->outport()].insert(vc);

  std::list<pisces_payload*>& blocked = blocked_messages_[inport][vc];
  if (blocked.empty()){
    spkt_throw_printf(sprockit::value_error,
      "channel is NOT blocked on deadlock check on outport=%d inport=%d vc=%d",
      payload->outport(), inport, vc);
  } else {
    pisces_payload* next = blocked.front();
    event_link* output = output_link(next);
    std::cerr << to_string() << " going to "
      << output->to_string()
      << " outport=" << next->outport()
      << " vc=" << next->next_vc()
      << " : " << next->to_string()
      << std::endl;
    output->deadlock_check(next);
  }
#endif
}

std::string
pisces_NtoM_queue::input_name(pisces_payload* pkt)
{
  event_link* link = inputs_[pkt->next_local_inport()].link;
  return link->to_string();
}

event_link*
pisces_NtoM_queue::output_link(pisces_payload* pkt)
{
  return outputs_[pkt->next_local_outport()].link;
}

std::string
pisces_NtoM_queue::output_name(pisces_payload* pkt)
{
  return output_link(pkt)->to_string();
}

void
pisces_NtoM_queue::send_payload(pisces_payload* pkt)
{
#if SSTMAC_SANITY_CHECK
  int port = pkt->next_local_outport();
  if (port >= outputs_.size() || outputs_[port].link == nullptr){
    auto* hdr = pkt->ctrl_header();
    spkt_abort_printf("got bad outport %d on stage %d", port, int(hdr->stage));
  }
  port = pkt->next_local_inport();
  if (port >= inputs_.size()){
    auto* hdr = pkt->ctrl_header();
    spkt_abort_printf("got bad inport %d on stage %d", port, int(hdr->stage));
  }
#endif
  send(arb_, pkt, inputs_[pkt->next_local_inport()], outputs_[pkt->next_local_outport()]);
}

void
pisces_NtoM_queue::handle_credit(event *ev)
{
  pisces_credit* pkt = static_cast<pisces_credit*>(ev);
  handle_credits(pkt->port(), pkt->vc(), pkt->num_credits());
  delete pkt;
}

void
pisces_NtoM_queue::handle_credits(int local_port, int vc, int credits)
{
  int channel = local_port * num_vc_ + vc;

  int& num_credits = credit(local_port, vc);

  pisces_debug(
    "On %s:%p with %d credits, handling credit for local port:%d vc:%d channel:%d",
     to_string().c_str(), this,
     num_credits, local_port, vc, channel);

  num_credits += credits;

#if SSTMAC_SANITY_CHECK
  if (num_credits > initial_credits_[slot(local_port,vc)]){
    spkt_abort_printf("initial credits exceeded");
  }
#endif

  pisces_payload* payload = queue(local_port, vc).pop(num_credits);
  if (payload) {
    num_credits -= payload->num_bytes();
    send_payload(payload);
  }
}

void
pisces_NtoM_queue::handle_payload(event* ev)
{
  auto pkt = static_cast<pisces_payload*>(ev);
  pkt->set_arrival(now());

  auto* hdr = pkt->ctrl_header();
  int dst_vc = update_vc_ ? pkt->next_vc() : pkt->vc();
  int loc_port = hdr->outports[hdr->stage];
  pisces_debug(
   "On %s:%p, handling {%s} for vc:%d local_port:%d on stage %d",
    to_string().c_str(), this,
    pkt->to_string().c_str(), dst_vc, loc_port, int(hdr->stage));

  if (dst_vc < 0 || loc_port < 0){
    spkt_abort_printf("On %s handling {%s}, got negative vc,local_port %d,%d",
        to_string().c_str(), pkt->to_string().c_str(), loc_port, dst_vc);
  }

  int& num_credits = credit(loc_port, dst_vc);
   pisces_debug(
    "On %s:%p with %d credits, handling {%s} for local port:%d vc:%d",
     to_string().c_str(), this,
     num_credits,
     pkt->to_string().c_str(),
     loc_port, dst_vc);

  if (num_credits >= pkt->num_bytes()) {
    num_credits -= pkt->num_bytes();
    send_payload(pkt);
  } else {
    pisces_debug(
      "On %s:%p, pushing back %s on queue %d=(%d,%d) for nq=%d nvc=%d mapper=(%d,%d,%d)",
      to_string().c_str(), this, pkt->to_string().c_str(),
      slot(loc_port, dst_vc), loc_port, dst_vc, queues_.size(), num_vc_,
      port_offset_, port_div_, port_mod_);
      queue(loc_port, dst_vc).push_back(pkt);
  }
}

void
pisces_NtoM_queue::resize_outports(int num_ports)
{
  outputs_.resize(num_ports);
  queues_.resize(num_ports*num_vc_);
  credits_.resize(num_ports*num_vc_);
#if SSTMAC_SANITY_CHECK
  initial_credits_.resize(num_ports*num_vc_);
#endif
}

void
pisces_NtoM_queue::set_input(
  sprockit::sim_parameters* port_params,
  int my_inport, int src_outport,
  event_link* link)
{
  // ports are local for local links and global otherwise

  debug_printf(sprockit::dbg::pisces_config | sprockit::dbg::pisces,
    "On %s:%d setting input %s:%d",
    to_string().c_str(), my_inport,
    link ? link->to_string().c_str() : "null", src_outport);
  input& inp = inputs_[my_inport];
  inp.link = link;
  inp.port_to_credit = src_outport;
  link->validate_latency(credit_lat_);
}

void
pisces_NtoM_queue::set_output(
  sprockit::sim_parameters* port_params,
  int my_outport, int dst_inport,
  event_link* link)
{
  // must be called with local my_outport (if there's a difference)
  // global port numbers aren't unique for individual elements of
  // a tiled switch, for instance, so this is the only logical approach

  // dst_inport is local for local links and global otherwise

  debug_printf(sprockit::dbg::pisces_config | sprockit::dbg::pisces,
    "On %s setting output %s:%d for local port %d, mapper=(%d,%d,%d) of %d",
    to_string().c_str(),
    link->to_string().c_str(), dst_inport,
    my_outport, port_offset_, port_div_, port_mod_,
    outputs_.size());

  if (my_outport > outputs_.size()) {
    spkt_throw_printf(sprockit::value_error,
                      "pisces_crossbar: my_outport %i > outputs_.size() %i",
                      my_outport, outputs_.size());
  }
  link->validate_latency(send_lat_);
  output& out = outputs_[my_outport];
  out.link = link;
  out.arrival_port = dst_inport;

  int num_credits = port_params->get_byte_length_param("credits");
  int num_credits_per_vc = num_credits / num_vc_;
  for (int i=0; i < num_vc_; ++i) {
    debug_printf(sprockit::dbg::pisces_config,
                 "On %s:%p, initing %d credits on port:%d vc:%d",
                 to_string().c_str(), this,
                 num_credits_per_vc,
                 my_outport, i);
    credit(my_outport, i) = num_credits_per_vc;
#if SSTMAC_SANITY_CHECK
    initial_credits_[slot(my_outport,i)] = num_credits_per_vc;
#endif
  }
}

#if PRINT_FINISH_DETAILS
void
print_msg(const std::string& prefix, switch_id addr, pisces_payload* pkt)
{
  structured_topology* top = safe_cast(structured_topology, sstmac_runtime::current_topology());
  coordinates src_coords = top->get_node_coords(msg->fromaddr());
  src_coords.resize(3);
  coordinates dst_coords = top->get_node_coords(msg->toaddr());
  dst_coords.resize(3);
  coordinates my_coords = top->get_switch_coords(addr);
  coutn << prefix << std::setw(12) << src_coords.to_string();
  coutn << "->";
  coutn << std::setw(12) << my_coords.to_string();
  coutn << "->";
  coutn << std::setw(12) << dst_coords.to_string();
  coutn << "  vc=" << msg->rinfo()->vc()
    << " port=" << msg->rinfo()->port() << std::endl;
}
#endif

void
pisces_NtoM_queue::start_message(message* msg)
{
  sprockit::abort("pisces_NtoM_queue:: should never start a flow");
}

#if PRINT_FINISH_DETAILS
  structured_topology* top = safe_cast(structured_topology, sstmac_runtime::current_topology());
  coordinates my_coords = top->get_switch_coords(router_->get_addr());
  coutn << "Crossbar " << my_coords.to_string() << "\n";
  { queue_map::iterator it, end = queues_.end();
  for (it = queues_.begin(); it != end; ++it){
    std::vector<payload_queue>& vec = it->second;
    coutn << "\tPort " << it->first << "\n";
    for (int i=0; i < vec.size(); ++i){
        coutn << "\t\tVC " << i << std::endl;
        payload_queue& que = vec[i];
        payload_queue::iterator pit, pend = que.end();
        for (pit = que.begin(); pit != pend; ++pit){
            pisces_payload* pkt = *pit;
            print_msg("\t\t\tPending: ", router_->get_addr(), msg);
        }
    }
  } }
  { credit_map::iterator it, end = credits_.end();
  for (it = credits_.begin(); it != end; ++it){
    std::vector<long>& vec = it->second;
    coutn << "\tPort " << it->first << "\n";
    for (int i=0; i < vec.size(); ++i){
      coutn << "\t\tVC " << i
            << " = " << vec[i] << " credits" << std::endl;

    }
  } }
#endif

}
}
