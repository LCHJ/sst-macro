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

#include <sstmac/software/process/ftq.h>
#include <sstmac/common/thread_lock.h>
#include <sstmac/software/process/thread.h>
#include <sstmac/backends/common/parallel_runtime.h>
#include <sprockit/delete.h>
#include <sprockit/sim_parameters.h>
#include <sprockit/util.h>
#include <sprockit/keyword_registration.h>
#include <sstream>

RegisterKeywords(
 { "epoch", "the size of a time epoch" },
);

namespace sstmac {
namespace sw {

std::unordered_map<int, AppFTQCalendar*> FTQCalendar::calendars_;
const uint64_t AppFTQCalendar::allocation_num_epochs = 10000;

FTQCalendar::FTQCalendar(SST::Params& params) :
  num_ticks_epoch_(0),
  Parent(params)
{
  num_ticks_epoch_ = Timestamp(params.findUnits("epoch").toDouble()).ticks();
}

FTQCalendar::~FTQCalendar()
{
  sprockit::delete_vals(calendars_);
  calendars_.clear();
}

void
FTQCalendar::init(long nticks_per_epoch)
{
  num_ticks_epoch_ = nticks_per_epoch;
}

void
FTQCalendar::registerApp(int aid, const std::string& appname)
{
  static thread_lock lock;
  lock.lock();
  AppFTQCalendar*& cal = calendars_[aid];
  if (!cal){
    cal = new AppFTQCalendar(aid, appname, num_ticks_epoch_);
  }
  lock.unlock();
}

/**
void
AppFTQCalendar::globalReduce(ParallelRuntime* rt)
{
  sprockit::abort("app_ftq_calendar::global_reduce: not implemented");
  //make a big buffer
  uint64_t my_num_epochs = max_epoch_;
  uint64_t max_num_epochs = rt->globalMax(my_num_epochs);
  int num_keys = 0;//key::num_categories();
  uint64_t buffer_length = max_num_epochs * uint64_t(num_keys);

  uint64_t* reduce_buffer = new uint64_t[buffer_length];
  ::memset(reduce_buffer, 0, buffer_length * sizeof(uint64_t));
  uint64_t* bufptr = reduce_buffer;
  for (uint64_t i=0; i < my_num_epochs; ++i){
   FTQEpoch& my_epoch = epochs_[i];
   for (int k=0; k < num_keys; ++k, ++bufptr){
     *bufptr = my_epoch.eventTime(k);
   }
  }

  int root = 0;
  rt->globalSum(reduce_buffer, buffer_length, root);

  rt->globalSum(aggregate_.totals_, num_keys, root);

  if (rt->me() != root){
    delete[] reduce_buffer;
    return;
  }

  //now loop back through
  allocateEpochs(max_num_epochs);

  bufptr = reduce_buffer;
  for (long i=0; i < max_num_epochs; ++i){
   FTQEpoch& my_epoch = epochs_[i];
   for (int k=0; k < num_keys; ++k, ++bufptr){
     my_epoch.setEventTime(k, *bufptr);
   }
  }

  delete[] reduce_buffer;

  max_epoch_ = max_num_epochs;
}

void
FTQCalendar::globalReduce(ParallelRuntime *rt)
{
  if (rt->nproc() == 1)
    return;

  std::unordered_map<int, AppFTQCalendar*>::iterator it, end = calendars_.end();
  for (it=calendars_.begin(); it != end; ++it){
    AppFTQCalendar* cal = it->second;
    cal->globalReduce(rt);
  }
}
*/

void
AppFTQCalendar::reduce(AppFTQCalendar* cal)
{
  /**
  int num_keys = key::num_categories();
  int num_epochs = cal->epochs_.size();

  allocateEpochs(num_epochs); //make sure we have enough

  for (int e=0; e < num_epochs; ++e){
    ftq_epoch& my_epoch = epochs_[e];
    ftq_epoch& his_epoch = cal->epochs_[e];
    for (int k=0; k < num_keys; ++k){
      my_epoch.collect(k, his_epoch.eventTime(k));
    }
  }

  for (int k=0; k < num_keys; ++k){
    totals_[k] += cal->totals_[k];
  }
  */
}

#if 0
void
FTQCalendar::reduce(StatCollector* coll)
{
  //nothing to do for now... we are using statics
  /**
  ftq_calendar* other = safe_cast(ftq_calendar, coll);
  std::unordered_map<int, app_ftq_calendar*>::iterator it, end = other->calendars_.end();
  for (it=other->calendars_.begin(); it!= end; ++it){
    int appnum = it->first;
    calendars_[appnum]->reduce(it->second);
  }
  */
}

void
FTQCalendar::dumpLocalData()
{
}

void
FTQCalendar::dumpGlobalData()
{
  std::unordered_map<int, AppFTQCalendar*>::iterator it, end = calendars_.end();
  for (it=calendars_.begin(); it != end; ++it) {
    it->second->dump(fileroot_);
  }
}
#endif

void
FTQCalendar::addData_impl(int event_typeid, int aid, int tid, uint64_t ticks_begin,
                     uint64_t num_ticks)
{
  static thread_lock lock;
  lock.lock();
  calendars_[aid]->collect(event_typeid, tid, ticks_begin, num_ticks);
  lock.unlock();
}

AppFTQCalendar*
FTQCalendar::getCalendar(int aid) const
{
  auto it = calendars_.find(aid);
  if (it == calendars_.end()) {
    spkt_throw_printf(sprockit::value_error, "no FTQ calendar found for app %d", aid);
  }
  return it->second;
}

AppFTQCalendar::AppFTQCalendar(int aid, const std::string& appname,
                                   uint64_t nticks_epoch)
  : epochs_(0),
    aid_(aid),
    max_tid_(0),
    max_epoch_(0),
    max_epoch_allocated_(0),
    num_ticks_epoch_(nticks_epoch),
    appname_(appname)
{
  int num_categories = FTQTag::num_categories();
  aggregate_.totals_ = new uint64_t[num_categories];
  for (int i=0; i < num_categories; ++i) {
    aggregate_.totals_[i] = 0;
  }
}

AppFTQCalendar::~AppFTQCalendar()
{
  sprockit::delete_all(buffers_);
  delete aggregate_.totals_;
}

void
AppFTQCalendar::allocateEpochs(uint64_t max_epoch)
{
  while (max_epoch >= max_epoch_allocated_) {
    uint64_t epoch_start = max_epoch_allocated_;
    max_epoch_allocated_ += allocation_num_epochs;
    epochs_.resize(max_epoch_allocated_);

    int num_categories = FTQTag::num_categories();
    uint64_t* buffer = new uint64_t[num_categories * allocation_num_epochs];
    uint64_t* buffer_ptr = buffer;
    for (long epoch=epoch_start; epoch < max_epoch_allocated_;
         buffer_ptr += num_categories, ++epoch) {
      epochs_[epoch].init(num_categories, buffer_ptr);
    }
    buffers_.push_back(buffer);
  }
}

void
AppFTQCalendar::collect(int event_typeid, int tid, uint64_t ticks_begin,
                        uint64_t num_ticks)
{
  /** aggregate for all time intervals and all threads for given event type */
  aggregate_.totals_[event_typeid] += num_ticks;
  uint64_t ticks_end = ticks_begin + num_ticks;
  uint64_t max_epoch = ticks_end / num_ticks_epoch_ +
                   1; //just always assume a remainder
  allocateEpochs(max_epoch);

  uint64_t first_epoch = ticks_begin / num_ticks_epoch_;
  uint64_t nticks_first_epoch = num_ticks_epoch_ - ticks_begin % num_ticks_epoch_;
  // this might not go until the end of the epoch
  nticks_first_epoch = std::min(nticks_first_epoch, num_ticks);
  /** aggregate for all threads for given time interval and event type */
  epochs_[first_epoch].collect(event_typeid, nticks_first_epoch);
  num_ticks -= nticks_first_epoch;
  //t_epoch[epoch].collect(event_typeid, nticks_next_epoch);
  for (uint64_t epoch=first_epoch+1; epoch <= max_epoch; ++epoch) {
    uint64_t nticks_this_epoch = std::min(num_ticks, num_ticks_epoch_);
    epochs_[epoch].collect(event_typeid, nticks_this_epoch);
    //t_epoch[epoch].collect(event_typeid, nticks_next_epoch);
    num_ticks -= nticks_this_epoch;
  }

  if (max_epoch > max_epoch_) {
    max_epoch_ = max_epoch;
  }

}

static const char* matplotlib_histogram_text_header =
    "#!/usr/bin/env python3\n"
    "\n"
    "try:\n"
    "    import sys\n"
    "    import numpy as np\n"
    "    import matplotlib.pyplot as plt\n"
    "    import argparse\n"
    "except ImportError:\n"
    "    print('ImportError caught. Please install matplotlib')\n"
    "    exit()\n"
    "\n"
    "import numpy as np\n"
    "import matplotlib.pyplot as plt\n"
    "import argparse\n"
    "\n"
    "# Getting CLI args\n"
    "parser = argparse.ArgumentParser()\n"
    "parser.add_argument('--show', action='store_true', help='display the plot on screen')\n"
    "parser.add_argument('--title', default='Histogram plot', help='set the title')\n"
    "parser.add_argument('--eps', action='store_true', help='output .eps file')\n"
    "parser.add_argument('--pdf', action='store_true', help='output .pdf file')\n"
    "parser.add_argument('--png', action='store_true', help='output .png file')\n"
    "parser.add_argument('--svg', action='store_true', help='output .svg file')\n"
    "args = parser.parse_args()\n"
    "\n"
    "# Parsing the data file\n"
    "file_name='";

static const char* matplotlib_histogram_text_footer =
    "'\n"
    "with open(file_name + '.dat') as f:\n"
    "    names = f.readline().split()\n"
    "    data = np.loadtxt(f, dtype=float).transpose()\n"
    "    time, normalized = data[0], np.divide(data[1:-1],data[-1])\n"
    "\n"
    "# Plot fomatting\n"
    "plt.xlabel('Time (us)')\n"
    "plt.xlim(time[0], time[-1])\n"
    "plt.ylim(0,1)\n"
    "plt.yticks([])\n"
    "plt.title(args.title)\n"
    "polys = plt.stackplot(time, normalized)\n"
    "legendProxies = []\n"
    "for poly in polys:\n"
    "   legendProxies.append(plt.Rectangle((0, 0), 1, 1, fc=poly.get_facecolor()[0]))\n"
    "plt.legend(legendProxies, names[1:])\n"
    "\n"
    "# Saving\n"
    "if args.eps: plt.savefig(file_name + '.eps')\n"
    "if args.pdf: plt.savefig(file_name + '.pdf')\n"
    "if args.png: plt.savefig(file_name + '.png')\n"
    "if args.svg: plt.savefig(file_name + '.svg')\n"
    "\n"
    "if args.show:\n"
    "    plt.show()\n";

void
AppFTQCalendar::dumpMatplotlibHistogram(const std::string& fileroot)
{
  std::string fname_prefix = sprockit::printf("%s_app%d", fileroot.c_str(), aid_);
  std::string fname = sprockit::printf("%s.py", fname_prefix.c_str());
  std::ofstream out(fname.c_str());
  out << matplotlib_histogram_text_header << fname_prefix << matplotlib_histogram_text_footer;
  out.close();
}

void
AppFTQCalendar::dump(const std::string& fileroot)
{
  int num_categories = FTQTag::num_categories();
  std::string fname = sprockit::printf("%s_app%d.dat", fileroot.c_str(), aid_);
  std::ofstream out(fname.c_str());
  //print the first line header
  out << sprockit::printf("%12s", "Epoch(us)");

  //sort the categories
  std::map<std::string, int> sorted_keys;
  for (int i=0; i < num_categories; ++i){
    sorted_keys[FTQTag::name(i)] = i;
  }

  int nonzero_categories[num_categories];
  int num_nonzero_cats = 0;
  std::map<std::string,int>::iterator it, end = sorted_keys.end();
  for (it=sorted_keys.begin(); it != end; ++it){
    int i = it->second;
    std::string name = it->first;
    if (aggregate_.totals_[i] > 0) {
      out << sprockit::printf(" %12s", name.c_str());
      nonzero_categories[num_nonzero_cats] = i;
      ++num_nonzero_cats;
    }
  }

  out << sprockit::printf(" %12s", "Total");

  std::stringstream sstr;
  sstr << "\n";
  Timestamp one_ms(1e-3);
  int64_t ticks_ms = one_ms.ticks();
  for (int ep=0; ep < max_epoch_; ++ep) {
    //figure out how many us
    double num_ms = double(ep * num_ticks_epoch_) / (double) ticks_ms;
    sstr << sprockit::printf("%12.4f", num_ms);
    long total_ticks = 0;
    for (int i=0; i < num_nonzero_cats; ++i) {
      int cat_id = nonzero_categories[i];
      long num_ticks = epochs_[ep].eventTime(cat_id);
      sstr << sprockit::printf(" %12ld", num_ticks);
      total_ticks += num_ticks;
    }
    sstr << sprockit::printf(" %12ld", total_ticks);
    sstr << "\n";
  }
  out << sstr.str();
  out.close();

  dumpMatplotlibHistogram(fileroot);

  Timestamp stamp_sec(1.0);
  int64_t ticks_s = stamp_sec.ticks();
  std::cout << sprockit::printf("Average time stats for application %s: \n",
                                     appname_.c_str());
  /** print the stat totals */
  int ntasks = max_tid_ + 1;

  end = sorted_keys.end();
  for (it=sorted_keys.begin(); it != end; ++it){
    int idx = it->second;
    std::string name = it->first;
    double av_per_app = (double) aggregate_.totals_[idx] / ntasks;
    double t_sec = av_per_app / (double) ticks_s;
    std::cout << sprockit::printf("%16s: %16.8f s\n", name.c_str(), t_sec);
  }
}

FTQEpoch::FTQEpoch()
  : totals_(nullptr)
{
}

FTQEpoch::~FTQEpoch()
{
}

void
FTQEpoch::init(int num_events, uint64_t *buffer)
{
  totals_ = buffer;
  for (int i=0; i < num_events; ++i) {
    totals_[i] = 0;
  }
}

// ftq_scope member functions
FTQScope::FTQScope(Thread* _thread, FTQTag _tag): _previous_tag(_thread->tag()) {
    this->_thread = _thread;
    _tag_previously_protected = _thread->protect_tag;

    // Ignoring nested tags is now an expected behavior
    //if (_thread->protect_tag == true) std::cerr << "WARNING: An 'FTQScope' is already active. Nested guards are ignored.";

    _thread->setTag(_tag);
    _thread->protect_tag = true;
}

FTQScope::FTQScope(Thread* _thread): FTQScope(_thread, _thread->tag()) {}


FTQScope::~FTQScope() {
    _thread->protect_tag = _tag_previously_protected;
    _thread->setTag(_previous_tag);
}

void* FTQScope::operator new(size_t size) throw() {
    return nullptr;
}


}
}
