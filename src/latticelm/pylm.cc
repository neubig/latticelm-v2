#include <latticelm/pylm.h>
#include <latticelm/pylm-fst.h>
#include <latticelm/macros.h>
#include <latticelm/sampgen.h>
#include <fst/compose.h>

using namespace latticelm;
using namespace fst;

PylmStateLink * Pylm::GetChildStateLink(int sid, WordId wid, bool add) {
  assert(wid >= 0);
  assert(sid < (int)states_.size() && sid >= 0);
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
      return &chil[mid];
    }
  }
  if(add && order_ > 1) {
    int backoff_sid = (states_[sid].backoff_sid == -1 ? 0 : GetChildStateId(states_[sid].backoff_sid, wid, add));
    if(states_[sid].level == order_-1) {
      chil.insert(chil.begin()+low, PylmStateLink(backoff_sid, wid, 0));
    } else {
      chil.insert(chil.begin()+low, PylmStateLink(states_.size(), wid, 0));
      states_.push_back(PylmState(wid, states_[sid].level+1, backoff_sid));
    }
    return &states_[sid].children[low];
  }
  return NULL; 
}

std::vector<PylmWordProbState> Pylm::CalcWordProbStates(int sid) {
  PylmState & state = states_[sid];
  std::vector<PylmWordProbState> wps;
  if(state.wid == 2) return wps; // If we're at a final state, we don't do anything
  assert(state.level < params_.size());
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
    wps.push_back(PylmWordProbState(0,state.CalcBackoff(param),state.backoff_sid));
    for(auto & itval : state.children)
      wps.push_back(state.CalcWPS(itval, param));
  }
  return wps;
}

void Pylm::ResampleParameters() {
  cerr << "WARNING: Pylm::ResampleParameters not implemented yet." << endl;
  // THROW_ERROR("Pylm::ResampleParameters");
}

bool Pylm::RemoveNgram(const Sentence & ngram) {
  int j, state = 0;
  for(j = 0; j < ngram.size()-1; j++) {
    state = GetChildStateId(state, ngram[j], false);
    assert(state != -1);
  }
  PylmStateLink* link = GetChildStateLink(state, ngram[j], false);
  assert(link != NULL && link->total_customers != 0);
  std::uniform_int_distribution<int> ud(1, link->total_customers);
  int left = ud(*GlobalVars::rndeng);
  link->total_customers--;
  states_[state].total_customers--;
  for(int t = 0; t < link->customers.size(); t++) {
    left -= link->customers[t];
    if(left <= 0) {
      if(link->customers[t] == 1) {
        link->customers.erase(link->customers.begin()+t);
        if(ngram.size() == 1)
          return true;
        Sentence new_ngram(ngram);
        new_ngram.erase(new_ngram.begin());
        return RemoveNgram(new_ngram);
      } else {
        link->customers[t]--;
        return false;
      }
    }
  }
  THROW_ERROR("Overflowed RemoveNgram");
}

void Pylm::RemoveSample(const Sentence & sent, vector<bool> & fellback) {
  Sentence ngram(1,1); // Starting with just the wordId corresponding to <s>
  for(size_t i = 0; i < sent.size(); i++) {
    if(ngram.size() >= order_) ngram.erase(ngram.begin());
    ngram.push_back(sent[i]);
    if(fellback.size()) {
      fellback[i] = RemoveNgram(ngram);
    } else {
      RemoveNgram(ngram);
    }
  }
}

bool Pylm::AddNgram(const Sentence & ngram, float base) {
  int i, j, t, lev, state; // i=first word, j=curr word, t=table, lev=context length
  vector<float> full_probs(1,base), single_probs;
  vector<PylmStateLink*> links;
  vector<int> states;
  // Calculate the probabilities from unigram onward
  for(lev = 0; lev < ngram.size(); lev++) {
    i = ngram.size()-lev-1;
    state = 0;
    for(j = i; j < ngram.size()-1; j++)
      state = GetChildStateId(state, ngram[j], true);
    PylmStateLink* link = GetChildStateLink(state, ngram[j], true);
    assert(lev >= 0 && lev < params_.size());
    float backoff = states_[state].CalcBackoff(params_[lev]);
    float prob = states_[state].CalcProb(*link,params_[lev]);
    single_probs.push_back(prob);
    full_probs.push_back((*full_probs.rbegin())*backoff + prob);
    states.push_back(state);
    links.push_back(link);
  }
  // Next, going backwards, at each level decide whether to add a new table
  for(lev = ngram.size()-1; lev >= 0; lev--) {
    std::bernoulli_distribution bd(single_probs[lev]/full_probs[lev+1]);
    PylmStateLink & link = *links[lev];
    states_[states[lev]].total_customers++;
    // Use existing table
    if(bd(*GlobalVars::rndeng)) {
      std::uniform_real_distribution<float> ud(0, link.total_customers - link.customers.size() * params_[lev].second);
      float left = ud(*GlobalVars::rndeng);
      for(t = 0; t < (int)link.customers.size(); t++) {
        left -= link.customers[t]-params_[lev].second;
        if(left <= 0) break;
      }
      assert(t < link.customers.size());
      link.customers[t]++; link.total_customers++;
      break;
    // Use new table
    } else {
      link.customers.push_back(1); link.total_customers++; 
    }
  }
  return lev < 0;
}

void Pylm::AddSample(const Sentence & sent, const vector<float> & bases, vector<bool> & fellback) {
  assert(base_size_ != 0 || (bases.size() == fellback.size() && bases.size() == sent.size()));
  Sentence ngram(1,1);
  for(size_t i = 0; i < sent.size(); i++) {
    if(ngram.size() >= order_) ngram.erase(ngram.begin());
    ngram.push_back(sent[i]);
    if(bases.size()) {
      fellback[i] = AddNgram(ngram, bases[i]);
    } else {
      AddNgram(ngram, 1.f/base_size_);
    }
  }
}

Sentence Pylm::CreateSample(const DataLattice & lattice, LLStats & stats) {
  PylmFst<LogArc> pylm_fst(*this);
  VectorFst<LogArc> vec_lattice_fst(lattice.GetFst()); vec_lattice_fst.Write("lattice.txt");
  VectorFst<LogArc> vec_pylm_fst(pylm_fst); vec_pylm_fst.Write("pylm.txt"); // cerr << "WARNING: not using dynamic PyLM, should be fixed" << endl;
  ComposeFst<LogArc> composed_fst(lattice.GetFst(), vec_pylm_fst);
  VectorFst<LogArc> vec_composed_fst(composed_fst); vec_composed_fst.Write("composed.txt"); // cerr << "WARNING: not using dynamic PyLM, should be fixed" << endl;
  VectorFst<LogArc> sample_fst;
  stats.lik_ += SampGen(composed_fst, sample_fst);
  sample_fst.Write("sample.txt");
  Sentence sent = FstToSent(sample_fst);
  stats.words_ += sent.size();
  return sent;
}
