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
  PylmState(WordId w, int l, int p) : wid(w), level(l), backoff_sid(p), children(), total_customers(0), total_tables(0) { }

  float CalcProb(const PylmStateLink & link, const pair<float,float> & param) {
    return (link.total_customers-link.customers.size()*param.second)/(total_customers+param.first);
  }

  PylmWordProbState CalcWPS(const PylmStateLink & link, const pair<float,float> & param) {
    return PylmWordProbState(link.wid, CalcProb(link,param), link.sid);
  }

  // Calculate the backoff probability for a state
  float CalcBackoff(const std::pair<float,float> & param) {
    return (total_tables*param.second + param.first)/(total_customers+param.first);
  }


  WordId wid;
  int level;
  int backoff_sid;
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


  PylmStateLink * GetChildStateLink(int sid, WordId wid, bool add);

  int GetChildStateId(int sid, WordId wid, bool add) {
    PylmStateLink * link = GetChildStateLink(sid, wid, add);
    return link ? link->sid : -1;
  }

  PylmState & GetState(int sid) {
    if(sid >= states_.size()) THROW_ERROR("State overflow");
    return states_[sid];
  }

  // Calculate the output for words, probabilities, and states for a particular
  // state in the LM tree. Word 0 is the fallback.
  std::vector<PylmWordProbState> CalcWordProbStates(int sid);
  
  std::vector<PylmState> & GetStates() { return states_; }

  // Remove or add a sample to the statistics
  void RemoveSample(const Sentence & sent, vector<bool> & fellback);
  void AddSample(const Sentence & sent, const vector<float> & bases, vector<bool> & fellback);

  void RemoveSample(const Sentence & sent) { 
    vector<bool> fellback;
    RemoveSample(sent, fellback);
  }
  void AddSample(const Sentence & sent) {
    vector<float> bases;
    vector<bool> fellback;
    AddSample(sent, bases, fellback);
  }

  bool AddNgram(const Sentence & ngram, float base);
  bool RemoveNgram(const Sentence & ngram);

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
