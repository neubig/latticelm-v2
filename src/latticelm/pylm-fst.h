#pragma once

#include <latticelm/macros.h>
#include <fst/fst.h>
#include <fst/vector-fst.h>

namespace latticelm {

class Pylm;

template <class A>
class PylmFst : public fst::Fst< A > {
  typedef typename A::StateId StateId;
  typedef typename A::Weight Weight;

public:
  PylmFst(Pylm & pylm) : pylm_(&pylm), 
                               properties_(/*fst::kExpanded | */fst::kAcceptor | fst::kIDeterministic | fst::kODeterministic | fst::kEpsilons | fst::kIEpsilons | fst::kOEpsilons | fst::kILabelSorted | fst::kOLabelSorted | fst::kWeighted | fst::kCyclic),
                               type_("pylm"),
                               arcs_(pylm.GetStates().size(),NULL) { }

  virtual StateId Start() const override { /* cerr << "PylmFst::Start() == " << pylm_->GetInitialStateId() << endl; */ return pylm_->GetInitialStateId(); };
  virtual Weight Final(StateId sid) const override { /* cerr << "PylmFst::Final("<<sid<<") == " << (pylm_->GetState(sid).wid == 2 ? 1 : 0) << endl; */ return pylm_->GetState(sid).wid == 2 ? Weight::One() : Weight::Zero(); };
  virtual size_t NumInputEpsilons(StateId sid) const override { THROW_ERROR("PylmFst::NumInputEpsilons unimplemented"); };
  virtual size_t NumOutputEpsilons(StateId sid) const override { THROW_ERROR("PylmFst::NumOutputEpsilons unimplemented"); };
  virtual uint64 Properties(uint64 mask, bool test) const override { return properties_; };
  virtual const string& Type() const override { /* cerr << "PylmFst::Type() == " << type_ << endl; */ return type_; };
  virtual fst::Fst<A> *Copy(bool safe = false) const override { return new PylmFst(*pylm_); };
  virtual StateId NumStates() const { /* cerr << "PylmFst::NumStates() == " << arcs_.size() << endl; */ return arcs_.size(); }  
  virtual size_t NumArcs(StateId sid) const override {
    /* cerr << "PylmFst::NumArcs("<<sid<<") == " << GetArcs(sid)->size() << endl; */
    return GetArcs(sid)->size();
  };
  virtual const fst::SymbolTable* InputSymbols() const override { /* cerr << "PylmFst::InputSymbols()" << endl; */ return NULL; }  // not necessary? 
  virtual const fst::SymbolTable* OutputSymbols() const override { /* cerr << "PylmFst::OutputSymbols()" << endl; */ return NULL; } // not necessary?
  virtual bool Write(const string &filename) const override { return fst::Fst<A>::WriteFile(filename); }
  

  virtual void InitStateIterator(fst::StateIteratorData<A>* data) const override {
      /* cerr << "PylmFst::InitStateIterator() == " << arcs_.size() << endl; */
      data->base = 0;
      data->nstates = arcs_.size();
  }

  virtual void InitArcIterator(StateId stateId, fst::ArcIteratorData<A>* data) const override {
      data->base = 0;
      const vector<A> * myArcs = GetArcs(stateId);
      data->narcs = myArcs->size();
      data->arcs = data->narcs > 0 ? &((*myArcs)[0]) : 0;
      data->ref_count = 0;
      /* cerr << "PylmFst::InitArcIterator("<<stateId<<") == " << myArcs->size() << endl; */
  }

  const vector<A> * GetArcs(StateId stateId) const {
    if(stateId < 0 || stateId >= (StateId)arcs_.size())
      THROW_ERROR("PylmFst::GetArcs: StateId is out of bounds");
    if(arcs_[stateId] == NULL) {
      vector<A> * logs = new vector<A>;
      vector<PylmWordProbState> probs = pylm_->CalcWordProbStates(stateId);
      for(auto & prob : probs) {
        /* cerr << stateId << "->" << prob.state << ":" << prob.word << "/" << prob.word << "(" << log(prob.prob) << ")" << endl; */
        logs->push_back(A(prob.word, prob.word, -log(prob.prob), prob.state));
      }
      arcs_[stateId] = logs;
    }
    return arcs_[stateId];
  }

protected:
  Pylm* pylm_;
  uint64 properties_;
  std::string type_;
  mutable vector< vector< A >* > arcs_;

};

}
