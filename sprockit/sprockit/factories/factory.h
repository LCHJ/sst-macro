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

#ifndef SPROCKIT_COMMON_FACTORIES_FACTORY_H_INCLUDED
#define SPROCKIT_COMMON_FACTORIES_FACTORY_H_INCLUDED

#include <cstdio>
#include <iostream>
#include <sprockit/errors.h>
#include <sprockit/statics.h>
#include <sprockit/sim_parameters_fwd.h>
#include <sprockit/basic_string_tokenizer.h>
#include <map>

namespace sprockit {

/**
 *  Object that will actually build classes from the given list of arguments
 */
template <class T, typename... Args>
class Builder
{
 public:
  virtual T* build(Args... args) = 0;

};

#include <type_traits>

template <class T> T& getLValue();
template <class T> T&& getRValue();

template <class> struct wrap { typedef void type; };

/**
 * @class call_finalizeInit
 * finalizeInit is not a required method for any of the classes.
 * This uses SFINAE tricks to call finalizeInit on classes that implement
 * the method or avoid calling it on classes that don't implement it
 */
template<typename T, class Enable=void>
struct callFinalizeInit : public std::false_type {
 public:
  void operator()(T* t, SST::Params& params){
    //if we compile here, class T does not have a finalizeInit
    //do nothing
  }
};



template<typename T>
struct callFinalizeInit<T,
  typename wrap<
    decltype(std::declval<T>().finalizeInit(getLValue<SST::Params>()))
   >::type
> : public std::true_type
{
 public:
  void operator()(T* t, SST::Params& params){
    //if we compile here, class T does have a finalizeInit
    //call that function
    t->finalizeInit(params);
  }
};

template <typename T, class... Args>
struct startCallFinalizeInit {
  void operator()(T* t, Args&&...args){}
};

template <typename T, class... Args>
struct startCallFinalizeInit<T,SST::Params&,Args...> {
  void operator()(T* t, SST::Params& params, Args&&... args){
    callFinalizeInit<T>()(t, params);
  }
};


template <class Factory>
class CleanupFactory {
 public:
  ~CleanupFactory(){
    Factory::cleanUp();
  }
};

/**
 * @class Factory
 * Object that provides static methods for mapping string names
 * to particular subclasses of T.  The constructor for each subclass U of T
 * must be either U(Args) or it must be U(params,Args) where params is a
 * params object
 */
template<class T, typename... Args>
class Factory
{

 public:
  typedef Builder<T,Args...> builder_t;
  typedef T element_type;

  typedef std::map<std::string, builder_t*> builder_map;
  static builder_map* builder_map_;

  typedef std::map<std::string, std::list<std::string> > alias_map;
  static alias_map* alias_map_;

  static CleanupFactory<Factory<T,Args...>> clean_up_;

  static const char* name_;

  /**
   * @brief register_alias Explicitly register an alias name for a
   * factory based on a previous
   * @param oldname The name currently registered or that will be registered
   *                depending on static init order
   * @param newname The new name that can be used for accessing child type
   */
  static void registerAlias(const std::string& oldname, const std::string& newname){
    if (!builder_map_) {
      builder_map_ = new builder_map;
    }
    if (!alias_map_){
      alias_map_ = new alias_map;
    }

    builder_t* base = (*builder_map_)[oldname];
    if (!base){
      (*alias_map_)[oldname].push_back(newname);
    } else {
      (*builder_map_)[newname] = base;
    }
  }

  static void cleanUp(){
    //do not iterate the builder map and delete entry
    //each builder_t is a static objec that gets cleaned up automatically

    if (builder_map_) delete builder_map_;
    if (alias_map_) delete alias_map_;

    builder_map_ = nullptr;
    alias_map_ = nullptr;
  }

  /**
   * @brief register_name
   * @param name  The string name that will map to the given type
   * @param builder The builder object whose virtual functio returns
   *                the correct child type
   */
  static void registerName(const std::string& name, builder_t* builder) {
    if (!builder_map_) {
      builder_map_ = new builder_map;
    }
    if (!alias_map_){
      alias_map_ = new alias_map;
    }
    addToMap(name, builder, builder_map_, alias_map_);
  }

 private:
  /**
   * @brief add_to_map
   * @param namestr
   * @param desc
   * @param descr_map
   * @param alias_map
   */
  static void addToMap(const std::string& namestr, builder_t* desc,
            std::map<std::string, builder_t*>* builder_map,
            std::map<std::string, std::list<std::string> >* alias_map) {
    // The namestr can be a | separate list of valid names, e.g.
    // "simple | LogP" to allow either name to map to the correct type
    std::string space = "|";
    std::deque<std::string> tok;
    pst::BasicStringTokenizer::tokenize(namestr, tok, space);
    for (auto& name : tok) {
      name = trim_str(name);
      auto it = alias_map->find(name);
      if (it != alias_map->end()){
        std::list<std::string>& alias_list = it->second;
        for (auto& alias : alias_list){
          (*builder_map_)[alias] = desc;
        }
      }
      (*builder_map_)[name] = desc;
    }
  }


  /**
   * @brief _get_value  Helper function that builds the correct child type
   *                    from the corresponding string name
   * @param valname     The string name mapping to a particular child type
   * @param params      The parameters potentially used in the constructor
   * @param args        The required arguments for the constructor
   * @return  A constructed child class corresponding to a given string name
   */
  template <class... InArgs>
  static T* _get_value(const std::string& valname,
             InArgs&&... args) {
    if (!builder_map_) {
      spkt_abort_printf(
           "could not find name %s for factory %s. no classes are registered",
           valname.c_str(),
           name_);
    }

    auto it = builder_map_->find(valname), end = builder_map_->end();
    if (it == end) {
      std::cerr << "Valid factories are:" << std::endl;
      printValidNames(std::cerr);
      spkt_abort_printf("could not find name %s for factory %s",
                       valname.c_str(), name_);
    }

    builder_t* builder = it->second;
    if (!builder) {
      spkt_abort_printf("initialized name %s with null builder for factory %s",
                       valname.c_str(), name_);
    }
    T* p = builder->build(std::forward<InArgs>(args)...);
    startCallFinalizeInit<T,InArgs...>()(p, std::forward<InArgs>(args)...);
    return p;
  }

 public:
  template <class Params>
  static bool isValidParam(const std::string& name, Params& params) {
    std::string value = params.template find<std::string>(name);
    return builder_map_->find(value) != builder_map_->end();
  }

  static bool isValidValue(const std::string& value) {
    return builder_map_->find(value) != builder_map_->end();
  }

  static const char* name() { return name_; }

  /**
   * @brief get_value Return a constructed child class corresponding
   *                  to a given string name
   * @param valname   The string name mapping to a particular child type
   * @param params    The parameters potentially used in the constructor
   * @param args      The required arguments for the constructor
   * @return  A constructed child class corresponding to a given string name
   */
  template <class... InArgs>
  static T* getValue(const std::string& valname,
            InArgs&&... args) {
    return _get_value(valname, std::forward<InArgs>(args)...);
  }

  static void printValidNames(std::ostream& os){
    for (auto& pair : *builder_map_){
      os << pair.first << "\n";
    }
  }

  static bool isValidName(const std::string& name){
    return builder_map_->find(name) != builder_map_->end();
  }

  /**
   * @brief get_param Return a constructed child class corresponding
   *                  to a given string name. The string name is not given directly,
   *                  instead being found by params.find<std::string>(param_name)
   * @param param_name   The name of the parameter such that params.find<std::string>(param_name)
   *                     returns the string that will map to the child class
   * @param params    The parameters potentially used in the constructor
   * @param args      The required arguments for the constructor
   * @return  A constructed child class corresponding to a given string name
   */
  template <class Params, class... InArgs>
  static T* getParam(const std::string& param_name,
                      Params& params, InArgs&&... args) {
    return _get_value(params.template find<std::string>(param_name),
                      params, std::forward<InArgs>(args)...);
  }


  /**
   * @brief get_extra param Return a constructed child class corresponding
   *          to a given string name. The string name is not given directly,
   *          instead being found by params.find<std::string>(param_name).  If no parameter
   *          corresponding to param_name exists, return get_value(defval)
   * @param param_name   The name of the parameter such that params.find<std::string>(param_name)
   *                     returns the string that will map to the child class
   * @param defval    The default value to use in case the parameter has not been specified
   * @param params    The parameters potentially used in the constructor
   * @param args      The required arguments for the constructor
   * @return  A constructed child class corresponding to a given string name
   */
  template <class Params, class... InArgs>
  static T* getOptionalParam(const std::string& param_name,
                     const std::string& defval,
                     Params& params, InArgs&&... args) {
    return _get_value(params.template find<std::string>(param_name, defval),
                      params, std::forward<InArgs>(args)...);
  }

};

template<class T, typename... Args> const char* Factory<T,Args...>::name_ = T::factoryName();
template<class T, typename... Args> std::map<std::string, typename Factory<T,Args...>::builder_t*>*
   Factory<T,Args...>::builder_map_ = nullptr;
template<class T, typename... Args> std::map<std::string, std::list<std::string>>*
  Factory<T,Args...>::alias_map_ = nullptr;
template<class T, typename... Args> CleanupFactory<Factory<T,Args...>>
  Factory<T,Args...>::clean_up_ = nullptr;

template<class Child, typename Parent, typename... Args>
class BuilderImpl : public Builder<Parent, Args...>
{
 public:
  BuilderImpl(const char *name){
    Factory<Parent, Args...>::registerName(name, this);
    registered_ = true;
  }

  Parent* build(Args... args) {
    return new Child(std::forward<Args>(args)...);
  }

  static bool isRegistered() {
    return registered_;
  }

 private:
  static bool registered_;

};
template <class Child, class Parent, class... Args> bool BuilderImpl<Child,Parent,Args...>::registered_ = false;

template <class Child, class Factory>
class BuilderRegistration {
};

template <class Child, class Parent, class... Args>
class BuilderRegistration<Child, Factory<Parent,Args...>>
{
 public:
  static bool builderRegistered(){ return builder_.isRegistered(); }

 private:
  static BuilderImpl<Child,Parent,Args...> builder_;
};

template <class Child, class Parent, class... Args> BuilderImpl<Child,Parent,Args...>
  BuilderRegistration<Child,Factory<Parent,Args...>>::builder_(Child::factoryString());

}

#define ArgStr(X) #X
#define ArgFactoryName(X) X##_factory

#define FactoryRegister(cls_str, parent_cls, child_cls, ...) \
  friend class ::sprockit::BuilderImpl<child_cls,typename parent_cls::factory>; \
  public: \
   static const char* factoryString() { \
     return cls_str; \
   } \
   static bool factoryRegistered() { \
     return ::sprockit::BuilderRegistration<child_cls,parent_cls::factory>::builderRegistered(); \
   }

#define DeclareFactoryArgs(T, ...) \
  public: \
   friend class ::sprockit::Factory<T,SST::Params&,__VA_ARGS__>; \
    using factory = ::sprockit::Factory<T,SST::Params&,__VA_ARGS__>; \
    static const char* className(){ \
      return factory::name(); \
    } \
    static const char* factoryName(){ \
      return ArgStr(T); \
    }

#define DeclareFactory(T) \
  public: \
   friend class ::sprockit::Factory<T,SST::Params&>; \
    using factory = ::sprockit::Factory<T,SST::Params&>; \
    static const char* className(){ \
      return factory::name(); \
    } \
    static const char* factoryName(){ \
      return ArgStr(T); \
    }

#define DeclareSimpleFactory(T,...) \
  public: \
   friend class ::sprockit::Factory<T,__VA_ARGS__>; \
    using factory = ::sprockit::Factory<T,__VA_ARGS__>; \
    static const char* className(){ \
      return factory::name(); \
    } \
    static const char* factoryName(){ \
      return ArgStr(T); \
    }

#endif
