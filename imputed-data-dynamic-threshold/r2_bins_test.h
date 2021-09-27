/*!
  \file r2_bins_test.h
  \brief tests for r2_bins class
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#ifndef IMPUTED_DATA_DYNAMIC_THRESHOLD_R2_BINS_TEST_H_
#define IMPUTED_DATA_DYNAMIC_THRESHOLD_R2_BINS_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cfloat>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include "imputed-data-dynamic-threshold/r2_bins.h"

namespace imputed_data_dynamic_threshold {
/*!
  \class r2_bins_test
  \brief text fixture for r2_bins tests
 */
class r2_bins_test : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(r2_bins_test);
  CPPUNIT_TEST(test_default_constructor);
  CPPUNIT_TEST(test_copy_constructor);
  CPPUNIT_TEST(test_set_bin_boundaries);
  CPPUNIT_TEST(test_find_maf_bin);
  CPPUNIT_TEST(test_load_info_file);
  CPPUNIT_TEST(test_compute_thresholds);
  CPPUNIT_TEST(test_report_thresholds);

  CPPUNIT_TEST(test_equality_operator);
  CPPUNIT_TEST(test_inequality_operator);
  CPPUNIT_TEST(test_get_bins);
  CPPUNIT_TEST(test_get_bin_lower_bounds);
  CPPUNIT_TEST(test_get_bin_upper_bounds);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_default_constructor();
  void test_copy_constructor();
  void test_set_bin_boundaries();
  void test_find_maf_bin();
  void test_load_info_file();
  void test_compute_thresholds();
  void test_report_thresholds();
  void test_report_passing_variants_from_ram();
  void test_report_passing_variants_from_file();
  void test_equality_operator();
  void test_inequality_operator();
  void test_get_bins();
  void test_get_bin_lower_bounds();
  void test_get_bin_upper_bounds();

 private:
};
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_R2_BINS_TEST_H_
