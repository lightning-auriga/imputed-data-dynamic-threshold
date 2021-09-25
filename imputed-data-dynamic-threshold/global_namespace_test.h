/*!
  \file global_namespace_test.h
  \brief tests for global functions
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#ifndef IMPUTED_DATA_DYNAMIC_THRESHOLD_GLOBAL_NAMESPACE_TEST_H_
#define IMPUTED_DATA_DYNAMIC_THRESHOLD_GLOBAL_NAMESPACE_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cfloat>
#include <cmath>
#include <string>
#include "imputed-data-dynamic-threshold/utilities.h"

namespace imputed_data_dynamic_threshold {
class global_namespace_test : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(global_namespace_test);
  CPPUNIT_TEST(test_from_string);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_from_string();

 private:
};
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_GLOBAL_NAMESPACE_TEST_H_
