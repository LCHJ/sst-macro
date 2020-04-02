/**
Copyright 2009-2020 National Technology and Engineering Solutions of Sandia, 
LLC (NTESS).  Under the terms of Contract DE-NA-0003525, the U.S.  Government 
retains certain rights in this software.

Sandia National Laboratories is a multimission laboratory managed and operated
by National Technology and Engineering Solutions of Sandia, LLC., a wholly 
owned subsidiary of Honeywell International, Inc., for the U.S. Department of 
Energy's National Nuclear Security Administration under contract DE-NA0003525.

Copyright (c) 2009-2020, NTESS

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

#ifndef SSTMAC_PTHREAD_MACRO_H
#define SSTMAC_PTHREAD_MACRO_H

#include <sstmac/libraries/pthread/sstmac_pthread_clear_macros.h>


#define PTHREAD_THREADS_MAX SSTMAC_PTHREAD_THREADS_MAX
#define PTHREAD_KEYS_MAX SSTMAC_PTHREAD_KEYS_MAX
#define PTHREAD_STACK_MIN SSTMAC_PTHREAD_STACK_MIN
#define PTHREAD_CREATE_DETACHED SSTMAC_PTHREAD_CREATE_DETACHED
#define PTHREAD_CREATE_JOINABLE SSTMAC_PTHREAD_CREATE_JOINABLE
#define PTHREAD_ONCE_INIT SSTMAC_PTHREAD_ONCE_INIT

#define PTHREAD_MUTEX_NORMAL SSTMAC_PTHREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_ERRORCHECK SSTMAC_PTHREAD_MUTEX_ERRORCHECK
#define PTHREAD_MUTEX_RECURSIVE SSTMAC_PTHREAD_MUTEX_RECURSIVE
#define PTHREAD_MUTEX_DEFAULT SSTMAC_PTHREAD_MUTEX_DEFAULT
#define PTHREAD_MUTEX_ERRORCHECK_NP SSTMAC_PTHREAD_MUTEX_ERRORCHECK_NP

#define pthread_create SSTMAC_pthread_create
#define pthread_exit SSTMAC_pthread_exit
#define pthread_join SSTMAC_pthread_join
#define pthread_tryjoin_np SSTMAC_pthread_tryjoin_np
#define pthread_timedjoin_np SSTMAC_pthread_timedjoin_np
#define pthread_testcancel SSTMAC_pthread_testcancel
#define pthread_detach SSTMAC_pthread_detach
#define pthread_equal SSTMAC_pthread_equal
#define pthread_attr_init SSTMAC_pthread_attr_init
#define pthread_attr_destroy SSTMAC_pthread_attr_destroy
#define pthread_attr_getdetachstate SSTMAC_pthread_attr_getdetachstate
#define pthread_attr_setdetachstate SSTMAC_pthread_attr_setdetachstate
#define pthread_attr_getguardsize SSTMAC_pthread_attr_getguardsize
#define pthread_attr_setguardsize SSTMAC_pthread_attr_setguardsize
#define pthread_attr_getschedparam SSTMAC_pthread_attr_getschedparam
#define pthread_attr_setschedparam SSTMAC_pthread_attr_setschedparam
#define pthread_attr_getschedpolicy SSTMAC_pthread_attr_getschedpolicy
#define pthread_attr_setschedpolicy SSTMAC_pthread_attr_setschedpolicy
#define pthread_attr_getinheritsched SSTMAC_pthread_attr_getinheritsched
#define pthread_attr_setinheritsched SSTMAC_pthread_attr_setinheritsched
#define pthread_attr_getscope SSTMAC_pthread_attr_getscope
#define pthread_attr_setscope SSTMAC_pthread_attr_setscope
#define pthread_attr_getstackaddr SSTMAC_pthread_attr_getstackaddr
#define pthread_attr_setstackaddr SSTMAC_pthread_attr_setstackaddr
#define pthread_attr_getstacksize SSTMAC_pthread_attr_getstacksize
#define pthread_attr_setstacksize SSTMAC_pthread_attr_setstacksize
#define pthread_attr_getstack SSTMAC_pthread_attr_getstack
#define pthread_attr_setstack SSTMAC_pthread_attr_setstack
#define pthread_attr_setaffinity_np SSTMAC_pthread_attr_setaffinity_np
#define pthread_attr_getaffinity_np SSTMAC_pthread_attr_getaffinity_np
#define pthread_getattr_np SSTMAC_pthread_getattr_np
#define pthread_setschedparam SSTMAC_pthread_setschedparam
#define pthread_getschedparam SSTMAC_pthread_getschedparam
#define pthread_setschedprio SSTMAC_pthread_setschedprio
#define pthread_getname_np SSTMAC_pthread_getname_np
#define pthread_setname_np SSTMAC_pthread_setname_np
#define pthread_getconcurrency SSTMAC_pthread_getconcurrency
#define pthread_setconcurrency SSTMAC_pthread_setconcurrency
#define pthread_yield SSTMAC_pthread_yield
#define pthread_setaffinity_np SSTMAC_pthread_setaffinity_np
#define pthread_getaffinity_np SSTMAC_pthread_getaffinity_np
#define pthread_once SSTMAC_pthread_once
#define pthread_setcancelstate SSTMAC_pthread_setcancelstate
#define pthread_setcanceltype SSTMAC_pthread_setcanceltype
#define pthread_cancel SSTMAC_pthread_cancel
#define pthread_mutex_init SSTMAC_pthread_mutex_init
#define pthread_mutex_destroy SSTMAC_pthread_mutex_destroy
#define pthread_mutex_trylock SSTMAC_pthread_mutex_trylock
#define pthread_mutex_lock SSTMAC_pthread_mutex_lock
#define pthread_mutex_timedlock SSTMAC_pthread_mutex_timedlock
#define pthread_mutex_unlock SSTMAC_pthread_mutex_unlock
#define pthread_mutex_getprioceiling SSTMAC_pthread_mutex_getprioceiling
#define pthread_mutex_setprioceiling SSTMAC_pthread_mutex_setprioceiling
#define pthread_mutex_consistent SSTMAC_pthread_mutex_consistent
#define pthread_mutex_consistent_np SSTMAC_pthread_mutex_consistent_np
#define pthread_mutexattr_init SSTMAC_pthread_mutexattr_init
#define pthread_mutexattr_destroy SSTMAC_pthread_mutexattr_destroy
#define pthread_mutexattr_getpshared SSTMAC_pthread_mutexattr_getpshared
#define pthread_mutexattr_setpshared SSTMAC_pthread_mutexattr_setpshared
#define pthread_mutexattr_gettype SSTMAC_pthread_mutexattr_gettype
#define pthread_mutexattr_settype SSTMAC_pthread_mutexattr_settype
#define pthread_mutexattr_getprotocol SSTMAC_pthread_mutexattr_getprotocol
#define pthread_mutexattr_setprotocol SSTMAC_pthread_mutexattr_setprotocol
#define pthread_mutexattr_getprioceiling SSTMAC_pthread_mutexattr_getprioceiling
#define pthread_mutexattr_setprioceiling SSTMAC_pthread_mutexattr_setprioceiling
#define pthread_mutexattr_getrobust SSTMAC_pthread_mutexattr_getrobust
#define pthread_mutexattr_getrobust_np SSTMAC_pthread_mutexattr_getrobust_np
#define pthread_mutexattr_setrobust SSTMAC_pthread_mutexattr_setrobust
#define pthread_mutexattr_setrobust_np SSTMAC_pthread_mutexattr_setrobust_np
#define pthread_rwlock_init SSTMAC_pthread_rwlock_init
#define pthread_rwlock_destroy SSTMAC_pthread_rwlock_destroy
#define pthread_rwlock_rdlock SSTMAC_pthread_rwlock_rdlock
#define pthread_rwlock_tryrdlock SSTMAC_pthread_rwlock_tryrdlock
#define pthread_rwlock_timedrdlock SSTMAC_pthread_rwlock_timedrdlock
#define pthread_rwlock_wrlock SSTMAC_pthread_rwlock_wrlock
#define pthread_rwlock_trywrlock SSTMAC_pthread_rwlock_trywrlock
#define pthread_rwlock_timedwrlock SSTMAC_pthread_rwlock_timedwrlock
#define pthread_rwlock_unlock SSTMAC_pthread_rwlock_unlock
#define pthread_rwlockattr_init SSTMAC_pthread_rwlockattr_init
#define pthread_rwlockattr_destroy SSTMAC_pthread_rwlockattr_destroy
#define pthread_rwlockattr_getpshared SSTMAC_pthread_rwlockattr_getpshared
#define pthread_rwlockattr_setpshared SSTMAC_pthread_rwlockattr_setpshared
#define pthread_rwlockattr_getkind_np SSTMAC_pthread_rwlockattr_getkind_np
#define pthread_rwlockattr_setkind_np SSTMAC_pthread_rwlockattr_setkind_np

#define pthread_cond_init SSTMAC_pthread_cond_init
#define pthread_cond_destroy SSTMAC_pthread_cond_destroy
#define pthread_cond_signal SSTMAC_pthread_cond_signal
#define pthread_cond_broadcast SSTMAC_pthread_cond_broadcast
#define pthread_cond_wait SSTMAC_pthread_cond_wait
#define pthread_cond_timedwait SSTMAC_pthread_cond_timedwait
#define pthread_condattr_init SSTMAC_pthread_condattr_init
#define pthread_condattr_destroy SSTMAC_pthread_condattr_destroy
#define pthread_condattr_getpshared SSTMAC_pthread_condattr_getpshared
#define pthread_condattr_setpshared SSTMAC_pthread_condattr_setpshared
#define pthread_condattr_getclock SSTMAC_pthread_condattr_getclock
#define pthread_condattr_setclock SSTMAC_pthread_condattr_setclock
#define pthread_spin_init SSTMAC_pthread_spin_init
#define pthread_spin_destroy SSTMAC_pthread_spin_destroy
#define pthread_spin_lock SSTMAC_pthread_spin_lock
#define pthread_spin_trylock SSTMAC_pthread_spin_trylock
#define pthread_spin_unlock SSTMAC_pthread_spin_unlock
#define pthread_barrier_init SSTMAC_pthread_barrier_init
#define pthread_barrier_destroy SSTMAC_pthread_barrier_destroy
#define pthread_barrier_wait SSTMAC_pthread_barrier_wait
#define pthread_barrierattr_init SSTMAC_pthread_barrierattr_init
#define pthread_barrierattr_destroy SSTMAC_pthread_barrierattr_destroy
#define pthread_barrierattr_getpshared SSTMAC_pthread_barrierattr_getpshared
#define pthread_barrierattr_setpshared SSTMAC_pthread_barrierattr_setpshared
#define pthread_key_create SSTMAC_pthread_key_create
#define pthread_key_delete SSTMAC_pthread_key_delete
#define pthread_setspecific SSTMAC_pthread_setspecific
#define pthread_getcpuclockid SSTMAC_pthread_getcpuclockid
#define pthread_atfork SSTMAC_pthread_atfork
#define pthread_self SSTMAC_pthread_self
#define pthread_kill SSTMAC_pthread_kill
#define pthread_getspecific SSTMAC_pthread_getspecific
#define pthread_setspecific SSTMAC_pthread_setspecific
#define pthread_mutexattr_setpshared SSTMAC_pthread_mutexattr_setpshared
#define pthread_mutexattr_getpshared SSTMAC_pthread_mutexattr_getpshared

#define PTHREAD_PROCESS_SHARED SSTMAC_PTHREAD_PROCESS_SHARED
#define PTHREAD_PROCESS_PRIVATE SSTMAC_PTHREAD_PROCESS_PRIVATE

#define pthread_t sstmac_pthread_t
#define pthread_attr_t sstmac_pthread_attr_t
#define pthread_key_t sstmac_pthread_key_t
#define pthread_cond_t sstmac_pthread_cond_t
#define pthread_condattr_t sstmac_pthread_condattr_t
#define pthread_mutex_t sstmac_pthread_mutex_t
#define pthread_spinlock_t sstmac_pthread_spinlock_t
#define pthread_once_t sstmac_pthread_once_t
#define pthread_mutexattr_t sstmac_pthread_mutexattr_t
#define pthread_rwlock_t sstmac_pthread_rwlock_t
#define pthread_rwlock_attr_t sstmac_pthread_rwlock_attr_t

#define pthread_setconcurrency SSTMAC_pthread_setconcurrency
#define pthread_getconcurrency SSTMAC_pthread_getconcurrency

#define pthread_cleanup_push SSTMAC_pthread_cleanup_push
#define pthread_cleanup_pop SSTMAC_pthread_cleanup_pop

#define PTHREAD_ONCE_INIT SSTMAC_PTHREAD_ONCE_INIT
#define PTHREAD_COND_INITIALIZER SSTMAC_PTHREAD_COND_INITIALIZER
#define PTHREAD_MUTEX_INITIALIZER SSTMAC_PTHREAD_MUTEX_INITIALIZER

#define PTHREAD_RWLOCK_INITIALIZER SSTMAC_PTHREAD_RWLOCK_INITIALIZER

#define PTHREAD_SCOPE_PROCESS SSTMAC_PTHREAD_SCOPE_PROCESS
#define PTHREAD_SCOPE_SYSTEM SSTMAC_PTHREAD_SCOPE_SYSTEM

//#undef __thread
//#define __thread thread_local_not_yet_allowed
//#undef thread_local
//#define thread_local thread_local_not_yet_allowed


#endif
