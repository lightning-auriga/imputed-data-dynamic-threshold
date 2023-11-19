/*!
  \file r2_bin_test.h
  \brief tests for r2_bin class
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#ifndef UNIT_TESTS_R2_BIN_TEST_H_
#define UNIT_TESTS_R2_BIN_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cfloat>
#include <cmath>
#include <string>

#include "imputed-data-dynamic-threshold/r2_bins.h"

namespace imputed_data_dynamic_threshold {
/*!
  \class r2_bin_test
  \brief text fixture for r2_bin tests
 */
class r2_bin_test : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(r2_bin_test);
  CPPUNIT_TEST(test_default_constructor);
  CPPUNIT_TEST(test_copy_constructor);
  CPPUNIT_TEST(test_set_bin_bounds);
  CPPUNIT_TEST(test_add_value);
  CPPUNIT_TEST(test_compute_threshold);
  CPPUNIT_TEST(test_report_threshold);
  CPPUNIT_TEST(test_report_stored_threshold);
  CPPUNIT_TEST(test_report_passing_variants);
  CPPUNIT_TEST(test_equality_operator);
  CPPUNIT_TEST(test_get_bin_min);
  CPPUNIT_TEST(test_get_bin_max);
  CPPUNIT_TEST(test_get_data);
  CPPUNIT_TEST(test_get_total);
  CPPUNIT_TEST(test_get_total_count);
  CPPUNIT_TEST(test_get_filtered_count);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_default_constructor();
  void test_copy_constructor();
  void test_set_bin_bounds();
  void test_add_value();
  void test_compute_threshold();
  void test_report_threshold();
  void test_report_stored_threshold();
  void test_report_passing_variants();
  void test_equality_operator();
  void test_get_bin_min();
  void test_get_bin_max();
  void test_get_data();
  void test_get_total();
  void test_get_total_count();
  void test_get_filtered_count();

 private:
};
}  // namespace imputed_data_dynamic_threshold

#endif  // UNIT_TESTS_R2_BIN_TEST_H_
