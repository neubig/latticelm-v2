AM_CXXFLAGS = $(BOOST_CPPFLAGS) $(CNN_CPPFLAGS) $(EIGEN_CPPFLAGS) $(OPENMP_CXXFLAGS) -I$(srcdir)/..

noinst_PROGRAMS = test-latticelm
TESTS = test-latticelm

test_latticelm_SOURCES = \
    test-latticelm.cc 

test_latticelm_LDADD = \
	../latticelm/liblatticelm.la \
    $(CNN_LDFLAGS) \
    $(BOOST_LDFLAGS) \
    $(BOOST_UNIT_TEST_FRAMEWORK_LIB) \
    $(BOOST_PROGRAM_OPTIONS_LIB) \
    $(BOOST_SERIALIZATION_LIB) \
    $(BOOST_IOSTREAMS_LIB) \
    $(OPENMP_CXXFLAGS)
