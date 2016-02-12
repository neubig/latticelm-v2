#pragma once

#include <latticelm/ll-stats.h>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>

namespace latticelm {

// A data structure to hold words, probabilities, and states
class PylmWordProbState {
public:
  PylmWordProbState(WordId w, float p, int s) : word(w), prob(p), state(s) { };
  WordId word;
  float prob;
  int state;
};

// Define the parent PylmState data structure containing this PylmStateLink as "c".
// This data structure contains all the info needed to calculate P(wid|c), then transition
// to the next context sid.
class PylmStateLink {
public:
  PylmStateLink(int s, WordId w, int cust) : sid(s), wid(w), customers(cust,1), total_customers(cust) { }
  int sid;
  WordId wid;
  vector<int> customers;
  int total_customers;
};

class PylmState {
public:
  PylmState(WordId w, int l, int p) : wid(w), level(l), parent(p), children(), total_customers(0), total_tables(0) { }

  PylmWordProbState CalcWPS(const PylmStateLink & link, const pair<float,float> & param) {
    return PylmWordProbState(link.wid, (link.total_customers-link.customers.size()*param.second)/(total_customers+param.first), link.sid);
  }

  WordId wid;
  int level;
  int parent;
  vector<PylmStateLink> children;
  int total_customers, total_tables;
};

class Pylm {

public:
  Pylm(int base_size, int order) : states_(1, PylmState(-1, 0, -1)), base_size_(base_size), order_(order), init_state_id_(0), params_(order, pair<float,float>(1.0, 0.1)) {
    GetChildStateId(0, 1, true); // Get "<s>"
    GetChildStateId(0, 2, true); // and "</s>"
  }
  ~Pylm() { }

  int GetInitialStateId() {
    if(init_state_id_ == 0 && states_[0].children.size() > 0 && states_[0].children[0].wid == 1)
      init_state_id_ = states_[0].children[0].sid;
    return init_state_id_;
  }

  int GetChildStateId(int sid, WordId wid, bool add) {
    if(sid >= states_.size()) THROW_ERROR("State overflow");
    vector<PylmStateLink> & chil = states_[sid].children;
    int low = 0, range = chil.size(), mid;
    while(range) {
      mid = low+range/2;
      if(chil[mid].wid > wid) {
        range = mid-low;
      } else if(chil[mid].wid < wid) {
        range -= mid-low+1;
        low = mid+1;
      } else {
        return chil[mid].sid;
      }
    }
    if(add) {
      chil.insert(chil.begin()+low, PylmStateLink(states_.size(), wid, 0));
      states_.push_back(PylmState(wid, states_[sid].level+1, sid));
      return states_.size()-1;
    }
    return -1; 
  }

  PylmState & GetState(int sid) {
    if(sid >= states_.size()) THROW_ERROR("State overflow");
    return states_[sid];
  }

  // Calculate the output for words, probabilities, and states for a particular
  // state in the LM tree. Word 0 is the fallback.
  std::vector<PylmWordProbState> CalcWordProbStates(int sid) {
    PylmState & state = states_[sid];
    std::vector<PylmWordProbState> wps;
    if(state.wid == 2) return wps; // If we're at the final state, we don't do anything
    pair<float,float> & param = params_[state.level];
    // If we're at the top state, calculate everything, but no backoff
    if(state.level == 0 && base_size_ > 0) {
      float base_prob = 1.f/base_size_;
      auto it = state.children.begin();
      for(int i = 1; i <= base_size_; i++) {
        if(it != state.children.end() && it->wid == i) {
          wps.push_back(state.CalcWPS(*it++, param));
          wps.rbegin()->prob += base_prob;
        } else {
          wps.push_back(PylmWordProbState(i, base_prob, 0));
        }
      }
    // Otherwise, calculate a backoff and only some things
    } else {
      float fallback_prob = (state.total_tables*param.second + param.first)/(state.total_customers+param.first);
      wps.push_back(PylmWordProbState(0,fallback_prob,0));
      for(auto & itval : state.children)
        wps.push_back(state.CalcWPS(itval, param));
    }
    return wps;
  }
  
  std::vector<PylmState> & GetStates() { return states_; }

  // Remove or add a sample to the statistics
  void RemoveSample(const Sentence & sent);
  void AddSample(const Sentence & sent);

  // Create a sample from the lattice
  Sentence CreateSample(const DataLattice & lat, LLStats & stats);  

  void ResampleParameters();

protected:

  std::vector<PylmState> states_;

  int base_size_, order_;
  
  int init_state_id_;

  vector<pair<float,float> > params_;

};

}
