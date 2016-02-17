#pragma once

#include <latticelm/ll-stats.h>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>

namespace latticelm {

class LexicalTM {

public:

  LexicalTM(int f_vocab_size, int e_vocab_size) {
    f_vocab_size_ = f_vocab_size;
    e_vocab_size_ = e_vocab_size;
  }

  void RemoveSample(const Sentence & sent);
  void AddSample(const Sentence & sent);
  Sentence CreateSample(const DataLattice & lat, LLStats & stats);
  void ResampleParameters();

protected:

  int f_vocab_size_;
  int e_vocab_size_;

  // Will have parameters here once I decide how they manifest themselves.

};

}
