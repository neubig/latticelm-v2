#include <latticelm/pylm.h>
#include <latticelm/macros.h>

using namespace latticelm;

void PYLM::ResampleParameters() {
  THROW_ERROR("PYLM::ResampleParameters");
}

void PYLM::RemoveSample(const Sentence & sent) {
  THROW_ERROR("PYLM::RemoveSample not implemented yet");
}

void PYLM::AddSample(const Sentence & sent) {
  THROW_ERROR("PYLM::AddSample not implemented yet");
}

Sentence PYLM::CreateSample(const DataLattice & lattice, float lattice_weight, LLStats & stats) {
  THROW_ERROR("PYLM::CreateSample not implemented yet");
}
