/**
Copyright 2009-2017 National Technology and Engineering Solutions of Sandia, 
LLC (NTESS).  Under the terms of Contract DE-NA-0003525, the U.S.  Government 
retains certain rights in this software.

Sandia National Laboratories is a multimission laboratory managed and operated
by National Technology and Engineering Solutions of Sandia, LLC., a wholly 
owned subsidiary of Honeywell International, Inc., for the U.S. Department of 
Energy's National Nuclear Security Administration under contract DE-NA0003525.

Copyright (c) 2009-2017, NTESS

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of Sandia Corporation nor the names of its
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

#include <sstmac/backends/common/sim_partition.h>
#include <sstmac/backends/common/parallel_runtime.h>
#include <sstmac/common/sstmac_config.h>
#include <sstmac/common/serializable.h>
#include <sstmac/common/sst_event.h>
#include <sstmac/common/event_manager.h>
#include <sprockit/output.h>
#include <sprockit/fileio.h>
#include <fstream>
#include <sstream>
#include <sprockit/keyword_registration.h>

RegisterDebugSlot(parallel);

RegisterKeywords(
"serialization_buffer_size",
"serialization_num_bufs_allocation",
"partition",
"runtime",
);

namespace sstmac {

const int parallel_runtime::global_root = -1;
parallel_runtime* parallel_runtime::static_runtime_ = nullptr;

void
parallel_runtime::bcast_string(std::string& str, int root)
{
  if (nproc_ == 1)
    return;

  if (root == me_){
    int size = str.size(); //+1 for null terminator
    int root = 0;
    bcast(&size, sizeof(int), root);
    char* buf = const_cast<char*>(str.c_str());
    bcast(buf, size, root);
  } else {
    int size;
    bcast(&size, sizeof(int), root);
    str.resize(size);
    char* buf = const_cast<char*>(str.c_str());
    bcast(buf, size, root);
  }
}

std::istream*
parallel_runtime::bcast_file_stream(const std::string &fname)
{

  if (me_ == 0){
    std::ifstream* fstr = new std::ifstream;
    sprockit::SpktFileIO::open_file(*fstr, fname);

    if (!fstr->is_open()) {
      spkt_throw_printf(sprockit::input_error,
       "could not find file %s in current folder or configuration include path",
       fname.c_str());
    }

    if (nproc_ == 1){
      return fstr; //nothing to do
    }

    std::stringstream sstr;
    std::string line;
    while (fstr->good()){
      std::getline(*fstr, line);
      sstr << line << "\n";
    }
    std::string all_text = sstr.str();
    bcast_string(all_text, 0);
    //go back to the beginning of the file
    fstr->clear();
    fstr->seekg(0, fstr->beg);
    return fstr;
  } else {
    std::string all_text;
    bcast_string(all_text, 0);
    //std::cout << all_text << std::endl;
    return new std::stringstream(all_text);
  }
}

void
parallel_runtime::init_partition_params(sprockit::sim_parameters *params)
{
#if SSTMAC_INTEGRATED_SST_CORE
  sprockit::abort("parallel_runtime::init_partition_params: should not be used with integrated core");
#else
  //out with the old, in with the new
  if (part_) delete part_;
  part_ = partition::factory::get_optional_param("partition", SSTMAC_DEFAULT_PARTITION_STRING, params, this);
#endif
}

parallel_runtime*
parallel_runtime::static_runtime(sprockit::sim_parameters* params)
{
#if SSTMAC_INTEGRATED_SST_CORE
  return nullptr;
#else
  static thread_lock rt_lock;
  rt_lock.lock();
  if (!static_runtime_){
    static_runtime_ = parallel_runtime::factory::get_param("runtime", params);
  }
  rt_lock.unlock();
  return static_runtime_;
#endif
}

void
parallel_runtime::init_runtime_params(sprockit::sim_parameters *params)
{
  //turn the number of procs and my rank into keywords
  nthread_ = params->get_optional_int_param("sst_nthread", 1);

  buf_size_ = params->get_optional_byte_length_param("serialization_buffer_size", 512);
  int num_bufs_window = params->get_optional_int_param("serialization_num_bufs_allocation", 100);

  size_t allocSize = buf_size_ * num_bufs_window;

  send_buffers_.resize(nproc_);
  recv_buffers_.resize(nproc_);
  for (int i=0; i < nproc_; ++i){
    send_buffers_[i].init(allocSize);
    recv_buffers_[i].init(allocSize);
  }
}

parallel_runtime::parallel_runtime(sprockit::sim_parameters* params,
                                   int me, int nproc)
  : part_(nullptr),
    me_(me),
    nproc_(nproc)
{
  if (me_ == 0){
    sprockit::output::init_out0(&std::cout);
    sprockit::output::init_err0(&std::cerr);
  }
  else {
    sprockit::output::init_out0(new std::ofstream("/dev/null"));
    sprockit::output::init_err0(new std::ofstream("/dev/null"));
  }
  sprockit::output::init_outn(&std::cout);
  sprockit::output::init_errn(&std::cerr);
}

parallel_runtime::~parallel_runtime()
{
  if (part_) delete part_;
}

void
parallel_runtime::run_serialize(serializer& ser, ipc_event_t* iev)
{
  ser & iev->dst;
  ser & iev->src;
  ser & iev->seqnum;
  ser & iev->t;
  ser & iev->ev;
}

void parallel_runtime::send_event(int thread_id, ipc_event_t* iev)
{
#if SSTMAC_INTEGRATED_SST_CORE
  spkt_throw_printf(sprockit::unimplemented_error,
      "parallel_runtime::send_event: should not be called on integrated core");
#else
  int lp;
  switch (iev->dst.type()){
    case device_id::router:
      lp = part_->lpid_for_switch(iev->dst.id());
      break;
    case device_id::logp_overlay:
      lp = iev->dst.id();
      break;
    default:
      spkt_abort_printf("Invalid IPC handler of type %d", iev->dst.type());
  }

  size_t overhead = sizeof(iev->t) + sizeof(iev->dst) + sizeof(iev->src) + sizeof(iev->seqnum);

  sprockit::serializer ser;
  ser.start_sizing();
  ser & iev->ev;

  size_t buffer_space_needed = overhead + ser.size();
  debug_printf(sprockit::dbg::parallel, "sending event of size %lu + %lu = %lu to lp %d at t=%10.6e: %s",
               overhead, ser.size(), buffer_space_needed, lp, iev->t.sec(),
               sprockit::to_string(iev->ev).c_str());
  comm_buffer& buff = send_buffers_[lp];
  char* ptr = buff.ensureSpace(buffer_space_needed);
  ser.start_packing(ptr, buffer_space_needed);
  run_serialize(ser, iev);
  buff.shift(buffer_space_needed);
#endif
}

void
parallel_runtime::reset_send_recv()
{
  //and all the send buffers are now done
  for (int t=0; t < nproc_; ++t){
    send_buffers_[t].reset();
    recv_buffers_[t].reset();
  }
}

void
parallel_runtime::send_recv_messages()
{
}

}
