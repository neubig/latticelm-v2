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
 
  static std::vector<DataLatticePtr> ReadFromFile(const std::string & format, const std::string & filename, SymbolSet<std::string> & dict);
  static std::vector<DataLatticePtr> ReadFromTextFile(const std::string & filename, SymbolSet<std::string> & dict);
  static std::vector<DataLatticePtr> ReadFromOpenFSTFile(const std::string & filename, SymbolSet<std::string> & dict);

protected:
  fst::StdVectorFst fst_;

};

}
