#include <latticelm/lexical-tm.h>
#include <latticelm/sampgen.h>
#include <fst/compose.h>

using namespace latticelm;
using namespace fst;

Alignment LexTM::CreateSample(const DataLattice & lattice, LLStats & stats) {

  // Perform reduction on TM to make it conform to the lattice.translation_

  // Compose the lattice with the reduced tm.
  StdComposeFst composed_fst(lattice.GetFst(), reduced_tm);

  // Sample from the composed Fst.
  StdVectorFst sample_fst;
  /*stats.lik_ +=*/ SampGen(composed_fst, sample_fst);

  Alignment align = FstToAlign(sample_fst);
  return align;

}
