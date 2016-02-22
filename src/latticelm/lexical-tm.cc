#include <latticelm/lexical-tm.h>
#include <latticelm/sampgen.h>
#include <fst/compose.h>
#include <iostream>

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

void LexicalTM::PrintParams() {
  cout << endl << "CPD parameters: " << endl;
  for(int i = 0; i < f_vocab_size_; i++) {
    for(int j = 0; j < e_vocab_size_; j++) {
      cout << cpd_[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl << "Alignment counts: " << endl;
  for(int i = 0; i < f_vocab_size_; i++) {
    for(int j = 0; j < e_vocab_size_; j++) {
      cout << counts_[i][j] << " ";
    }
    cout << endl;
  }
  cout << endl;
}

/** Create a TM based on the parameters that is constrained by the lattice's translation **/
VectorFst<LogArc> LexicalTM::CreateReducedTM(const DataLattice & lattice) {
  VectorFst<LogArc> reduced_tm;
  VectorFst<LogArc>::StateId only_state = reduced_tm.AddState();
  reduced_tm.SetStart(only_state);
  reduced_tm.SetFinal(only_state, LogArc::Weight::One());

  PrintParams();

  Sentence translation = lattice.GetTranslation();

  // %TODO: This should be optimized at some point

  // Starting at 1 because 0 represents an epsilon transition and we don't accept epsilon transitions on the German side in the translation model. That would result in loops in the composition.
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
          break;
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
          break;
        }
      }
    }
  }
  return reduced_tm;
}

Alignment LexicalTM::CreateSample(const DataLattice & lattice, LLStats & stats) {

  TestLogWeightSampling();
  exit(0);

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

void LexicalTM::TestLogWeightSampling() {

  // Define some probabilities
  LogWeight eighty = LogWeight(0.22314);
  LogWeight twenty = LogWeight(1.60944);
  LogWeight sixty = LogWeight(0.51083);
  LogWeight ninetysix = LogWeight(0.04082);

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
  ifst.AddArc(0, LogArc(1,1, ninetysix, 1));
  ifst.AddArc(0, LogArc(2,2, fst::Times(twenty,twenty), 1));

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

/*
  LogWeight x = LogWeight::Zero();
  cout << "x_init: " << x << endl;
  LogWeight a = LogWeight(1.2);
  LogWeight b = LogWeight(0.5);
  x = fst::Plus(x, a);
  x = fst::Plus(x, b);
  cout << "x: " << x << endl;

  vector<float> weights;
  weights.push_back(5.0);

  LogWeight neglogprob_a(0.1054);
  LogWeight neglogprob_b(2.3026);

  LogWeight logprob_half(-0.6931);
  cout << fst::Plus(logprob_a, logprob_b) << endl;

  VectorFst<LogArc> ifst;
  ifst.AddState();
  ifst.AddState();
  ifst.AddState();
  ifst.SetStart(0);
  ifst.SetFinal(2, LogArc::Weight::One());
  //ifst.AddArc(0, LogArc(0,0, fst::Times(fst::Times(logprob_half, logprob_half), fst::Times(logprob_half, logprob_half)), 1));
  ifst.AddArc(0, LogArc(0,0, logprob_a, 1));
  ifst.AddArc(0, LogArc(1,1, logprob_b, 1));
  ifst.AddArc(1, LogArc(2,2, logprob_a, 2));
  ifst.AddArc(1, LogArc(3,3, logprob_b, 2));
  VectorFst<LogArc> ofst;

  for(int i = 0; i < 40; i++) {
    float output = SampGen(ifst, ofst);
    cout << output << endl;
    ofst.Write("ofst.fst");
  }

  exit(0);
*/
