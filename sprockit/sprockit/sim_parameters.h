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

#ifndef SPROCKIT_COMMON_SIM_PARAMETERS_H_INCLUDED
#define SPROCKIT_COMMON_SIM_PARAMETERS_H_INCLUDED

#include <sprockit/spkt_config.h>
#include <sprockit/debug.h>
#include <sprockit/sim_parameters_fwd.h>
#include <unordered_map>

#include <sstream>
#include <iostream>
#include <list>
#include <vector>
#include <set>

#include <sprockit/basic_string_tokenizer.h>

DeclareDebugSlot(params)
DeclareDebugSlot(read_params)
DeclareDebugSlot(write_params)


namespace sprockit {

bool get_quantity_with_units(const char *value, double& ret);
double get_quantity_with_units(const char *value, const char* key);

class param_assign {
 public:
  param_assign(std::string& p, const std::string& k) :
    param_(p), key_(k)
  {
  }

  void operator=(int a);
  void operator=(double x);
  void operator=(const std::string& str){
    param_ = str;
  }

  const std::string& setByteLength(long x, const char* units);
  const std::string& setBandwidth(double x, const char* units);
  const std::string& setFrequency(double x, const char* units);
  const std::string& setTime(double x, const char* units);
  const std::string& setValue(double x, const char* units);
  const std::string& set(const char* str);
  const std::string& set(const std::string& str);

  long getByteLength() const;
  double getBandwidth() const;
  double getTime() const;
  double getFrequency() const;

  operator int() const;

  operator double() const;

  operator std::string() const {
    return param_;
  }

 private:
  std::string& param_;
  const std::string& key_;

};

class param_bcaster {
 public:
  virtual void bcast(void* buf, int size, int me, int root) = 0;

  void bcastString(std::string& str, int me, int root);
};

class sim_parameters  {
 public:
  friend class SST::Params;

  using ptr = std::shared_ptr<sim_parameters>;
  using const_ptr = std::shared_ptr<const sim_parameters>;

  struct parameter_entry
  {
    parameter_entry() : read(false) {}
    std::string value;
    bool read;
  };


  static sim_parameters::ptr& empty() {
    return empty_ns_params_;
  }

  void reproduce_params(std::ostream& os) const ;

  typedef std::unordered_map<std::string, parameter_entry> key_value_map;

  sim_parameters();

  sim_parameters(const key_value_map& p);

  sim_parameters(const std::string& filename);

  sim_parameters(sprockit::sim_parameters::const_ptr params); //deep copy

  /**
   * In a parallel environment (abstracted through a param_bcaster object),
   * have rank 0 read params from a file and bcast result to all other ranks
   * @param params  A parameter object (already allocated)
   * @param me  The rank of the calling process
   * @param nproc The total number of ranks
   * @param filename
   * @param bcaster
   */
  static void
  parallel_build_params(sprockit::sim_parameters::ptr& params,
                        int me, int nproc,
                        const std::string& filename,
                        param_bcaster* bcaster,
                        bool fail_if_not_found = true);

  virtual ~sim_parameters();

  bool public_scope() const {
    return public_scope_;
  }

  void set_public_scope(bool flag){
    public_scope_ = flag;
  }

  /**
   * Needed in conjunction with moved constructors to clear map
   * and keep things from getting deleted
   */
  void moved();

  void remove_param(const std::string &key);

  std::string get_variable(const std::string& str);

  std::string get_param(const std::string& key, bool throw_on_error = true);

  std::string get_lowercase_param(const std::string& key, bool throw_on_error = true);

  std::string get_scoped_param(const std::string& key, bool throw_on_error = true);

  sim_parameters::ptr get_optional_local_namespace(const std::string& ns);

  std::string reread_param(const std::string& key) {
    return get_param(key);
  }

  std::string reread_optional_param(const std::string& key, const std::string& def) {
    return get_optional_param(key, def);
  }

  /// Return the value of the keyword if it exists. Otherwise return
  /// a default value.
  /// @param key gives the keyword
  /// @param def gives the default value (used if has_param(key) is false)
  /// @return the value if it exists, otherwise the default
  std::string get_optional_param(const std::string &key, const std::string &def);

  std::string deprecated_optional_param(const std::string &key, const std::string &def);

  std::string deprecated_param(const std::string& key);

  /**
   * @brief get_either_or_param Return a parameter which can have one of two names.
   *        If both names are found, results in error.
   * @param key1
   * @param key2
   * @return The parameter value for key1 or key2
   */
  std::string get_either_or_param(const std::string& key1, const std::string& key2);

  int get_either_or_int_param(const std::string& key1, const std::string& key2);

  double get_either_or_time_param(const std::string& key1, const std::string& key2);

  double get_either_or_bandwidth_param(const std::string& key1, const std::string& key2);

  void add_param(const std::string& key, const std::string& val);

  void copy_param(const std::string& oldname, const std::string& newname);

  void copy_optional_param(const std::string& oldname, const std::string& newname);

  void add_param_override(const std::string& key, const std::string& val);

  void add_param_override(const std::string &key, double val);

  void add_param_override(const std::string& key, double val, const char* units);

  void add_param_override(const std::string& key, int val);

  void add_param_override_recursive(const std::string& key, int val);

  void add_param_override_recursive(const std::string& key, const std::string& val);

  void combine_into(sim_parameters::ptr sp,
               bool fail_on_existing = false,
               bool override_existing = true,
               bool mark_as_read = true);

  std::string print_scoped_params(std::ostream& os) const;

  std::string print_scopes(std::ostream& os);

  void print_local_params(std::ostream& os, const std::string& prefix) const;

  void print_params(std::ostream& os = std::cerr, const std::string& prefix = "") const;

  bool print_unread_params(std::ostream& os = std::cerr) const;

  bool has_param(const std::string& key) const;

  bool has_scoped_param(const std::string& key) const;

  int get_int_param(const std::string& key);

  int deprecated_int_param(const std::string& key);

  int deprecated_optional_int_param(const std::string &key, int def);

  int reread_int_param(const std::string& key);

  int reread_optional_int_param(const std::string& key, int def);

  /// Return the value of the keyword if it exists. Otherwise return
  /// a default value.
  /// @param key gives the keyword
  /// @param def gives the default value (used if has_param(key) is false)
  /// @return the value if it exists, otherwise the default
  int get_optional_int_param(const std::string &key, int def);

  /// Returns the value of the key as a boolean.
  bool get_bool_param(const std::string &key);

  bool reread_bool_param(const std::string& key);

  bool deprecated_bool_param(const std::string& key);

  bool deprecated_optional_bool_param(const std::string& key, bool def);

  bool reread_optional_bool_param(const std::string& key, bool def);

  /// Return the value of the keyword if it exists. Otherwise return
  /// a default value.
  /// @param key gives the keyword
  /// @param def gives the default value (used if has_param(key) is false)
  /// @return the value if it exists, otherwise the default
  bool get_optional_bool_param(const std::string &key, bool def);

  double get_bandwidth_param(const std::string& key);

  /**
   @param key The parameter name
   @return A value with units. This loops through bandwidth, frequency, time, etc
           to return any value that can have units. Everything is normalized to baseslines
           of B/s, s, Hz, etc
  */
  double get_quantity(const std::string& key);

  double get_optional_quantity(const std::string& key, double def);

  double deprecated_bandwidth_param(const std::string& key);

  double reread_bandwidth_param(const std::string& key);

  double reread_optional_bandwidth_param(const std::string& key, double def);

  double deprecated_optional_bandwidth_param(const std::string& key, double def);

  double get_optional_bandwidth_param(const std::string &key, double def);

  double get_optional_bandwidth_param(
    const std::string& key,
    const std::string& def);

  long get_byte_length_param(const std::string& key);

  long get_optional_byte_length_param(const std::string& key, long def);

  long deprecated_byte_length_param(const std::string& key);

  long deprecated_optional_byte_length_param(const std::string& key, long def);

  double get_optional_freq_param(const std::string& key, double def);

  double get_freq_param(const std::string& key);

  double deprecated_freq_param(const std::string& key);

  double deprecated_optional_freq_param(const std::string& key, double def);

  /// Return the value of the keyword if it exists. Otherwise return
  /// a default value.
  /// @param key gives the keyword
  /// @param def gives the default value (used if has_param(key) is false)
  /// @return the value if it exists, otherwise the default
  long get_optional_long_param(const std::string &key, long def);

  long get_long_param(const std::string& key);

  long deprecated_long_param(const std::string& key);

  long deprecated_optional_long_param(const std::string& key, long def);

  long reread_long_param(const std::string& key);

  void reread_optional_long_param(const std::string& key);

  double get_double_param(const std::string& key);

  /// Return the value of the keyword if it exists. Otherwise return
  /// a default value.
  /// @param key gives the keyword
  /// @param def gives the default value (used if has_param(key) is false)
  /// @return the value if it exists, otherwise the default
  double get_optional_double_param(const std::string &key, double def);

  double reread_double_param(const std::string& key);

  double reread_optional_double_param(const std::string &key, double def);

  double deprecated_double_param(const std::string& key);

  double deprecated_optional_double_param(const std::string& key, double def);

  double get_time_param(const std::string& key);

  double get_optional_time_param(const std::string &key, double def);

  double deprecated_optional_time_param(const std::string &key, double def);

  double deprecated_time_param(const std::string& key);

  template <class T> void get_vector_param(const std::string& key, std::vector<T>& vals){
    std::deque<std::string> toks = get_tokenizer(key);
    for (auto& item : toks){
      if (item.size() > 0) {
        std::stringstream sstr(item);
        T val;
        sstr >> val;
        vals.push_back(val);
      }
    }
  }

  sim_parameters::ptr get_namespace(const std::string& ns);

  sim_parameters::ptr get_optional_namespace(const std::string& ns);

  void set_namespace(const std::string& ns, sim_parameters::ptr params){
    subspaces_[ns] = params;
  }

  bool has_namespace(const std::string& ns) const;

  void parse_file(const std::string& fname, bool fail_on_existing,
             bool override_existing, bool fail_if_not_found = true);

  void parse_stream(std::istream& in, bool fail_on_existing, bool override_existing);

  void parse_line(const std::string& line, bool fail_on_existing, bool override_existing);

  /**
    @param key
    @param value
    @param fail_on_existing Fail if the parameter named by key already exists
  */
  void parse_keyval(const std::string& key,
    const std::string& value,
    bool fail_on_existing,
    bool override_existing,
    bool mark_as_read);

  /**
   * This is a dirty hack to store non-sprockit parameter objects
   * on this parameter object
   */
  void append_extra_data(void* data) {
    extra_data_ = data;
  }

  template <class T>
  T* extra_data() const {
    void* ptr = _extra_data();
    return static_cast<T*>(ptr);
  }

  param_assign operator[](const std::string& key);

  key_value_map::iterator begin() { return params_.begin(); }
  key_value_map::const_iterator begin() const { return params_.begin(); }

  key_value_map::iterator end() { return params_.end(); }
  key_value_map::const_iterator end() const { return params_.end(); }

  using namespace_iterator = std::map<std::string, sim_parameters::ptr>::iterator ;
  using const_namespace_iterator = std::map<std::string, sim_parameters::ptr>::const_iterator;
  namespace_iterator ns_begin() { return subspaces_.begin(); }
  const_namespace_iterator ns_begin() const { return subspaces_.begin(); }
  namespace_iterator ns_end() { return subspaces_.end(); }
  const_namespace_iterator ns_end() const { return subspaces_.end(); }

 private:
  std::map<std::string, sim_parameters::ptr> subspaces_;
  std::map<std::string, std::string> variables_;

  /**
   * @brief check_either_or Determine whether the parameters are valid for either/or
   *        parameters. If both keys are given or neither are given, abort.
   *        There should only be a single key specified.
   * @param key1
   * @param key2
   * @return True if key1 is available, False if key2 is available.
   */
  bool check_either_or(const std::string& key1,
                       const std::string& key2);

  sim_parameters* parent_;

  void* _extra_data() const;
  void* extra_data_;

  static sim_parameters::ptr empty_ns_params_;

  std::string namespace_;

  key_value_map params_;

  uint64_t current_id_;

  bool public_scope_;

  /**
   * @brief _get_namespace Get a parameter namespace. If the namespace does not exist in the current scope locally,
   *  search through any parent namespaces. This follow C++ scoping rules as if you had requested ns::variable.
   * @param ns The namespace to get
   * @return The set of all parameters in a given param namespace
   */
  sim_parameters::ptr _get_namespace(const std::string &ns);

  sim_parameters::ptr build_local_namespace(const std::string& ns);

  void throw_key_error(const std::string& key) const;

  void set_parent(sim_parameters* p) {
    parent_ = p;
  }

  void set_namespace(const std::string& str) {
    namespace_ = str;
  }

  bool local_has_namespace(const std::string& ns) const {
    return subspaces_.find(ns) != subspaces_.end();
  }

  void split_line(const std::string& line, std::pair<std::string, std::string>& p);

  void try_to_parse(const std::string& fname, bool fail_on_existing, bool override_existing);

  void print_params(const key_value_map& pmap, std::ostream& os, bool pretty_print, std::list<std::string>& ns) const;

  void do_add_param(const std::string& key,
    const std::string& val,
    bool fail_on_existing,
    bool override_existing,
    bool mark_as_read);


  sim_parameters* get_scope_and_key(const std::string& key, std::string& final_key);

  bool get_param(std::string& inout, const std::string& key);

  bool get_scoped_param(std::string& inout, const std::string& key);

  std::deque<std::string> get_tokenizer(const std::string& key);

};

}

namespace SST {

template <class T>
struct CallGetParam {};

template <> struct CallGetParam<long>  {
  static double get(sprockit::sim_parameters::ptr& ptr, const std::string& key){
    return ptr->get_long_param(key);
  }
  static double getOptional(sprockit::sim_parameters::ptr &ptr, const std::string& key, long def){
    return ptr->get_optional_long_param(key, def);
  }
};

template <> struct CallGetParam<double>  {
  static double get(sprockit::sim_parameters::ptr& ptr, const std::string& key){
    return ptr->get_double_param(key);
  }
  static double getOptional(sprockit::sim_parameters::ptr &ptr, const std::string& key, double def){
    return ptr->get_optional_double_param(key, def);
  }
};

template <> struct CallGetParam<int>  {
  static int get(sprockit::sim_parameters::ptr& ptr, const std::string& key){
    return ptr->get_int_param(key);
  }
  static int getOptional(sprockit::sim_parameters::ptr &ptr, const std::string& key, int def){
    return ptr->get_optional_int_param(key, def);
  }
};

template <> struct CallGetParam<bool>  {
  static int get(sprockit::sim_parameters::ptr& ptr, const std::string& key){
    return ptr->get_bool_param(key);
  }
  static int getOptional(sprockit::sim_parameters::ptr &ptr, const std::string& key, bool def){
    return ptr->get_optional_bool_param(key, def);
  }
};

template <> struct CallGetParam<std::string> {
  static std::string get(sprockit::sim_parameters::ptr& ptr, const std::string& key){
    return ptr->get_param(key);
  }

  static std::string getOptional(sprockit::sim_parameters::ptr& ptr,
                                 const std::string& key, std::string&& def){
    return ptr->get_optional_param(key, def);
  }

  static std::string getOptional(sprockit::sim_parameters::ptr& ptr,
                                 const std::string& key, const std::string& def){
    return ptr->get_optional_param(key, def);
  }

};

struct UnitAlgebra
{
 public:
  UnitAlgebra(const std::string& val){
    double tmp;
    bool failed = sprockit::get_quantity_with_units(val.c_str(), tmp);
    if (failed){
      std::cerr << "Failed to parse value with units from " << val << std::endl;
      ::abort();
    }
    value_ = tmp;
  }

  UnitAlgebra inverse(){
    return UnitAlgebra(1.0/value_);
  }

  int64_t getRoundedValue() const {
    return value_;
  }

  const UnitAlgebra& getValue() const {
    return *this;
  }

  double toDouble() const {
    return value_;
  }

 private:
  UnitAlgebra(double v) : value_(v){}

  double value_;

};

class Params {
 public:
  Params() : params_(std::make_shared<sprockit::sim_parameters>())
  {
  }

  Params(const std::string& name) : params_(std::make_shared<sprockit::sim_parameters>(name))
  {
  }

  operator bool(){
    return bool(params_);
  }

  Params(const sprockit::sim_parameters::ptr& params) :
    params_(params)
  {
  }

  template <class T> void find_array(const std::string& key, std::vector<T>& vec){
    return params_->get_vector_param(key, vec);
  }

  bool contains(const std::string& k) const {
    return params_->has_param(k);
  }

  void print_all_params(std::ostream& os){
    params_->print_params(os);
  }

  sprockit::sim_parameters* operator->(){
    return params_.get();
  }

  SST::Params get_namespace(const std::string& name){
    return params_->get_namespace(name);
  }

  SST::Params find_prefix_params(const std::string& name){
    return params_->get_optional_namespace(name);
  }

  UnitAlgebra findUnits(const std::string& key){
    return UnitAlgebra(params_->get_param(key));
  }

  UnitAlgebra findUnits(const std::string& key, const std::string& def){
    return UnitAlgebra(params_->get_optional_param(key, def));
  }

  template <class T> T find(const std::string& key) {
    return CallGetParam<T>::get(params_, key);
  }

  template <class T, class U> T find(const std::string& key, U&& def) {
    return CallGetParam<T>::getOptional(params_, key, std::forward<U>(def));
  }

  sprockit::param_assign operator[](const std::string& key){
    return (*params_)[key];
  }

  sprockit::param_assign operator[](const char* key){
    return (*params_)[key];
  }

  void combine_into(SST::Params& params,
               bool fail_on_existing = false,
               bool override_existing = true,
               bool mark_as_read = true){
    params_->combine_into(params.params_,
      fail_on_existing,override_existing,mark_as_read);
  }

 private:
  sprockit::sim_parameters::ptr params_;
};

}


#endif
