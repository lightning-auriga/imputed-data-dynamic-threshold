bin_PROGRAMS = imputed-data-dynamic-threshold.out test-suite.out

AM_CXXFLAGS = $(BOOST_CPPFLAGS) -ggdb -Wall -std=c++17

COMBINED_SOURCES = imputed-data-dynamic-threshold/cargs.cc imputed-data-dynamic-threshold/cargs.h imputed-data-dynamic-threshold/executor.cc imputed-data-dynamic-threshold/executor.h imputed-data-dynamic-threshold/r2_bins.cc imputed-data-dynamic-threshold/r2_bins.h imputed-data-dynamic-threshold/utilities.cc imputed-data-dynamic-threshold/utilities.h
COMBINED_LDADD = $(BOOST_LDFLAGS) -lboost_program_options -lboost_system -lboost_filesystem -lz -lhts

imputed_data_dynamic_threshold_out_SOURCES = imputed-data-dynamic-threshold/main.cc $(COMBINED_SOURCES)
imputed_data_dynamic_threshold_out_LDADD = $(COMBINED_LDADD)

UNIT_TEST_SOURCES = unit_tests/global_namespace_test.cc unit_tests/global_namespace_test.h unit_tests/r2_bins_test.cc unit_tests/r2_bins_test.h unit_tests/r2_bin_test.cc unit_tests/r2_bin_test.h

test_suite_out_SOURCES = $(UNIT_TEST_SOURCES) $(COMBINED_SOURCES)
test_suite_out_LDADD = $(COMBINED_LDADD) -lgtest_main -lgtest -lpthread

dist_doc_DATA = README
ACLOCAL_AMFLAGS = -I m4
