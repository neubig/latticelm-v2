#pragma once

#include <latticelm/ll-stats.h>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>

namespace latticelm {

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
  PylmState(WordId w) : wid(w), children() { }

  WordId wid;
  vector<PylmStateLink> children;
};

class Pylm {

public:
  Pylm(int base_size, int order) : states_(1, PylmState(-1)), base_size_(base_size), order_(order), init_state_id_(-1) { }
  ~Pylm() { }

  int GetInitialStateId() {
    if(init_state_id_ == -1 && states_[0].children.size() > 0 && states_[0].children[0].wid == 1)
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
        range -= mid-low;
        low = mid+1;
      } else {
        return chil[mid].sid;
      }
    }
    if(add) {
      chil.insert(chil.begin()+low, PylmStateLink(states_.size(), wid, 0));
      states_.push_back(PylmState(wid));
      return states_.size()-1;
    }
    return -1; 
  }

  PylmState & GetState(int sid) {
    if(sid >= states_.size()) THROW_ERROR("State overflow");
    return states_[sid];
  }

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

};

}
