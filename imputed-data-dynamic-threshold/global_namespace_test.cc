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
  CPPUNIT_ASSERT_EQUAL_MESSAGE("from_string: convert unsigneds", 12u,
                               from_string<unsigned>("12"));
  // test: converts double
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("from_string: convert doubles", 1.2345,
                                       from_string<double>("1.2345"),
                                       DBL_EPSILON);
  // test: converts float
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("from_string: convert floats", 9.876f,
                                       from_string<float>("9.876"),
                                       FLT_EPSILON);
  // test: detects types that cannot be converted to floats
  CPPUNIT_ASSERT_THROW(from_string<float>("-"), std::runtime_error);
  CPPUNIT_ASSERT_THROW(from_string<float>("."), std::runtime_error);
  CPPUNIT_ASSERT_THROW(from_string<float>("Rsq"), std::runtime_error);
}

void imputed_data_dynamic_threshold::global_namespace_test::
    test_float_vector_equals() {
  std::vector<float> a, b;
  a.push_back(0.1f);
  CPPUNIT_ASSERT_MESSAGE("float_vector_equals: self",
                         float_vector_equals(a, a));
  CPPUNIT_ASSERT_MESSAGE("float_vector_equals: catches size difference",
                         !float_vector_equals(a, b));
  b.push_back(0.1f);
  CPPUNIT_ASSERT_MESSAGE("float_vector_equals: two identical length 1",
                         float_vector_equals(a, b));
  a.push_back(0.2f);
  b.push_back(0.3f);
  CPPUNIT_ASSERT_MESSAGE("float_vector_equals: mismatch in second spot",
                         !float_vector_equals(a, b));
}

void imputed_data_dynamic_threshold::global_namespace_test::test_files_equal() {
  CPPUNIT_ASSERT_MESSAGE("files_equal: identical files",
                         files_equal("Makefile", "Makefile"));
  CPPUNIT_ASSERT_MESSAGE("files_equal: different files",
                         !files_equal("Makefile", "configure"));
}

CPPUNIT_TEST_SUITE_REGISTRATION(
    imputed_data_dynamic_threshold::global_namespace_test);
