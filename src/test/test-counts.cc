#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <latticelm/macros.h>
#include <sstream>

using namespace std;
using namespace lamtram;

// ****** The tests *******
BOOST_AUTO_TEST_SUITE(vocabulary)

BOOST_AUTO_TEST_CASE(TestCounts) {
  //TODO

  std::vector<int> exp, act;  
  BOOST_CHECK_EQUAL_COLLECTIONS(exp.begin(), exp.end(), act.begin(), act.end());
}


BOOST_AUTO_TEST_SUITE_END()
