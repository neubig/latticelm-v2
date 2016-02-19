#pragma once

#include <vector>
#include <memory>
#include <sstream>
#include <iterator>
#include <fst/vector-fst.h>
#include <latticelm/symbol-set.h>
#include <latticelm/sentence.h>

namespace latticelm {

class DataLattice;
typedef std::shared_ptr<DataLattice> DataLatticePtr;

class DataLattice {

public:
  DataLattice() { }
  ~DataLattice() { }
 
  static std::vector<DataLatticePtr> ReadFromFile(const std::string & format, float weight, const std::string & filename, const std::string & trans_filename, SymbolSet<std::string> & dict, SymbolSet<std::string> & trans_dict);
  static std::vector<DataLatticePtr> ReadFromTextFile(const std::string & filename, float weight, SymbolSet<std::string> & dict);
  static std::vector<DataLatticePtr> ReadFromOpenFSTFile(const std::string & filename, float weight, SymbolSet<std::string> & dict);
  static void ReadTranslations(vector<DataLatticePtr> data_lattices, const string & trans_filename, SymbolSet<std::string> & trans_dict);

  const fst::StdVectorFst & GetFst() const { return fst_; }

  Sentence getTranslation() {
    return translation_;
  }
  void setTranslation(Sentence translation) {
    translation_ = translation;
  }

protected:
  fst::StdVectorFst fst_;
  // A word-tokenized English translation for building translation models.
  Sentence translation_;

};

}
