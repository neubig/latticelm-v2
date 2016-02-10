#pragma once

#include <latticelm/pylm.h>
#include <latticelm/ll-stats.h>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>

namespace latticelm {

class HierarchicalLM {

public:
  HierarchicalLM(int char_types, int char_n, int word_n) : char_lm_(char_types, char_n), word_lm_(-1, word_n) { }

  // Remove or add a sample to the statistics
  void RemoveSample(const Sentence & sent);
  void AddSample(const Sentence & sent);

  // Create a sample from the lattice
  Sentence CreateSample(const DataLattice & lat, LLStats & stats);

  // Resample the parameters
  void ResampleParameters() {
    char_lm_.ResampleParameters();
    word_lm_.ResampleParameters();
  }

protected:
  Pylm char_lm_, word_lm_;
  SymbolSet<Sentence> word_ids_;
  std::vector<WordId> word_ids_stale_;

};

}
