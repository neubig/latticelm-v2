#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <latticelm/macros.h>
#include <sstream>

using namespace std;

// ****** The fixture *******
struct TestIo {

  TestIo() : dummy_str("test") {
  }
  ~TestIo() { }
  
  string dummy_str;
};

// ****** The tests *******
BOOST_FIXTURE_TEST_SUITE(io, TestIo)

BOOST_AUTO_TEST_CASE(TestIdentity) {
  BOOST_CHECK_EQUAL(dummy_str, "test");
}

BOOST_AUTO_TEST_SUITE_END()
