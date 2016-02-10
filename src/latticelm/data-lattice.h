#pragma once

#include <vector>
#include <memory>
#include <fst/vector-fst.h>
#include <latticelm/symbol-set.h>

namespace latticelm {

class DataLattice;
typedef std::shared_ptr<DataLattice> DataLatticePtr;

class DataLattice {

public:
  DataLattice() { }
  ~DataLattice() { }
 
  static std::vector<DataLatticePtr> ReadFromFile(const std::string & format, float weight, const std::string & filename, SymbolSet<std::string> & dict);
  static std::vector<DataLatticePtr> ReadFromTextFile(const std::string & filename, float weight, SymbolSet<std::string> & dict);
  static std::vector<DataLatticePtr> ReadFromOpenFSTFile(const std::string & filename, float weight, SymbolSet<std::string> & dict);

  const fst::StdVectorFst & GetFst() const { return fst_; }

protected:
  fst::StdVectorFst fst_;

};

}
