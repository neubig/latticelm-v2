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
  PylmFst(const Pylm & pylm) : pylm_(&pylm) { }

  virtual StateId Start() const override { return 0; };
  virtual Weight Final(StateId sid) const override { THROW_ERROR("PylmFst::Final unimplemented"); };
  virtual size_t NumArcs(StateId sid) const override { THROW_ERROR("PylmFst::NumArcs unimplemented"); };
  virtual size_t NumInputEpsilons(StateId sid) const override { THROW_ERROR("PylmFst::NumInputEpsilons unimplemented"); };
  virtual size_t NumOutputEpsilons(StateId sid) const override { THROW_ERROR("PylmFst::NumOutputEpsilons unimplemented"); };
  virtual uint64 Properties(uint64 mask, bool test) const override { THROW_ERROR("PylmFst::Properties unimplemented"); };
  virtual const string& Type() const override { THROW_ERROR("PylmFst::Type unimplemented"); };
  virtual fst::Fst<A> *Copy(bool safe = false) const override { THROW_ERROR("PylmFst::Copy unimplemented"); };
  virtual const fst::SymbolTable* InputSymbols() const override { THROW_ERROR("PylmFst::InputSymbols unimplemented"); };
  virtual const fst::SymbolTable* OutputSymbols() const override { THROW_ERROR("PylmFst::OutputSymbols unimplemented"); };
  virtual void InitStateIterator(fst::StateIteratorData<A> *) const override { THROW_ERROR("PylmFst::InitStateIterator unimplemented"); };
  virtual void InitArcIterator(StateId sid, fst::ArcIteratorData<A> *) const override { THROW_ERROR("PylmFst::InitArcIterator unimplemented"); };

protected:
  const Pylm* pylm_;

};

}
