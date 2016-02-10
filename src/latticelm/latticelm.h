#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <latticelm/timer.h>
#include <latticelm/symbol-set.h>

namespace latticelm {


class LatticeLM {

public:
  LatticeLM()  { }
  
protected:

  std::string model_in_file_, model_out_file_;

  SymbolSet wids_, cids_;

  int epochs_, beam_;

};

}
