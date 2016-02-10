#pragma once

#include <vector>
#include <memory>
#include <latticelm/symbol-set.h>

namespace latticelm {

class DataLattice {

public:
  DataLattice() { }
  ~DataLattice() { }
 
  std::vector<DataLatticePtr> ReadFromFile(const std::string & filename, SymbolSet & dict);

protected:

};

typedef std::shared_ptr<DataLattice> DataLatticePtr;

}
