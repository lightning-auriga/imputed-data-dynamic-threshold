bin_PROGRAMS = imputed-data-dynamic-threshold.out

AM_CXXFLAGS = $(BOOST_CPPFLAGS) -ggdb -Wall -std=c++17

imputed_data_dynamic_threshold_out_SOURCES = imputed-data-dynamic-threshold/cargs.cc imputed-data-dynamic-threshold/cargs.h imputed-data-dynamic-threshold/main.cc imputed-data-dynamic-threshold/r2_bins.cc imputed-data-dynamic-threshold/r2_bins.h imputed-data-dynamic-threshold/utilities.h
imputed_data_dynamic_threshold_out_LDADD = $(BOOST_LDFLAGS) -lboost_program_options -lboost_system -lboost_filesystem -lboost_iostreams -lyaml-cpp -lz
dist_doc_DATA = README
ACLOCAL_AMFLAGS = -I m4
## TAP support
TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/tap-driver.sh
TESTS = tests/placeholder.test
EXTRA_DIST = $(TESTS)
