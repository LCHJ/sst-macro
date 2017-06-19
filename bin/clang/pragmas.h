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

#ifndef bin_clang_pragmas_h
#define bin_clang_pragmas_h

#include "clangHeaders.h"
#include "util.h"
#include <set>

class SkeletonASTVisitor;

struct SSTReplacePragma;
struct SSTNullVariablePragma;
struct PragmaConfig {
  int pragmaDepth;
  bool makeNoChanges;
  std::map<std::string,SSTReplacePragma*> replacePragmas;
  std::map<clang::Decl*,SSTNullVariablePragma*> nullVariables;
  std::set<const clang::DeclRefExpr*> deletedRefs;
  SkeletonASTVisitor* astVisitor;
  PragmaConfig() : pragmaDepth(0),
    makeNoChanges(false) {}
};

struct SSTPragmaList;
struct SSTPragma {
  typedef enum {
    Replace=0,
    Delete=1,
    Compute=2,
    New=3,
    Keep=4,
    NullVariable=5,
    LoopCount=6,
    Init=7
  } class_t;
  clang::StringRef name;
  clang::SourceLocation startLoc;
  clang::SourceLocation endLoc;
  clang::CompilerInstance* CI;
  SkeletonASTVisitor* visitor;
  std::set<clang::Stmt*>* deleted;
  SSTPragmaList* pragmaList;
  class_t cls;


  template <class T>
  bool matches(T* s){
    return startLoc < s->getLocStart() && s->getLocStart() <= endLoc;
  }

  SSTPragma(class_t _cls) : cls(_cls){}

  virtual void activate(clang::Stmt* s, clang::Rewriter& r, PragmaConfig& cfg) = 0;
  virtual void activate(clang::Decl* d, clang::Rewriter& r, PragmaConfig &cfg){} //not required
  virtual void deactivate(clang::Stmt* s, PragmaConfig& cfg){} //not required

  static void tokenStreamToString(clang::SourceLocation loc,
      std::list<clang::Token>::const_iterator beg,
      std::list<clang::Token>::const_iterator end,
      std::ostream& os, clang::CompilerInstance& CI);

 protected:
  void replace(const clang::Stmt* s, clang::Rewriter& r, const std::string& repl);

};

class SSTNullVariablePragma : public SSTPragma {
 public:
  SSTNullVariablePragma(clang::SourceLocation loc,
                        clang::CompilerInstance& CI,
                        const std::list<clang::Token>& tokens);
  bool hasOnly() const {
    return !nullOnly_.empty();
  }

  bool hasExceptions() const {
    return !nullExcept_.empty();
  }

  bool isOnly(clang::NamedDecl* d) const {
    return nullOnly_.find(d->getNameAsString()) != nullOnly_.end();
  }

  bool isException(clang::NamedDecl* d) const {
    return nullExcept_.find(d->getNameAsString()) != nullExcept_.end();
  }

  bool keepCtor() const {
    return !nullOnly_.empty() || !nullExcept_.empty();
  }

 private:
  void activate(clang::Stmt* s, clang::Rewriter& r, PragmaConfig& cfg) override;
  void activate(clang::Decl* d, clang::Rewriter& r, PragmaConfig& cfg) override;
  void replace(clang::Stmt* s, clang::Rewriter& r, const char* repl);
  std::set<std::string> nullOnly_;
  std::set<std::string> nullExcept_;
};

class SSTDeletePragma : public SSTPragma {
 public:
  SSTDeletePragma() : SSTPragma(Delete) {}
 private:
  void activate(clang::Stmt* s, clang::Rewriter& r, PragmaConfig& cfg) override;
  void activate(clang::Decl* d, clang::Rewriter& r, PragmaConfig& cfg) override;
  void replace(clang::Stmt* s, clang::Rewriter& r, const char* repl);
};

class SSTMallocPragma : public SSTDeletePragma {
 private:
  void activate(clang::Stmt *stmt, clang::Rewriter &r, PragmaConfig& cfg) override;
  void visitDeclStmt(clang::DeclStmt* stmt, clang::Rewriter& r);
  void visitBinaryOperator(clang::BinaryOperator* op, clang::Rewriter& r);
};

class SSTKeepPragma : public SSTPragma {
 public:
  SSTKeepPragma() : SSTPragma(Keep) {}
 private:
  void activate(clang::Stmt *s, clang::Rewriter &r, PragmaConfig &cfg) override {
    cfg.makeNoChanges = true;
  }
  void deactivate(clang::Stmt *s, PragmaConfig &cfg) override {
    cfg.makeNoChanges = false;
  }
};

class SSTNewPragma : public SSTPragma {
 public:
  SSTNewPragma() : SSTPragma(New) {}
  static void defaultAct(clang::Stmt* stmt, clang::Rewriter& r, clang::CompilerInstance& CI,
                         bool insertStartAfter, bool insertStopAfter, bool trailingSemiColon=false);
 private:
  void activate(clang::Stmt *stmt, clang::Rewriter &r, PragmaConfig& cfg) override;
  void activate(clang::Decl* d, clang::Rewriter &r, PragmaConfig& cfg) override;
  void visitDeclStmt(clang::DeclStmt *stmt, clang::Rewriter &r);
  void visitCompoundStmt(clang::CompoundStmt *stmt, clang::Rewriter& r);
  void visitBinaryOperator(clang::BinaryOperator *op, clang::Rewriter& r);
  void visitCXXMethodDecl(clang::CXXMethodDecl* decl, clang::Rewriter& r);
  void visitFunctionDecl(clang::FunctionDecl* decl, clang::Rewriter& r);
  void visitForStmt(clang::ForStmt* stmt, clang::Rewriter& r);
};

struct SSTPragmaList {
  void push_back(SSTPragma* p){
    pragmas.push_back(p);
  }

  template <class T>
  SSTPragma*
  takeMatch(T* t){
    auto end = pragmas.end();
    for (auto iter=pragmas.begin(); iter != end; ++iter){
      SSTPragma* p = *iter;
      bool match = p->matches<T>(t);
      if (match){
        pragmas.erase(iter);
        return p;
      }
    }
    return nullptr;
  }

  std::list<SSTPragma*> pragmas;
};

class SSTPragmaHandler : public clang::PragmaHandler {

 protected:
  SSTPragmaHandler(const char* name, SSTPragmaList& plist,
                   clang::CompilerInstance& ci,
                   SkeletonASTVisitor& visitor,
                   std::set<clang::Stmt*>& deleted) :
    PragmaHandler(name), pragmas_(plist), ci_(ci),
    deleted_(deleted), visitor_(visitor)
  {}
  SSTPragmaList& pragmas_;
  clang::CompilerInstance& ci_;
  SkeletonASTVisitor& visitor_;
  std::set<clang::Stmt*>& deleted_;

  /**
   * @brief configure Assuming the PP lex position is currently on eod,
   *        configure the source locations of the pragma and initialize fields
   * @param PP
   * @param pragma
   */
  void configure(clang::Token& PragmaTok, clang::Preprocessor& PP, SSTPragma* pragma);

};

/**
 * @brief The SSTSimplePragmaHandler_base class
 * Used for pragmas of the form #pragma sst action
 * Here action is a single keyword that takes no arguments and acts on the next expression
 */
class SSTSimplePragmaHandler_base : public SSTPragmaHandler {
 public:
  void HandlePragma(clang::Preprocessor &PP,
                    clang::PragmaIntroducerKind Introducer,
                    clang::Token &PragmaTok);

 protected:
  SSTSimplePragmaHandler_base(const char* name, SSTPragmaList& plist,
                              clang::CompilerInstance& CI,
                              SkeletonASTVisitor& visitor,
                              std::set<clang::Stmt*>& deld) :
    SSTPragmaHandler(name, plist, CI, visitor, deld)
  {}

 private:
  virtual SSTPragma* allocatePragma() = 0;
};

template <class T>
class SSTSimplePragmaHandler : public SSTSimplePragmaHandler_base
{
 protected:
  /**
   * @brief SSTSimplePragmaHandler Constructor for pragma handlers for pragmas of the form
   *        #pragma sst name
   * @param name  The string identifying the pragma
   * @param plist The pragma list to append to
   */
  SSTSimplePragmaHandler(const char* name, SSTPragmaList& plist,
                         clang::CompilerInstance& CI,
                         SkeletonASTVisitor& visitor,
                         std::set<clang::Stmt*>& deld) :
    SSTSimplePragmaHandler_base(name, plist, CI, visitor, deld)
  {}

 private:
  /**
   * @brief allocatePragma
   * @return A derived type that performs the correct pragma operation for name
   */
  SSTPragma* allocatePragma(){
    return new T;
  }
};

class SSTDeletePragmaHandler : public SSTSimplePragmaHandler<SSTDeletePragma> {
 public:
  SSTDeletePragmaHandler(SSTPragmaList& plist, clang::CompilerInstance& CI,
                         SkeletonASTVisitor& visitor, std::set<clang::Stmt*>& deld) :
    SSTSimplePragmaHandler<SSTDeletePragma>("delete", plist, CI, visitor, deld)
  {}
};

class SSTMallocPragmaHandler : public SSTSimplePragmaHandler<SSTMallocPragma> {
 public:
  SSTMallocPragmaHandler(SSTPragmaList& plist, clang::CompilerInstance& CI,
                         SkeletonASTVisitor& visitor, std::set<clang::Stmt*>& deld) :
   SSTSimplePragmaHandler<SSTMallocPragma>("malloc", plist, CI, visitor, deld)
  {}
};

class SSTNewPragmaHandler : public SSTSimplePragmaHandler<SSTNewPragma> {
 public:
  SSTNewPragmaHandler(SSTPragmaList& plist, clang::CompilerInstance& CI,
                      SkeletonASTVisitor& visitor, std::set<clang::Stmt*>& deld) :
   SSTSimplePragmaHandler<SSTNewPragma>("new", plist, CI, visitor, deld)
  {}
};

class SSTKeepPragmaHandler : public SSTSimplePragmaHandler<SSTKeepPragma> {
 public:
  SSTKeepPragmaHandler(SSTPragmaList& plist, clang::CompilerInstance& CI,
                      SkeletonASTVisitor& visitor, std::set<clang::Stmt*>& deld) :
   SSTSimplePragmaHandler<SSTKeepPragma>("keep", plist, CI, visitor, deld)
  {}
};

class SSTTokenStreamPragmaHandler : public SSTPragmaHandler
{
 public:
  void HandlePragma(clang::Preprocessor &PP,
                   clang::PragmaIntroducerKind Introducer,
                   clang::Token &PragmaTok);

 protected:
  /**
   * @brief SSTSimplePragmaHandler Constructor for pragma handlers for pragmas of the form
   *        #pragma sst name
   * @param name  The string identifying the pragma
   * @param plist The pragma list to append to
   */
  SSTTokenStreamPragmaHandler(const char* name, SSTPragmaList& plist,
                         clang::CompilerInstance& CI,
                         SkeletonASTVisitor& visitor,
                         std::set<clang::Stmt*>& deld) :
    SSTPragmaHandler(name, plist, CI, visitor, deld)
  {}

 private:
  virtual SSTPragma* allocatePragma(clang::SourceLocation loc, //for error printing
                                    const std::list<clang::Token>& tokens) const = 0;
};


class SSTNullVariablePragmaHandler : public SSTTokenStreamPragmaHandler
{
 public:
  SSTNullVariablePragmaHandler(SSTPragmaList& plist,
                        clang::CompilerInstance& CI,
                        SkeletonASTVisitor& visitor,
                        std::set<clang::Stmt*>& deld) :
     SSTTokenStreamPragmaHandler("null_variable", plist, CI, visitor, deld){}

 private:
  SSTPragma* allocatePragma(clang::SourceLocation loc,
                            const std::list<clang::Token> &tokens) const;

};

#endif
