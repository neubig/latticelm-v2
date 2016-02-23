#include <iostream>
#include <fstream>
#include <string>
#include <numeric>
#include <boost/program_options.hpp>
#include <latticelm/timer.h>
#include <latticelm/latticelm.h>
#include <latticelm/data-lattice.h>
#include <latticelm/hierarchical-lm.h>
#include <latticelm/lexical-tm.h>
#include <latticelm/ll-stats.h>
#include <latticelm/macros.h>

using namespace std;
namespace po = boost::program_options;

namespace latticelm {

void LatticeLM::PerformTrainingLexTM(const vector<DataLatticePtr> & lattices, LexicalTM & tm) {
  // Perform training
  vector<int> order(lattices.size()); std::iota(order.begin(), order.end(), 0);
  vector<Alignment> alignments(lattices.size());
  tm.PrintParams();
  for(int epoch = 1; epoch <= epochs_; epoch++) {
    std::shuffle(order.begin(), order.end(), *GlobalVars::rndeng);
    LLStats ep_stats;
    for(int align_id : order) {
      if(epoch != 1)
        tm.RemoveSample(alignments[align_id]);
      cout << "align_id: " << align_id << endl;
      alignments[align_id] = tm.CreateSample(*lattices[align_id], ep_stats);
      tm.AddSample(alignments[align_id]);
      tm.PrintCounts();
    }
    cerr << "Finished epoch " << epoch << ": char=" << ep_stats.words_ << ", ppl=" << ep_stats.CalcPPL() << " (s=" << time_.Elapsed() << ")" << endl;
    tm.ResampleParameters();
    tm.PrintParams();
  }
  tm.Normalize(epochs_);
}

template <class LM>
void LatticeLM::PerformTraining(const vector<DataLatticePtr> & lattices, LM & lm) {

  // Perform training
  vector<int> order(lattices.size()); std::iota(order.begin(), order.end(), 0);
  vector<Sentence> sentences(lattices.size());
  for(int epoch = 1; epoch <= epochs_; epoch++) {
    std::shuffle(order.begin(), order.end(), *GlobalVars::rndeng);
    LLStats ep_stats;
    for(int sid : order) {
      if(epoch != 1)
        lm.RemoveSample(sentences[sid]);
      sentences[sid] = lm.CreateSample(*lattices[sid], ep_stats);
      lm.AddSample(sentences[sid]);
    }
    cerr << "Finished epoch " << epoch << ": char=" << ep_stats.words_ << ", ppl=" << ep_stats.CalcPPL() << " (s=" << time_.Elapsed() << ")" << endl;
    lm.ResampleParameters();
  }
}

int LatticeLM::main(int argc, char** argv) {
  po::options_description desc("*** latticelm (by Graham Neubig) ***");
  desc.add_options()
      ("help", "Produce help message")
      ("train_file", po::value<string>()->default_value(""), "Training file")
      ("train_ref", po::value<string>()->default_value(""), "Training reference file containing true phoneme strings (optional)")
      ("trans_file", po::value<string>()->default_value(""), "File containing word-tokenized translations of the training lattices in plain text.")
      ("file_format", po::value<string>()->default_value("text"), "The format of the lattices in the input file")
      ("model_type", po::value<string>()->default_value("pylm"), "Model type (hierlm to do segmentation and LM learning, pylm to just do lm learning)")
      ("beam", po::value<int>()->default_value(0), "Beam size")
      ("epochs", po::value<int>()->default_value(100), "Epochs")
      ("word_n", po::value<int>()->default_value(2), "Length of word n-grams")
      ("char_n", po::value<int>()->default_value(2), "Length of character n-grams")
      ("model_in", po::value<string>()->default_value(""), "The file to read the model to")
      ("model_out", po::value<string>()->default_value(""), "The file to write the final model to")
      ("seed", po::value<int>()->default_value(0), "The random seed, or 0 to change every time")
      ("lattice_weight", po::value<float>()->default_value(1.f), "Amount of weight to give to the lattice probabilities")
      ("verbose", po::value<int>()->default_value(1), "Verbosity of messages to print")
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
  epochs_ = vm["epochs"].as<int>();
  beam_ = vm["beam"].as<int>();
  char_n_ = vm["char_n"].as<int>();
  word_n_ = vm["word_n"].as<int>();
  lattice_weight_ = vm["lattice_weight"].as<float>();
  file_format_ = vm["file_format"].as<string>();
  model_type_ = vm["model_type"].as<string>();

  GlobalVars::Init(vm["verbose"].as<int>(), vm["seed"].as<int>());

  // Initialize the vocabulary
  cids_.GetId("<eps>");
  cids_.GetId("<s>");
  cids_.GetId("</s>");

  // Initialize the translation vocabulary
  trans_ids_.GetId("<eps>");
  //trans_ids_.GetId("<s>");
  //trans_ids_.GetId("</s>");

  // Load data
  vector<DataLatticePtr> lattices = DataLattice::ReadFromFile(file_format_, lattice_weight_, vm["train_file"].as<string>(), vm["trans_file"].as<string>(), cids_, trans_ids_);

  // Create the hierarchical LM
  if(model_type_ == "pylm") {
    Pylm pylm(cids_.size(), char_n_);
    PerformTraining(lattices, pylm);
  } else if(model_type_ == "hierlm") {
    HierarchicalLM hlm(cids_.size(), char_n_, word_n_);
    PerformTraining(lattices, hlm);
  } else if(model_type_ == "lextm") {
    LexicalTM tm(cids_, trans_ids_);
    PerformTrainingLexTM(lattices, tm);
  }

  return 0;

}

}
