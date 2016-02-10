#pragma once

#include <latticelm/ll-stats.h>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>

namespace latticelm {

class Pylm {

public:
  Pylm(int base_size, int order) : base_size_(base_size), order_(order) { }
  ~Pylm() { }

  // Remove or add a sample to the statistics
  void RemoveSample(const Sentence & sent);
  void AddSample(const Sentence & sent);

  // Create a sample from the lattice
  Sentence CreateSample(const DataLattice & lat, LLStats & stats);  

  void ResampleParameters();

protected:
  int base_size_, order_;

};

}
