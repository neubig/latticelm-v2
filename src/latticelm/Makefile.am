####### Library info

LIBCPP = \
  latticelm.cc \
  dict.cc \
  timer.cc \
  hierarchical-lm.cc> \
  macros.cc


AM_CXXFLAGS = $(BOOST_CPPFLAGS) $(EIGEN_CPPFLAGS) $(CNN_CPPFLAGS) $(OPENMP_CXXFLAGS) -I$(srcdir)/..

lib_LTLIBRARIES = liblatticelm.la

liblatticelm_la_SOURCES = ${LIBCPP}
liblatticelm_la_LDFLAGS = -version-info 0:0:0

####### Binary info

LDADD=liblatticelm.la \
    $(CNN_LDFLAGS) \
    $(BOOST_LDFLAGS) \
    $(BOOST_SYSTEM_LIB) \
    $(BOOST_PROGRAM_OPTIONS_LIB) \
    $(BOOST_SERIALIZATION_LIB) \
    $(BOOST_IOSTREAMS_LIB) \
    $(OPENMP_CXXFLAGS)

bin_PROGRAMS = latticelm

latticelm_SOURCES = latticelm-main.cc
latticelm_LDADD = $(LDADD)
