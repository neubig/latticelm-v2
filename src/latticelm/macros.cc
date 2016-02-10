#include <latticelm/macros.h>

using namespace latticelm;

int GlobalVars::verbose = 0;
std::mt19937* GlobalVars::rndeng = nullptr;

void GlobalVars::Init(int v, unsigned random_seed) {
  GlobalVars::verbose = v;
  if(!random_seed) {
    std::random_device rd;
    random_seed = rd();
  }
  GlobalVars::rndeng = new std::mt19937(random_seed);
}

