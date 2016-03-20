#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <latticelm/macros.h>
#include <latticelm/pylm.h>
#include <sstream>

using namespace std;
using namespace latticelm;

// ****** The fixture *******
struct TestPylm {

  TestPylm() {
    GlobalVars::Init(0, 100);
  }
  ~TestPylm() { }
  
  string dummy_str;
};

// ****** The tests *******
BOOST_FIXTURE_TEST_SUITE(pylm, TestPylm)

BOOST_AUTO_TEST_CASE(TestAddRemoveNgram) {
  Pylm pylm(20, 3);
  Sentence sent = {1,4,2};
  pylm.AddNgram(sent, 0.05);
  // We should have four states: "0, 0->1, 0->2, 0->1->4, 0->4
  BOOST_CHECK_EQUAL(pylm.GetStates().size(), 6);
  pylm.RemoveNgram(sent);
  BOOST_CHECK(pylm.CheckEmpty());
}

BOOST_AUTO_TEST_SUITE_END()
