#include <sampgen.h>

namespace latticelm {

// sample a single value appropriately from a vector of weights
unsigned SampleWeights(vector<float> & ws, float anneal) {

  if(ws.size() == 0) { 
    THROW_ERROR("No final states found during sampling");
  } else if(ws.size() == 1) {
    return 0;
  }

  float minWeight = fst::numeric_limits<float>::infinity(), weightTotal = 0;
  unsigned i;
  for (i = 0; i < ws.size(); i++) {
    ws[i] *= anneal;
    minWeight = std::min(ws[i], minWeight);
  }
  for (i = 0; i < ws.size(); i++) {
    float & f = ws[i];
    f = exp(minWeight-f);
    weightTotal += f;
  }
  // cout << "Total weight=" << weightTotal;
  weightTotal *= rand()/(double)RAND_MAX;
  // cout << ", random weight=" << weightTotal << " (basis " << minWeight << ")"<<endl;
  for(i = 0; i < ws.size(); i++) {
    weightTotal -= ws[i];
    // cout << " after weight " << i << ", " << weightTotal << endl;
    if(weightTotal <= 0)
      break;
  }
  if(i == ws.size()) {
    cerr << "WARNING: Sampling failed, probability mass left at end of cycle" << endl;
    i--;
  }
  return i;
}

}
