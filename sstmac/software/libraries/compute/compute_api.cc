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

#include <sstmac/software/process/operating_system.h>
#include <sstmac/software/process/thread.h>
#include <sstmac/software/process/app.h>
#include <sstmac/software/process/time.h>
#include <sstmac/software/libraries/compute/compute_api.h>
#include <sprockit/thread_safe_new.h>


using sstmac::timestamp;
using os = sstmac::sw::operating_system;

extern "C" unsigned int sstmac_sleep(unsigned int secs){
  os::current_os()->sleep(timestamp(secs, timestamp::seconds));
  return 0;
}

extern "C" unsigned sstmac_sleep_until(double t){
  os::current_os()->sleep_until(timestamp(t));
  return 0;
}

extern "C" int sstmac_usleep(unsigned int usecs){
  os::current_os()->sleep(timestamp(usecs, timestamp::microseconds));
  return 0;
}

extern "C" int sstmac_nanosleep(unsigned int nanosecs){
  os::current_os()->sleep(timestamp(nanosecs, timestamp::nanoseconds));
  return 0;
}

extern "C" int sstmac_msleep(unsigned int msecs){
  os::current_os()->sleep(timestamp(msecs, timestamp::milliseconds));
  return 0;
}

extern "C" int sstmac_fsleep(double secs){
  sstmac::sw::operating_system::current_thread()->parent_app()->sleep(sstmac::timestamp(secs));
  return 0;
}

extern "C" void sstmac_compute(double secs){
  sstmac::sw::operating_system::current_thread()->parent_app()->compute(sstmac::timestamp(secs));
}

extern "C" void sstmac_memread(uint64_t bytes){
  sstmac::sw::operating_system::current_thread()->parent_app()
    ->compute_block_read(bytes);
}

extern "C" void sstmac_memwrite(uint64_t bytes){
  sstmac::sw::operating_system::current_thread()->parent_app()
    ->compute_block_write(bytes);
}

extern "C" void sstmac_memcopy(uint64_t bytes){
  sstmac::sw::operating_system::current_thread()->parent_app()
    ->compute_block_memcpy(bytes);
}

extern "C" void sstmac_compute_detailed(uint64_t nflops, uint64_t nintops, uint64_t bytes){
  sstmac::sw::operating_system::current_thread()
    ->compute_detailed(nflops, nintops, bytes);
}

extern "C" void sstmac_compute_detailed_nthr(uint64_t nflops, uint64_t nintops, uint64_t bytes,
                                        int nthread){
  sstmac::sw::operating_system::current_thread()
    ->compute_detailed(nflops, nintops, bytes, nthread);
}

extern "C" void sstmac_compute_loop(uint64_t num_loops, uint32_t nflops_per_loop,
                    uint32_t nintops_per_loop, uint32_t bytes_per_loop){
  sstmac::sw::operating_system::current_thread()->parent_app()
    ->compute_loop(num_loops, nflops_per_loop, nintops_per_loop, bytes_per_loop);
}

extern "C" void sstmac_compute_loop2(uint64_t isize, uint64_t jsize,
                    uint32_t nflops_per_loop,
                    uint32_t nintops_per_loop, uint32_t bytes_per_loop){
  uint64_t num_loops = isize * jsize;
  sstmac::sw::operating_system::current_thread()->parent_app()
    ->compute_loop(num_loops, nflops_per_loop, nintops_per_loop, bytes_per_loop);
}

extern "C" void
sstmac_compute_loop3(uint64_t isize, uint64_t jsize, uint64_t ksize,
                    uint32_t nflops_per_loop,
                    uint32_t nintops_per_loop,
                    uint32_t bytes_per_loop){
  uint64_t num_loops = isize * jsize * ksize;
  sstmac::sw::operating_system::current_thread()->parent_app()
    ->compute_loop(num_loops, nflops_per_loop, nintops_per_loop, bytes_per_loop);
}

extern "C" void
sstmac_compute_loop4(uint64_t isize, uint64_t jsize, uint64_t ksize, uint64_t lsize,
                     uint32_t nflops_per_loop,
                     uint32_t nintops_per_loop,
                     uint32_t bytes_per_loop){
  uint64_t num_loops = isize * jsize * ksize * lsize;
  sstmac::sw::operating_system::current_thread()->parent_app()
    ->compute_loop(num_loops, nflops_per_loop, nintops_per_loop, bytes_per_loop);
}

extern "C" void sstmac_start_memoize(const char *token, const char* model)
{
  sstmac::sw::operating_system::start_memoize(token, model);
}

extern "C" void sstmac_finish_memoize0(const char* token)
{
  double params[0];
  sstmac::sw::operating_system::stop_memoize(token, 0, params);
}

extern "C" void sstmac_finish_memoize1(const char *token, double param1)
{
  double params[1];
  params[0] = param1;
  sstmac::sw::operating_system::stop_memoize(token, 1, params);
}

extern "C" void sstmac_finish_memoize2(const char *token,
                                       double param1, double param2)
{
  double params[2];
  params[0] = param1;
  params[1] = param2;
  sstmac::sw::operating_system::stop_memoize(token, 2, params);
}

extern "C" void sstmac_compute_memoize0(const char* token)
{
  double params[0];
  sstmac::sw::operating_system::compute_memoize(token, 0, params);
}

extern "C" void sstmac_compute_memoize1(const char *token, double param1)
{
  double params[1];
  params[0] = param1;
  sstmac::sw::operating_system::compute_memoize(token, 1, params);
}

extern "C" void sstmac_compute_memoize2(const char *token, double param1, double param2)
{
  double params[2];
  params[0] = param1;
  params[1] = param2;
  sstmac::sw::operating_system::compute_memoize(token, 2, params);
}

extern "C" void* sstmac_alloc_stack(int sz, int md_sz)
{
  if (md_sz >= SSTMAC_TLS_OFFSET){
    spkt_abort_printf("Cannot have stack metadata larger than %d - requested %d",
                      SSTMAC_TLS_OFFSET, md_sz);
  }
  if (sz > sstmac::sw::operating_system::stacksize()){
    spkt_abort_printf("Cannot allocate stack larger than %d - requested %d",
                      sstmac::sw::operating_system::stacksize(), sz);
  }
  void* stack = sstmac::sw::stack_alloc::alloc();
  int getstack; int* stackPtr = &getstack;
  uintptr_t localStorage = ((uintptr_t) stackPtr/sstmac_global_stacksize)*sstmac_global_stacksize;

  void* new_mdata = (char*)stack + SSTMAC_TLS_OFFSET;
  void* old_mdata = (char*)localStorage + SSTMAC_TLS_OFFSET;
  ::memcpy(new_mdata, old_mdata, SSTMAC_TLS_SIZE);

  return stack;
}

extern "C" void sstmac_free_stack(void* ptr)
{
  sstmac::sw::stack_alloc::free(ptr);
}
