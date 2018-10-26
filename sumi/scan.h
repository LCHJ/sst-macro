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

#ifndef sumi_scan_h
#define sumi_scan_h

#include <sumi/collective.h>
#include <sumi/collective_actor.h>
#include <sumi/collective_message.h>
#include <sumi/comm_functions.h>

namespace sumi {

class simultaneous_btree_scan_actor :
  public dag_collective_actor
{

 public:
  simultaneous_btree_scan_actor(collective_engine* engine, void* dst, void* src, int nelems, int type_size,
                                int tag, reduce_fxn fxn, int cq_id, communicator* comm) :
    dag_collective_actor(collective::scan, engine, dst, src, type_size, tag, cq_id, comm),
    nelems_(nelems), fxn_(fxn)
  {
  }

  std::string to_string() const override {
    return "simultaneous btree scan";
  }

 private:
  void buffer_action(void *dst_buffer, void *msg_buffer, action* ac) override;
  void finalize_buffers() override;
  void init_buffers() override;
  void init_dag() override;
  void start_shuffle(action* ac) override;

 private:
  int nelems_;

  reduce_fxn fxn_;

  int num_reducing_rounds_;

  int num_total_rounds_;

};

class simultaneous_btree_scan :
  public dag_collective
{
 public:
  simultaneous_btree_scan(collective_engine* engine, void* dst, void* src, int nelems,
                          int type_size, int tag, reduce_fxn fxn, int cq_id, communicator* comm) :
    dag_collective(collective::scan, engine, dst, src, type_size, tag, cq_id, comm),
    nelems_(nelems), fxn_(fxn)
  {
  }


  std::string to_string() const override {
    return "simultaneous btree scan";
  }

  dag_collective_actor* new_actor() const override {
    return new simultaneous_btree_scan_actor(engine_, dst_buffer_, src_buffer_,
                                             nelems_, type_size_, tag_, fxn_, cq_id_, comm_);
  }

 private:
  int nelems_;
  reduce_fxn fxn_;


};


}


#endif
