#include <latticelm/lexical-tm.h>
#include <latticelm/sampgen.h>
#include <fst/compose.h>
#include <iostream>
#include <cmath>

using namespace latticelm;
using namespace fst;

void LexicalTM::RemoveSample(const Alignment & align) {
  //Reduce the counts for the alignments.
  for(int i = 0; i < align.size(); i++) {
    counts_[align[i].first][align[i].second]--;
    assert(counts_[align[i].first][align[i].second] >= 0);
  }
}

void LexicalTM::AddSample(const Alignment & align) {
  //Reduce the counts for the alignments.
  for(int i = 0; i < align.size(); i++) {
    counts_[align[i].first][align[i].second]++;
    assert(counts_[align[i].first][align[i].second] > 0);
  }
}

void LexicalTM::PrintCounts() {
  cout << endl << "Alignment counts: " << endl;
  for(int i = 0; i < f_vocab_size_; i++) {
    for(int j = 0; j < e_vocab_size_; j++) {
      cout << counts_[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void LexicalTM::PrintParams() {
  cout << endl << "CPD parameters: " << endl;
  for(int i = 0; i < f_vocab_size_; i++) {
    for(int j = 0; j < e_vocab_size_; j++) {
      cout << exp(-1*cpd_[i][j].Value()) << " ";
    }
    cout << endl;
  }
  cout << endl;
}

void LexicalTM::Normalize(int epochs) {
  for(int i = 0; i < f_vocab_size_; i++) {
    for(int j = 0; j < e_vocab_size_; j++) {
      cpd_[i][j] = fst::Divide(cpd_[i][j],LogWeight(-log(epochs)));
    }
  }
}

/** Create a TM based on the parameters that is constrained by the lattice's translation **/
VectorFst<LogArc> LexicalTM::CreateReducedTM(const DataLattice & lattice) {
  VectorFst<LogArc> reduced_tm;
  VectorFst<LogArc>::StateId only_state = reduced_tm.AddState();
  reduced_tm.SetStart(only_state);
  reduced_tm.SetFinal(only_state, LogArc::Weight::One());

  Sentence translation = lattice.GetTranslation();

  // %TODO: This should be optimized at some point

  // Starting at 1 because 0 represents an epsilon transition and we don't
  // accept epsilon transitions on the German side in the translation model.
  // That would result in loops in the composition.
  for(int i = 1; i < f_vocab_size_; i++) {
    // Normalizing the probabilities. Two steps:
    // 1. Find the total probability mass of the English words that occur in
    //    the translation given the foreign word
    LogWeight total = LogWeight::Zero();
    // First add the probability of an epsilon (ie. null token) on the English side.
    total = fst::Plus(total, cpd_[i][0]);
    // Then check each of the English words to see if they are in the translation.
    for(int j = 1; j < e_vocab_size_; j++) {
      for(int k = 0; k < translation.size(); k++) {
        if(j == translation[k]) {
          total = fst::Plus(total, cpd_[i][j]);
          // We break here because we don't want that English word counted twice. Or do we?
          // Yeah, actually we do. A German word aligns to one English word.
          // The more duplicates of the English word we see in the translation
          // the more likely we want the German word to align to that word?
          // So we don't //break;
        }
      }
    }
    // 2. Divide the conditional probability of each of the English words by the
    //    aforementioned total when adding a corresponding arc to the WFST.

    // First for the necessarily present null token on the English side.
    reduced_tm.AddArc(only_state, LogArc(i, 0, fst::Divide(cpd_[i][0], total), only_state));
    // Then  checking if the rest of the English words are in the translation, before adding the arc.
    for(int j = 1; j < e_vocab_size_; j++) {
      for(int k = 0; k < translation.size(); k++) {
        if(j == translation[k]) {
          reduced_tm.AddArc(only_state, LogArc(i, j, fst::Divide(cpd_[i][j], total), only_state));
          // Not breaking here as the English side may have duplicate tokens. // break;
        }
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

  Alignment align = FstToAlign(sample_fst);
  cout << "align: " << align << endl;

  return align;

}

void LexicalTM::ResampleParameters() {
  // Specify hyperparameters of the Dirichlet Process.
  double alpha = 0.1; // The strength parameter.
  LogWeight log_alpha = LogWeight(-log(alpha));
  // We assume a uniform distribution, base_dist_, which has been initialized to uniform.
  for(int i = 0; i < f_vocab_size_; i++) {
    double row_total = 0;
    for(int j = 0; j < e_vocab_size_; j++) {
      row_total += counts_[i][j];
    }
    for(int j = 0; j < e_vocab_size_; j++) {
      LogWeight numerator = fst::Plus(fst::Times(log_alpha,base_dist_[i][j]), LogWeight(-log(counts_[i][j])));
      LogWeight denominator = fst::Plus(log_alpha,LogWeight(-log(row_total)));
      cpd_[i][j] = fst::Plus(cpd_[i][j], fst::Divide(numerator,denominator));
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
