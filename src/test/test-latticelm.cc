#define BOOST_TEST_MODULE "modlm Tests"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

// Set up CNN
struct LatticeLMBasic {
    LatticeLMBasic()   { 
    }
    ~LatticeLMBasic()  { /* shutdown your allocator/check memory leaks here */ }
};

BOOST_GLOBAL_FIXTURE( LatticeLMBasic );
