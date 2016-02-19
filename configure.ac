#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([latticelm], [0.1.0], [neubig@gmail.com])
AC_CONFIG_SRCDIR(src/latticelm/latticelm.cc)
AC_CONFIG_HEADERS([src/latticelm/config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/latticelm/Makefile
    src/test/Makefile
])

AM_INIT_AUTOMAKE([-Wall subdir-objects])
AM_PROG_AR()

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC

AC_LANG_CPLUSPLUS

# Check for OPENFST
AC_ARG_WITH(openfst,
	[AC_HELP_STRING([--with-openfst=DIR], [src in DIR])],
	[openfst_dir="${withval}"], [openfst_dir=''])
if test "x$openfst_dir" != "x"; then
  OPENFST_CPPFLAGS="-I${openfst_dir}/src/include"
  OPENFST_LDFLAGS="-L${openfst_dir}/src/lib -lfst"
else
  AS_ERROR("Must specify --with-openfst=[OPENFST_DIR]")
fi

# Checks for libraries.
AC_SYS_LARGEFILE
AC_SEARCH_LIBS([dlopen], [dl dld], [], [
  AC_MSG_ERROR([unable to find the dlopen() function])
])

# Checks for header files.
AC_CHECK_HEADERS([sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Check to make sure that we have unordered_map
AC_LANG([C++])
AC_CHECK_HEADERS([tr1/unordered_map])
AC_CHECK_HEADERS([ext/hash_map])
CXXFLAGS="$CXXFLAGS -std=c++11"

# Check if we have rt/boost
AX_BOOST_BASE([1.49], , AC_MSG_ERROR([Boost 1.49 or later is required]))
AX_BOOST_SYSTEM
AX_BOOST_PROGRAM_OPTIONS
AX_BOOST_SERIALIZATION
AX_BOOST_IOSTREAMS
AX_BOOST_UNIT_TEST_FRAMEWORK

AC_OPENMP
AC_SUBST(OPENMP_CXXFLAGS,${OPENMP_CXXFLAGS})

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday pow sqrt])

AC_OUTPUT
