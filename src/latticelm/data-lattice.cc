#include <latticelm/data-lattice.h>
#include <latticelm/macros.h>
#include <latticelm/sentence.h>

using namespace latticelm;
using namespace std;
using namespace fst;

vector<DataLatticePtr> DataLattice::ReadFromFile(const std::string & format, const std::string & filename, SymbolSet<string> & dict) {
  if(format == "text") {
    return ReadFromTextFile(filename, dict);
  } else if (format == "openfst") {
    return ReadFromOpenFSTFile(filename, dict);
  } else {
    THROW_ERROR("Illegal file format: " << format);
  }
}

vector<DataLatticePtr> DataLattice::ReadFromTextFile(const std::string & filename, SymbolSet<string> & dict) {
  string line;
  ifstream in(filename);
  if(!in) THROW_ERROR("Could not open " << filename);
  vector<DataLatticePtr> ret;
  while(getline(in, line)) {
    Sentence sent = ParseSentence(line, dict);
    DataLatticePtr ptr(new DataLattice);
    StdVectorFst::StateId last_id = ptr->fst_.AddState(), next_id;
    for(auto wid : sent) {
      next_id = ptr->fst_.AddState();
      ptr->fst_.AddArc(last_id, StdArc(wid, wid, 0.f, next_id));
      last_id = next_id;
    }
    ret.push_back(ptr);
  }
  return ret;
}

vector<DataLatticePtr> DataLattice::ReadFromOpenFSTFile(const std::string & filename, SymbolSet<string> & dict) {
  THROW_ERROR("DataLattice::ReadFromOpenFSTFile not implemented");
}
