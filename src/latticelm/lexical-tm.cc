#include <latticelm/lexical-tm.h>
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
    cout << align[i].first << endl;
    cout << align[i].second << endl;
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
  StdVectorFst::StateId only_state = reduced_tm.AddState();
  reduced_tm.SetStart(only_state);
  reduced_tm.SetFinal(only_state, LogArc::Weight::One());

  PrintParams();

  Sentence translation = lattice.GetTranslation();

  // %TODO: This should be optimized at some point
  for(int i = 0; i < f_vocab_size_; i++) {
    // Normalizing the probabilities. Two steps:
    // 1. Find the total probability mass of the English words that occur in
    //    the translation given the foreign word
    LogWeight total = LogWeight(0);
    for(int j = 0; j < e_vocab_size_; j++) {
      // If the English word is in the translation, then consider these.
      for(int k = 0; k < translation.size(); k++) {
        if(j == translation[k]) {
          total = fst::Plus(total, cpd_[i][j]);
          // We break here because we don't want that English word counted twice. Or do we?
          break;
        }
      }
    }
    // 2. Divide the conditinal probability of each of the English words by the
    //    aforementioned total when adding a corresponding arc to the WFST.
    for(int j = 0; j < e_vocab_size_; j++) {
      // If the English word is in the translation, then consider these.
      for(int k = 0; k < translation.size(); k++) {
        if(j == translation[k]) {
          reduced_tm.AddArc(only_state, LogArc(i, j, fst::Divide(cpd_[i][j], total), only_state));
          break;
        }
      }
    }
  }
  static int count;
  reduced_tm.Write("reduced_tm_" + to_string(count) + ".fst");
  return reduced_tm;
}

Alignment LexicalTM::CreateSample(const DataLattice & lattice, LLStats & stats) {

  // Perform reduction on TM to make it conform to the lattice.translation_
  VectorFst<LogArc> reduced_tm = CreateReducedTM(lattice);

  // Compose the lattice with the reduced tm.
  //StdComposeFst composed_fst(lattice.GetFst(), reduced_tm);

  // Sample from the composed Fst.
  //StdVectorFst sample_fst;
  ///*stats.lik_ +=*/ SampGen(composed_fst, sample_fst);

  //Alignment align = FstToAlign(sample_fst);
  //return align;

  /*
  Alignment align;
  std::pair<WordId,WordId> arrow(1,2);
  align.push_back(arrow);
  return align;
  */

}
