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
    test_string_float_vector_equals() {
  std::vector<std::pair<std::string, float> > a, b;
  a.push_back(std::pair<std::string, float>("a", 0.1f));
  CPPUNIT_ASSERT_MESSAGE("string_float_vector_equals: self",
                         string_float_vector_equals(a, a));
  CPPUNIT_ASSERT_MESSAGE("string_float_vector_equals: catches size difference",
                         !string_float_vector_equals(a, b));
  b.push_back(std::pair<std::string, float>("a", 0.1f));
  CPPUNIT_ASSERT_MESSAGE("string_float_vector_equals: two identical length 1",
                         string_float_vector_equals(a, b));
  a.push_back(std::pair<std::string, float>("b", 0.2f));
  b.push_back(std::pair<std::string, float>("b", 0.3f));
  CPPUNIT_ASSERT_MESSAGE(
      "string_float_vector_equals: mismatch in second spot, float",
      !string_float_vector_equals(a, b));
  a.at(1).first = "c";
  b.at(1).second = 0.2f;
  CPPUNIT_ASSERT_MESSAGE(
      "string_float_vector_equals: mismatch in second spot, string",
      !string_float_vector_equals(a, b));
}

void imputed_data_dynamic_threshold::global_namespace_test::test_files_equal() {
  CPPUNIT_ASSERT_MESSAGE("files_equal: identical files",
                         files_equal("Makefile", "Makefile"));
  CPPUNIT_ASSERT_MESSAGE("files_equal: different files",
                         !files_equal("Makefile", "configure"));
}

void imputed_data_dynamic_threshold::global_namespace_test::
    test_string_float_less_than() {
  CPPUNIT_ASSERT_MESSAGE("string_float_less_than: less than",
                         string_float_less_than(std::make_pair("a", 0.1f),
                                                std::make_pair("b", 0.2f)));
  CPPUNIT_ASSERT_MESSAGE("string_float_less_than: not greater than",
                         !string_float_less_than(std::make_pair("a", 0.2f),
                                                 std::make_pair("b", 0.1f)));
  CPPUNIT_ASSERT_MESSAGE("string_float_less_than: strict",
                         !string_float_less_than(std::make_pair("a", 0.1f),
                                                 std::make_pair("b", 0.1f)));
  CPPUNIT_ASSERT_MESSAGE(
      "string_float_less_than: nan",
      !string_float_less_than(std::make_pair("a", 1.0f / 0.0f),
                              std::make_pair("b", 1.0f / 0.0f)));
}

/// @cond registration
CPPUNIT_TEST_SUITE_REGISTRATION(
    imputed_data_dynamic_threshold::global_namespace_test);
/// @endcond
