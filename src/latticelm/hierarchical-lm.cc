#include <latticelm/hierarchical-lm.h>
#include <latticelm/macros.h>
#include <latticelm/data-lattice.h>

using namespace latticelm;
using namespace std;

void HierarchicalLM::RemoveSample(const Sentence & sent) {
  THROW_ERROR("RemoveSample not implemented yet");
}

void HierarchicalLM::AddSample(const Sentence & sent) {
  THROW_ERROR("AddSample not implemented yet");
}

Sentence HierarchicalLM::CreateSample(const DataLattice & lattice, LLStats & stats) {
  THROW_ERROR("CreateSample not implemented yet");
}
