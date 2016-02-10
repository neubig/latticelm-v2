#include <latticelm/pylm.h>
#include <latticelm/pylm-fst.h>
#include <latticelm/macros.h>
#include <latticelm/sampgen.h>
#include <fst/compose.h>

using namespace latticelm;
using namespace fst;

void Pylm::ResampleParameters() {
  THROW_ERROR("Pylm::ResampleParameters");
}

void Pylm::RemoveSample(const Sentence & sent) {
  THROW_ERROR("Pylm::RemoveSample not implemented yet");
}

void Pylm::AddSample(const Sentence & sent) {
  THROW_ERROR("Pylm::AddSample not implemented yet");
}

Sentence Pylm::CreateSample(const DataLattice & lattice, LLStats & stats) {
  StdVectorFst sample_fst;
  PylmFst<StdArc> pylm_fst(*this);
  SampGen(pylm_fst, sample_fst);
  // StdComposeFst composed_fst(lattice.GetFst(), pylm_fst);
  // SampGen(composed_fst, sample_fst);
  return FstToSent(sample_fst);
}
