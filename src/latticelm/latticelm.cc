#include <iostream>
#include <fstream>
#include <string>
// #include <algorithm>
// #include <numeric>
#include <boost/program_options.hpp>
// #include <boost/algorithm/string.hpp>
// #include <boost/archive/text_iarchive.hpp>
// #include <boost/archive/text_oarchive.hpp>
// #include <cnn/expr.h>
// #include <cnn/cnn.h>
// #include <cnn/dict.h>
// #include <cnn/training.h>
// #include <cnn/rnn.h>
// #include <cnn/lstm.h>
// #include <cnn/grad-check.h>
// #include <latticelm/latticelm.h>
// #include <latticelm/macros.h>
#include <latticelm/timer.h>
// #include <latticelm/counts.h>
// #include <latticelm/dist-ngram.h>
// #include <latticelm/dist-factory.h>
// #include <latticelm/dict-utils.h>
// #include <latticelm/whitener.h>
// #include <latticelm/heuristic.h>
// #include <latticelm/ff-builder.h>
// #include <latticelm/sequence-indexer.h>
// #include <latticelm/input-file-stream.h>

using namespace std;
using namespace cnn::expr;
namespace po = boost::program_options;

namespace latticelm {

int LatticeLM::main(int argc, char** argv) {
  po::options_description desc("*** latticelm (by Graham Neubig) ***");
  desc.add_options()
      ("help", "Produce help message")
      ("train_file", po::value<string>()->default_value(""), "Training file")
      ("train_ref", po::value<string>()->default_value(""), "Training reference file containing true phoneme strings (optional)")
      ("beam", po::value<int>()->default_value(-1), "Beam size")
      ("epochs", po::value<int>()->default_value(100), "Epochs")
      ("word_n", po::value<int>()->default_value(3), "Length of word n-grams")
      ("char_n", po::value<int>()->default_value(3), "Length of character n-grams")
      ("lattice_weight", po::value<float>()->default_value(1.f), "Amount of weight to give to the lattice probabilities")
      ("verbose", "Verbosity of help messages to print")
      ;
  boost::program_options::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);   
  if (vm.count("help")) {
      cout << desc << endl;
      return 1;
  }

  // Create the timer
  time_ = Timer();
  cout << "Started training! (s=" << time_.Elapsed() << ")" << endl;

  // Temporary buffers
  string line;

  // Save various settings
  GlobalVars::verbose = vm["verbose"].as<int>();
  epochs_ = vm["epochs"].as<int>();
  beam_ = vm["beam"].as<int>();
  char_n_ = vm["char_n"].as<int>();
  word_n_ = vm["word_n"].as<int>();
  lattice_weight_ = vm["lattice_weight"].as<int>();

  // Load data
  vector<DataLatticePtr> lattices = DataLattice::ReadFromFile(vm["train_file"].as<string>(), cids_);
  vector<int> order(lattices.size()); std::iota(order.begin(), order.end(), 0);

  // Create the hierarchical LM
  HierarchicalLM hlm(cids_.size(), char_n, word_n);

  // Sentences of words
  vector<Sentence> sentences;
  
  for(int epoch = 1; epoch <= epochs_; epoch++) {
    LLStats ep_stats;
    std::shuffle(order.begin(), order.end());
    for(int sid : order) {
      if(epoch != 1)
        hlm.RemoveSample(sentences[sid]);
      sentences[sid] = hlm.CreateSample(*lattices[sid], lattice_weight_, ep_stats);
      hlm.AddSample(sentences[sid]);
    }
    cerr << "Finished epoch " << epoch << ": char=" << ep_stats.words_ << ", ppl=" << ep_stats.CalcPPL() << " (s=" << time_.Elapsed() << endl;
    hlm.ResampleParameters();
  }

  return 0;

}

}
