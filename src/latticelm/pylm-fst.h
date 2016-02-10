#pragma once

#include <latticelm/macros.h>
#include <fst/fst.h>

namespace latticelm {

class Pylm;

template <class A>
class PylmFst : public fst::Fst< A > {
  typedef typename A::StateId StateId;
  typedef typename A::Weight Weight;

public:
  PylmFst(Pylm & pylm) : pylm_(&pylm), 
                               properties_(fst::kOEpsilons | fst::kILabelSorted | fst::kOLabelSorted),
                               type_("pylm") { }

  virtual StateId Start() const override { return pylm_->GetInitialStateId(); };
  virtual Weight Final(StateId sid) const override { return pylm_->GetState(sid).wid == 1 ? Weight::One() : Weight::Zero(); };
  virtual size_t NumArcs(StateId sid) const override { THROW_ERROR("PylmFst::NumArcs unimplemented"); };
  virtual size_t NumInputEpsilons(StateId sid) const override { THROW_ERROR("PylmFst::NumInputEpsilons unimplemented"); };
  virtual size_t NumOutputEpsilons(StateId sid) const override { THROW_ERROR("PylmFst::NumOutputEpsilons unimplemented"); };
  virtual uint64 Properties(uint64 mask, bool test) const override { return properties_; };
  virtual const string& Type() const override { return type_; };
  virtual fst::Fst<A> *Copy(bool safe = false) const override { return new PylmFst(*pylm_); };
  virtual const fst::SymbolTable* InputSymbols() const override { return NULL; }  // not necessary? 
  virtual const fst::SymbolTable* OutputSymbols() const override { return NULL; } // not necessary?
  virtual void InitStateIterator(fst::StateIteratorData<A> *) const override { THROW_ERROR("PylmFst::InitStateIterator unimplemented"); };
  virtual void InitArcIterator(StateId sid, fst::ArcIteratorData<A> *) const override { THROW_ERROR("PylmFst::InitArcIterator unimplemented"); };

protected:
  Pylm* pylm_;
  uint64 properties_;
  std::string type_;

};

}
