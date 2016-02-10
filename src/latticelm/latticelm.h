#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <latticelm/timer.h>
#include <latticelm/symbol-set.h>
#include <latticelm/sentence.h>
#include <latticelm/data-lattice.h>

namespace latticelm {


class LatticeLM {

public:
  LatticeLM()  { }

  int main(int argc, char** argv);

  template <class LM>
  void PerformTraining(const vector<DataLatticePtr> & lattices, LM & lm);
  
protected:

  std::string file_format_;
  std::string model_in_file_, model_out_file_;
  std::string model_type_;

  SymbolSet<std::string> cids_;

  int epochs_, beam_;
  int char_n_, word_n_;
  float lattice_weight_;

  Timer time_;

};

}
