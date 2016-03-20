#pragma once

#include <latticelm/sentence.h>
#include <latticelm/macros.h>
#include <fst/fst.h>
#include <fst/mutable-fst.h>
#include <vector>
#include <stdexcept>

namespace latticelm {

// sample a single value appropriately from a vector of weights
unsigned SampleWeights(std::vector<float> & ws, float anneal = 1);

template<class A>
float SampGen(const fst::Fst<A> & ifst, fst::MutableFst<A> & ofst, unsigned nbest = 1, float anneal = 1);

template<class A>
Sentence FstToSent(const fst::Fst<A> & ifst) {
  typedef fst::Fst<A> F;
  auto sid = ifst.Start();
  Sentence ret;
  while(true) {
  typename fst::ArcIterator< F > aiter(ifst, sid);
  if(aiter.Done()) break;
  const A& a = aiter.Value();
  if(a.olabel != 0 && a.olabel != 3) ret.push_back(a.olabel);
  sid = a.nextstate;
  }
  return ret;
}

template<class A>
Alignment FstToAlign(const fst::Fst<A> & ifst) {
  typedef fst::Fst<A> F;
  auto state_id = ifst.Start();
  Alignment ret;
  while(true) {
  typename fst::ArcIterator<F> aiter(ifst, state_id);
  if(aiter.Done()) break;
  const A& arc = aiter.Value();
  if(arc.olabel != 0 || arc.ilabel != 0) {
    ret.push_back(std::pair<WordId,WordId>(arc.ilabel, arc.olabel));
  }
  state_id = arc.nextstate;
  }
  return ret;
}

}
