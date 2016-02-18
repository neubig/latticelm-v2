#include <latticelm/data-lattice.h>
#include <latticelm/macros.h>
#include <latticelm/sentence.h>
#include <boost/algorithm/string.hpp>
#include <fst/script/compile-impl.h>

using namespace latticelm;
using namespace std;
using namespace fst;

vector<DataLatticePtr> DataLattice::ReadFromFile(const std::string & format, float weight, const std::string & filename, const std::string & trans_filename, SymbolSet<string> & dict, SymbolSet<string> & trans_dict) {
  vector<DataLatticePtr> data_lattices;
  if(format == "text") {
    data_lattices = ReadFromTextFile(filename, weight, dict);
  } else if (format == "openfst") {
    data_lattices = ReadFromOpenFSTFile(filename, weight, dict);
  } else {
    THROW_ERROR("Illegal file format: " << format);
  }
  if(!trans_filename.empty()) {
    DataLattice::ReadTranslations(data_lattices, trans_filename, trans_dict);
  }
  return data_lattices;
}

vector<DataLatticePtr> DataLattice::ReadFromTextFile(const std::string & filename, float weight, SymbolSet<string> & dict) {
  string line;
  ifstream in(filename);
  if(!in) THROW_ERROR("Could not open " << filename);
  vector<DataLatticePtr> ret;
  while(getline(in, line)) {
    Sentence sent = ParseSentence(line, dict);
    if(*sent.rbegin() != 2) sent.push_back(2); // All sentences must end with a sentence ending
    DataLatticePtr ptr(new DataLattice);
    StdVectorFst::StateId last_id = ptr->fst_.AddState(), next_id;
    ptr->fst_.SetStart(last_id);
    for(auto wid : sent) {
      next_id = ptr->fst_.AddState();
      ptr->fst_.AddArc(last_id, StdArc(wid, wid, 0.f, next_id));
      last_id = next_id;
    }
    ptr->fst_.SetFinal(last_id, StdArc::Weight::One());
    ret.push_back(ptr);
  }
  return ret;
}

vector<DataLatticePtr> DataLattice::ReadFromOpenFSTFile(const std::string & filename, float weight, SymbolSet<string> & dict) {
  /** Be wary of the assumptions this method makes:
    *   - The input file includes some number of FSTs, each of which is separated by a blank line.
    *   - An FST description is comprised of a number of lines. Each line
    *   represents an arc and is delimited by tabs or spaces. The first two values are
    *   the ids from and to. The second to are the transduction input and
    *   output. The final value is the weight.
    *   - The first state in the first listed arc of each FST is the sole start state.
    *   - The final state in the final listed arc of each FST is the sole final state.
    *   - StateIds created by Add state start from 0 and increment.
    *   - I assume I can implicitly cast or convert an integer to a stateid (as evidenced by the call to stoi())
    */
  string line;
  ifstream in(filename);
  if(!in) THROW_ERROR("Could not open " << filename);
  vector<DataLatticePtr> ret;
  // Initialize lattice
  DataLatticePtr ptr(new DataLattice);
  StdVectorFst::StateId last_id = ptr->fst_.AddState();
  ptr->fst_.SetStart(last_id);
  StdVectorFst::StateId num_states = last_id + 1;
  StdVectorFst::StateId to_state;
  while(getline(in, line)) {
    if(line == "") {
      // If there are no more lines after this, let's leave this loop.
      if(!getline(in, line)) {
        break;
      }
      // Otherwise wrap up this lattice and initialize a new one.
      ptr->fst_.SetFinal(to_state, StdArc::Weight::One());
      ret.push_back(ptr);
      ptr = DataLatticePtr(new DataLattice);
      StdVectorFst::StateId last_id = ptr->fst_.AddState();
      ptr->fst_.SetStart(last_id);
      num_states = last_id + 1;
    }
    // Read in tokens
    vector<string> line_tokens;
    boost::split(line_tokens, line, boost::is_any_of("\t "), boost::token_compress_on);
    if(line_tokens.size() != 5) {
        THROW_ERROR("Ill-formed FST input. Each line must consist of 5 tokens tab or space delimited.")
    }
    StdVectorFst::StateId from_state = stoi(line_tokens[0]);
    to_state = stoi(line_tokens[1]);
    WordId in = dict.GetId(line_tokens[2]);
    WordId out = dict.GetId(line_tokens[3]);
    TropicalWeight weight = TropicalWeight(stof(line_tokens[4]));
    // Add any necessary states before we add the arc.
    while(num_states < from_state+1 || num_states < to_state+1) {
      ptr->fst_.AddState();
      num_states += 1;
    }
    ptr->fst_.AddArc(from_state, StdArc(in, out, weight, to_state));
  }
  // Wrap up the last uncompleted lattice.
  ptr->fst_.SetFinal(to_state, StdArc::Weight::One());
  ret.push_back(ptr);
  return ret;
}

void DataLattice::ReadTranslations(vector<DataLatticePtr> data_lattices, const string & trans_filename, SymbolSet<string> & trans_dict) {
  // Assuming each line in the translation file corresponds to one lattice, we iterate
  // through the already loaded lattices and give them their corresponding translation.
  string line;
  ifstream in(trans_filename);
  if(!in) THROW_ERROR("Could not open " << trans_filename);
  int i = 0;
  while(getline(in, line)) {
    // Tokenize the string using whitespace.
    Sentence sent = ParseSentence(line, trans_dict);
    data_lattices[i++]->SetTranslation(sent);
  }
  if(i != data_lattices.size()) THROW_ERROR("Number of lattices and number of translations are not equal.");
}
