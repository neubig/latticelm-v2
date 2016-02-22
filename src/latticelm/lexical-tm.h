#pragma once

#include <latticelm/ll-stats.h>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>
#include <fst/vector-fst.h>
#include <fst/float-weight.h>
#include <cmath>
#include <iostream>

namespace latticelm {

class LexicalTM {

public:

  LexicalTM(int f_vocab_size, int e_vocab_size) {
    f_vocab_size_ = f_vocab_size;
    e_vocab_size_ = e_vocab_size;

    // Zero the count vectors. Assign uniform log probabilities to the CPD

    // Doesn't matter if we're including or excluding foreign epsilons as we're conditioning on the foreign side.
    for(int i=0; i < f_vocab_size_; i++) {
      vector<fst::LogWeight> cpd_row;
      vector<int> counts_row;
      for(int j=0; j < e_vocab_size_; j++) {
        cpd_row.push_back(fst::LogWeight(log(1.0/e_vocab_size)));
        counts_row.push_back(0);
      }
      cpd_.push_back(cpd_row);
      counts_.push_back(counts_row);
    }

  }

  void RemoveSample(const Alignment & align);
  void AddSample(const Alignment & align);
  Alignment CreateSample(const DataLattice & lat, LLStats & stats);
  void ResampleParameters();
  fst::VectorFst<fst::LogArc> CreateReducedTM(const DataLattice & lattice);

  // Test methods to be moved elsewhere later
  void TestLogWeightSampling();

  // Helpful methods
  void PrintParams();
  void PrintCounts();

protected:

  // Assuming our vocab is less than 32,767.
  int f_vocab_size_;
  int e_vocab_size_;

  // A conditional probability disribution that will give the probability of
  // seeing an English WordId given a German WordId.
  vector<vector<fst::LogWeight>> cpd_;
  // The number of times we've seen a German WordId align to an English WordId.
  vector<vector<int>> counts_;

};

}
