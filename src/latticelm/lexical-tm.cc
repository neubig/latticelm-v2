#include <latticelm/lexical-tm.h>
#include <latticelm/sampgen.h>
#include <fst/compose.h>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace latticelm;
using namespace fst;

void LexicalTM::RemoveSample(const Alignment & align) {
  //Reduce the counts for the alignments.
  for(int i = 0; i < align.size(); i++) {
    counts_[align[i].second][align[i].first]--;
    assert(counts_[align[i].second][align[i].first] >= 0);
  }
}

void LexicalTM::AddSample(const Alignment & align) {
  //Reduce the counts for the alignments.
  for(int i = 0; i < align.size(); i++) {
    counts_[align[i].second][align[i].first]++;
    assert(counts_[align[i].second][align[i].first] > 0);
  }
}

void LexicalTM::PrintCounts() {
  cout << endl << "Alignment counts: " << endl;
  cout << "\t";
  for(int j = 0; j < f_vocab_size_; j++) {
    cout << f_vocab_.GetSym(j) << "\t";
  }
  cout << endl;
  for(int i = 0; i < e_vocab_size_; i++) {
    cout << e_vocab_.GetSym(i) << "\t";
    for(int j = 0; j < f_vocab_size_; j++) {
      cout << counts_[i][j] << "\t";
    }
    cout << endl;
  }
  cout << endl;
}

void LexicalTM::PrintParams() {
  cout << std::fixed << std::setw( 1 ) << std::setprecision( 3 );
  cout << endl << "CPD parameters: " << endl;
  cout << "\t";
  for(int j = 0; j < f_vocab_size_; j++) {
    cout << f_vocab_.GetSym(j) << "\t";
  }
  cout << endl;
  for(int i = 0; i < e_vocab_size_; i++) {
    cout << e_vocab_.GetSym(i) << "\t";
    for(int j = 0; j < f_vocab_size_; j++) {
      cout << exp(-1*cpd_[i][j].Value()) << "\t";
    }
    cout << endl;
  }
  cout << endl;
}

void LexicalTM::Normalize(int epochs) {
  for(int i = 0; i < e_vocab_size_; i++) {
    for(int j = 0; j < f_vocab_size_; j++) {
      cpd_accumulator_[i][j] = fst::Divide(cpd_accumulator_[i][j],LogWeight(-log(epochs)));
    }
  }
  cout << std::fixed << std::setw( 1 ) << std::setprecision( 3 );
  cout << endl << "Average CPD parameters: " << endl;
  cout << "\t";
  for(int j = 0; j < f_vocab_size_; j++) {
    cout << f_vocab_.GetSym(j) << "\t";
  }
  cout << endl;
  for(int i = 0; i < e_vocab_size_; i++) {
    cout << e_vocab_.GetSym(i) << "\t";
    for(int j = 0; j < f_vocab_size_; j++) {
      cout << exp(-1*cpd_accumulator_[i][j].Value()) << "\t";
    }
    cout << endl;
  }
  cout << endl;
}

int in(WordId word_id, Sentence sentence) {
  int ret = 0;
  for(int i = 0; i < sentence.size(); i++) {
    if (word_id == sentence[i]) {
      ret++;
    }
  }
  return ret;
}

/** Create a TM based on the parameters that is constrained by the lattice's translation **/
VectorFst<LogArc> LexicalTM::CreateReducedTM(const DataLattice & lattice) {
  VectorFst<LogArc> reduced_tm;
  VectorFst<LogArc>::StateId only_state = reduced_tm.AddState();
  reduced_tm.SetStart(only_state);
  reduced_tm.SetFinal(only_state, LogArc::Weight::One());

  Sentence translation = lattice.GetTranslation();

  // %TODO: Perhaps this should be optimized at some point.

  // Starting at 1 because 0 represents an epsilon transition and we don't
  // accept epsilon transitions on the foreign side in the translation model.
  // That would result in loops in the composition.
  for(int f_word_id = 1; f_word_id < f_vocab_size_; f_word_id++) {
    // Normalizing the probabilities. Two steps:
    // 1. Find the total probability mass of the p(f|e) for each of the English words that occur in
    //    the translation given the foreign word.
    LogWeight total = LogWeight::Zero();
    // First add the probability of an epsilon (ie. null token) on the English side.
    total = fst::Plus(total, cpd_[0][f_word_id]);
    // Then check each of the English words to see if they are in the
    // translation, and add probability mass if they are
    for(int e_word_id = 1; e_word_id < e_vocab_size_; e_word_id++) {
      int times_in = in(e_word_id, translation);
      for(int i = 0; i < times_in; i++) {
        total = fst::Plus(total, cpd_[e_word_id][f_word_id]);
      }
    }
    // 2. Divide the conditional probability of each of the English words by the
    //    aforementioned total when adding a corresponding arc to the WFST.
    reduced_tm.AddArc(only_state, LogArc(f_word_id, 0, fst::Divide(cpd_[0][f_word_id], total), only_state));
    for(int e_word_id = 1; e_word_id < e_vocab_size_; e_word_id++) {
      int times_in = in(e_word_id, translation);
      for(int i = 0; i < times_in; i++) {
        reduced_tm.AddArc(only_state, LogArc(f_word_id, e_word_id, fst::Divide(cpd_[e_word_id][f_word_id], total), only_state));
      }
    }
  }

  return reduced_tm;
}

Alignment LexicalTM::CreateSample(const DataLattice & lattice, LLStats & stats) {

  //TestLogWeightSampling();
  //exit(0);

  // Perform reduction on TM to make it conform to the lattice.translation_
  VectorFst<LogArc> reduced_tm = CreateReducedTM(lattice);
  reduced_tm.Write("reduced_tm.fst");

  lattice.GetFst().Write("lattice.fst");

  // Compose the lattice with the reduced tm.
  ComposeFst<LogArc> composed_fst(lattice.GetFst(), reduced_tm);
  VectorFst<LogArc> vecfst(composed_fst);
  vecfst.Write("composed.fst");
  //const SymbolTable* isyms = composed_fst.InputSymbols();
  //const SymbolTable* osyms = composed_fst.OutputSymbols();
  //const string ifn("isyms.txt");
  //composed_fst.InputSymbols()->WriteText(ifn);
  //composed_fst.OutputSymbols()->WriteText("osyms.txt");

  // Sample from the composed Fst.
  VectorFst<LogArc> sample_fst;
  /*stats.lik_ +=*/ SampGen(composed_fst, sample_fst);
  sample_fst.Write("sample.fst");

  /*
  vector<int> counts = {0,0,0,0,0,0,0};
  for(int e = 0; e < 1000; e++) {
    VectorFst<LogArc> sample_fst;
    SampGen(composed_fst, sample_fst);
    Alignment align = FstToAlign(sample_fst);
    counts[align[0].first]++;
    counts[align[1].first]++;
  }
  cout << counts << endl;
  exit(0);
  */

  Alignment align = FstToAlign(sample_fst);
  cout << "align: " << align << endl;

  return align;

}

void LexicalTM::ResampleParameters() {
  // Specify hyperparameters of the Dirichlet Process.
  // We assume a uniform distribution, base_dist_, which has been initialized to uniform.
  for(int i = 0; i < e_vocab_size_; i++) {
    double row_total = 0;
    for(int j = 0; j < f_vocab_size_; j++) {
      row_total += counts_[i][j];
    }
    for(int j = 0; j < f_vocab_size_; j++) {
      LogWeight numerator = fst::Plus(fst::Times(log_alpha_,base_dist_[i][j]), LogWeight(-log(counts_[i][j])));
      LogWeight denominator = fst::Plus(log_alpha_,LogWeight(-log(row_total)));
      cpd_[i][j] = fst::Divide(numerator,denominator);
      cpd_accumulator_[i][j] = fst::Plus(cpd_accumulator_[i][j], cpd_[i][j]);
    }
  }
}

void LexicalTM::TestLogWeightSampling() {

  // Define some probabilities
  LogWeight eighty = LogWeight(0.22314);
  LogWeight twenty = LogWeight(1.60944);
  LogWeight sixty = LogWeight(0.51083);
  LogWeight ninetysix = LogWeight(0.04082);
  LogWeight five = LogWeight(-1.6094);

  // Create a trivial FST.
  VectorFst<LogArc> ifst;
  ifst.AddState();
  ifst.AddState();
  ifst.SetStart(0);
  ifst.SetFinal(1, LogArc::Weight::One());
  //ifst.AddArc(0, LogArc(1,1, eighty, 1));
  //ifst.AddArc(0, LogArc(2,2, twenty, 1));
  //ifst.AddArc(0, LogArc(1,1, fst::Times(eighty,eighty), 1));
  //ifst.AddArc(0, LogArc(2,2, fst::Times(twenty,twenty), 1));
  //ifst.AddArc(0, LogArc(1,1, sixty, 1));
  ifst.AddArc(0, LogArc(1,1, sixty, 1));
  ifst.AddArc(0, LogArc(2,2, twenty,1));
  ifst.AddArc(0, LogArc(2,2, twenty,1));

  cout << fst::Times(twenty,twenty) << endl;
  cout << fst::Plus(twenty,twenty) << endl;

  std::vector<int> count {0,0,0};
  for(int epoch=0; epoch < 10000; epoch++){
    VectorFst<LogArc> sample;
    SampGen(ifst, sample);
    Alignment align = FstToAlign(sample);
    count[align[0].first]++;
  }
  cout << count << endl;;

}
