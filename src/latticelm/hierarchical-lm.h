#pragma once

#include <latticelm/pylm.h>
#include <latticelm/ll-stats.h>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>

namespace latticelm {

class HierarchicalLM {

public:
  HierarchicalLM(int char_types, int char_n, int word_n) : char_types_(char_types), char_lm_(char_n), word_lm_(word_n) { }

  // Remove or add a sample to the statistics
  void RemoveSample(const Sentence & sent);
  void AddSample(const Sentence & sent);

  // Create a sample from the lattice
  Sentence CreateSample(const DataLattice & lat, float lattice_weight, LLStats & stats);

  // Resample the parameters
  void ResampleParameters() {
    char_lm_.ResampleParameters();
    word_lm_.ResampleParameters();
  }

protected:
  int char_types_;
  PYLM char_lm_, word_lm_;
  SymbolSet word_ids_;
  std::vector<WordId> word_ids_stale_;

};

}
