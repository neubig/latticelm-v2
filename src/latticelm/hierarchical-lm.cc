#include <latticelm/hierarchical-lm.h>
#include <latticelm/macros.h>

using namespace latticelm;
using namespace std;

void LatticeLM::RemoveSample(const Sentence & sent) {
  THROW_ERROR("RemoveSample not implemented yet");
}

void LatticeLM::AddSample(const Sentence & sent) {
  THROW_ERROR("AddSample not implemented yet");
}

Sentence LatticeLM::CreateSample(const Lattice & lattice, float lattice_weight, LLStats & stats) {
  THROW_ERROR("CreateSample not implemented yet");
}
