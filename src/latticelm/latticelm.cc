#include <iostream>
#include <fstream>
#include <string>
#include <numeric>
#include <fst/compat.h>
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

DEFINE_bool(help, false, "Display help");
DEFINE_string(train_file, "", "Training file");
DEFINE_string(train_ref, "", "Training reference file containing true phoneme strings (optional)");
DEFINE_string(trans_file, "", "File containing word-tokenized translations of the training lattices in plain text.");
DEFINE_string(file_format, "text", "The format of the lattices in the input file (text/openfst)");
DEFINE_string(model_type, "pylm", "Model type (hierlm to do segmentation and LM learning, pylm to just do lm learning)");
DEFINE_int32(beam, 0, "Beam size");
DEFINE_int32(epochs, 100, "Epochs");
DEFINE_int32(word_n, 2, "Length of word n-grams");
DEFINE_int32(char_n, 2, "Length of character n-grams");
DEFINE_string(model_in, "", "The file to read the model to");
DEFINE_string(model_out, "", "The file to write the final model to");
DEFINE_int32(seed, 0, "The random seed, or 0 to change every time");
DEFINE_double(lattice_weight, 1.f, "Amount of weight to give to the lattice probabilities");
DEFINE_int32(verbose, 1, "Verbosity of messages to print");
DEFINE_double(concentration, 1.0, "The concentration parameter for the Dirichlet process of the translation model.");

namespace latticelm {

void LatticeLM::PerformTrainingLexTM(const vector<DataLatticePtr> & lattices, LexicalTM & tm) {
  // Perform training
  vector<int> order(lattices.size()); std::iota(order.begin(), order.end(), 0);
  vector<Alignment> alignments(lattices.size());
  tm.PrintParams();
  for(int epoch = 1; epoch <= FLAGS_epochs; epoch++) {
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
  tm.Normalize(FLAGS_epochs);
}

template <class LM>
void LatticeLM::PerformTraining(const vector<DataLatticePtr> & lattices, LM & lm) {

  // Perform training
  vector<int> order(lattices.size()); std::iota(order.begin(), order.end(), 0);
  vector<Sentence> sentences(lattices.size());
  for(int epoch = 1; epoch <= FLAGS_epochs; epoch++) {
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
  string usage = "*** latticelm (by Graham Neubig) ***";
  SET_FLAGS(usage.c_str(), &argc, &argv, true);

  if (FLAGS_help) {
    ShowUsage();
    return 1;
  }

  // Create the timer
  time_ = Timer();
  cout << "Started training! (s=" << time_.Elapsed() << ")" << endl;

  // Temporary buffers
  string line;

  // Save various settings

  GlobalVars::Init(FLAGS_verbose, FLAGS_seed);

  // Initialize the vocabulary
  cids_.GetId("<eps>");
  cids_.GetId("<phi>");
  cids_.GetId("<s>");
  cids_.GetId("</s>");

  // Initialize the translation vocabulary
  trans_ids_.GetId("<eps>");
  //trans_ids_.GetId("<s>");
  //trans_ids_.GetId("</s>");

  // Load data
  vector<DataLatticePtr> lattices = DataLattice::ReadFromFile(FLAGS_file_format, FLAGS_lattice_weight, FLAGS_train_file, FLAGS_trans_file, cids_, trans_ids_);

  // Create the hierarchical LM
  if(FLAGS_model_type == "pylm") {
    Pylm pylm(cids_.size(), FLAGS_char_n);
    PerformTraining(lattices, pylm);
  } else if(FLAGS_model_type == "hierlm") {
    HierarchicalLM hlm(cids_.size(), FLAGS_char_n, FLAGS_word_n);
    PerformTraining(lattices, hlm);
  } else if(FLAGS_model_type == "lextm") {
    LexicalTM tm(cids_, trans_ids_, FLAGS_concentration);
    PerformTrainingLexTM(lattices, tm);
  }

  return 0;

}

}
