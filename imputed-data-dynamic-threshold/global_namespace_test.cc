/*!
  \file global_namespace_test.cc
  \brief implementations for global function tests
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include "imputed-data-dynamic-threshold/global_namespace_test.h"

void imputed_data_dynamic_threshold::global_namespace_test::setUp() {
  // nothing to do at the moment
}

void imputed_data_dynamic_threshold::global_namespace_test::tearDown() {
  // nothing to do at the moment
}

void imputed_data_dynamic_threshold::global_namespace_test::test_from_string() {
  // test: converts unsigned
  CPPUNIT_ASSERT_MESSAGE("from_string: convert unsigneds",
                         from_string<unsigned>("12") == 12);
  // test: converts double
  CPPUNIT_ASSERT_MESSAGE(
      "from_string: convert doubles",
      fabs(from_string<double>("1.2345") - 1.2345) < DBL_EPSILON);
  // test: converts float
  CPPUNIT_ASSERT_MESSAGE(
      "from_string: convert floats",
      fabs(from_string<float>("9.876") - 9.876f) < FLT_EPSILON);
  // test: detects types that cannot be converted to floats
  CPPUNIT_ASSERT_THROW(from_string<float>("-"), std::runtime_error);
  CPPUNIT_ASSERT_THROW(from_string<float>("."), std::runtime_error);
  CPPUNIT_ASSERT_THROW(from_string<float>("Rsq"), std::runtime_error);
}

CPPUNIT_TEST_SUITE_REGISTRATION(
    imputed_data_dynamic_threshold::global_namespace_test);
