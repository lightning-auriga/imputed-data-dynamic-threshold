#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(imputed-data-dynamic-threshold, 1.2.0, lightning.auriga@gmail.com)
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AM_SILENT_RULES([yes])
AM_PROG_AR
LT_INIT
AC_CONFIG_SRCDIR([imputed-data-dynamic-threshold/main.cc])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([temp_config.h])
AX_PREFIX_CONFIG_H([imputed-data-dynamic-threshold/config.h])

# Checks for programs.
AC_LANG([C++])
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_CHECK_PROGS([INSTALL_INFO], [install-info])
AC_CHECK_PROGS([MAKEINFO], [makeinfo])
AC_CHECK_PROGS([TEXI2PDF], [texi2pdf])
AC_CHECK_PROGS([TEXI2DVI], [texi2dvi])
AC_PROG_CXX_C_O

#AC_ARG_ENABLE([threads], [AS_HELP_STRING([--disable-threads],
#		       [disable hyperthreading support (default=no)])],
#		       [],
#		       [AX_PTHREAD])
#AS_IF([test "x$ac_disable_threads" != xyes],
#	    [AX_PTHREAD]) #([CFLAGS=$CFLAGS $PTHREAD_CFLAGS])
#fi

AX_CXX_COMPILE_STDCXX_17([ext], [mandatory])

# Checks for libraries.
AX_BOOST_BASE([1.63.0])
AX_BOOST_FILESYSTEM
AX_BOOST_PROGRAM_OPTIONS

AC_CHECK_LIB([m],[cos])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([
 Makefile
 imputed-data-dynamic-threshold-1.2.0.pc
])

AC_OUTPUT
