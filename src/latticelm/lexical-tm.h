#pragma once

#include <latticelm/ll-stats>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>

namespace latticelm {

class LexTM {

public:

  void RemoveSample(const Sentence & sent);
  void AddSample(const Sentence & sent);
  Sentence CreateSample(const DataLattice & lat, LLStats & stats);
  void ResampleParameters();

protected:

  // Will have parameters here once I decide how they manifest themselves.

};

}
