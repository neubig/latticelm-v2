#pragma once

namespace latticelm {

class PYLM {

public:
  PYLM(int order) : order_(order) { }
  ~PYLM() { }

  void ResampleParameters();

protected:
  int order_;

};

}
