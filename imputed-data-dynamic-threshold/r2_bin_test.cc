/*!
  \file r2_bin_test.cc
  \brief implementations for global function tests
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include "imputed-data-dynamic-threshold/r2_bin_test.h"

void imputed_data_dynamic_threshold::r2_bin_test::setUp() {
  // nothing to do at the moment
}

void imputed_data_dynamic_threshold::r2_bin_test::tearDown() {
  // nothing to do at the moment
}

void imputed_data_dynamic_threshold::r2_bin_test::test_default_constructor() {
  r2_bin a;
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin default constructor, _bin_min",
                                       0.0, a.get_bin_min(), DBL_EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin default constructor, _bin_max",
                                       0.0, a.get_bin_max(), DBL_EPSILON);
  CPPUNIT_ASSERT_MESSAGE(
      "r2_bin default constructor, _data",
      float_vector_equals(std::vector<float>(), a.get_data()));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin default constructor, _total",
                                       0.0, a.get_total(), DBL_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bin default constructor, _total_count", 0u,
                               a.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bin default constructor, _filtered_count",
                               0u, a.get_filtered_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_copy_constructor() {
  r2_bin a;
  a.add_value(0.5);
  a.add_value(0.6);
  a.compute_threshold(0.58);
  r2_bin b(a);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin copy constructor, _bin_min",
                                       a.get_bin_min(), b.get_bin_min(),
                                       DBL_EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin copy constructor, _bin_max",
                                       a.get_bin_max(), b.get_bin_max(),
                                       DBL_EPSILON);
  CPPUNIT_ASSERT_MESSAGE("r2_bin copy constructor, _data",
                         float_vector_equals(a.get_data(), b.get_data()));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin copy constructor, _total",
                                       a.get_total(), b.get_total(),
                                       DBL_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bin copy constructor, _total_count",
                               a.get_total_count(), b.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bin copy constructor, _filtered_count",
                               a.get_filtered_count(), b.get_filtered_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_set_bin_bounds() {
  r2_bin a;
  a.set_bin_bounds(0.1, 0.2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin set_bin_bounds, minimum", 0.1,
                                       a.get_bin_min(), DBL_EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin set_bin_bounds, maximum", 0.2,
                                       a.get_bin_max(), DBL_EPSILON);
}

void imputed_data_dynamic_threshold::r2_bin_test::test_add_value() {
  r2_bin a;
  a.add_value(0.4f);
  std::vector<float> vec;
  vec.push_back(0.4f);
  CPPUNIT_ASSERT_MESSAGE("add_value: add single value, vector",
                         float_vector_equals(a.get_data(), vec));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("add_value: add single value, total",
                                       0.4f, a.get_total(), FLT_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("add_value: add single value, total count", 1u,
                               a.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("add_value: add single value, filtered count",
                               1u, a.get_filtered_count());
  a.add_value(0.35f);
  vec.push_back(0.35f);
  CPPUNIT_ASSERT_MESSAGE("add_value: add second value, vector",
                         float_vector_equals(a.get_data(), vec));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("add_value: add second value, total",
                                       0.75f, a.get_total(), FLT_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("add_value: add second value, total count", 2u,
                               a.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("add_value: add second value, filtered count",
                               2u, a.get_filtered_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_compute_threshold() {
  r2_bin a;
  a.add_value(0.4f);
  a.add_value(0.35f);
  a.compute_threshold(0.385f);
  std::vector<float> vec;
  vec.push_back(0.35f);
  vec.push_back(0.4f);
  CPPUNIT_ASSERT_MESSAGE("compute_threshold: two distinct values, data storage",
                         float_vector_equals(a.get_data(), vec));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
      "compute_threshold: two distinct values, total", 0.4f, a.get_total(),
      FLT_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "compute_threshold: two distinct values, total count", 2u,
      a.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "compute_threshold: two distinct values, filtered count", 1u,
      a.get_filtered_count());
  r2_bin b;
  b.add_value(0.5f);
  b.add_value(0.35f);
  b.add_value(0.35f);
  b.compute_threshold(0.41f);
  vec.clear();
  vec.push_back(0.35f);
  vec.push_back(0.35f);
  vec.push_back(0.5f);
  CPPUNIT_ASSERT_MESSAGE("compute_threshold: ties on boundary, data storage",
                         float_vector_equals(b.get_data(), vec));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
      "compute_threshold: ties on boundary, total", 0.5f, b.get_total(),
      FLT_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "compute_threshold: ties on boundary, total count", 3u,
      b.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "compute_threshold: ties on boundary, filtered count", 1u,
      b.get_filtered_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_report_threshold() {
  r2_bin a;
  a.set_bin_bounds(0.1, 0.2);
  a.add_value(0.4f);
  a.add_value(0.5f);
  a.add_value(0.6f);
  a.add_value(0.7f);
  a.compute_threshold(0.58);
  std::ostringstream o;
  a.report_threshold(o);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("report_threshold: case 1",
                               std::string("0.1\t0.2\t4\t0.5\t3\t0.75\n"),
                               o.str());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_equality_operator() {
  r2_bin a;
  a.add_value(0.5f);
  a.add_value(0.6f);
  a.compute_threshold(0.58f);
  r2_bin b(a);
  CPPUNIT_ASSERT_MESSAGE("r2_bin equality operator", a == b);
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_bin_min() {
  r2_bin a;
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("get_bin_min default test", a._bin_min,
                                       a.get_bin_min(), DBL_EPSILON);
  a.set_bin_bounds(0.1, 0.2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("get_bin_min set test", a._bin_min,
                                       a.get_bin_min(), DBL_EPSILON);
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_bin_max() {
  r2_bin a;
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("get_bin_max default test", a._bin_max,
                                       a.get_bin_max(), DBL_EPSILON);
  a.set_bin_bounds(0.1, 0.2);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("get_bin_max set test", a._bin_max,
                                       a.get_bin_max(), DBL_EPSILON);
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_data() {
  r2_bin a;
  CPPUNIT_ASSERT_MESSAGE("get_data default test",
                         float_vector_equals(a._data, a.get_data()));
  a.add_value(0.1f);
  a.add_value(0.5f);
  a.add_value(0.3f);
  CPPUNIT_ASSERT_MESSAGE("get_data set test",
                         float_vector_equals(a._data, a.get_data()));
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_total() {
  r2_bin a;
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("get_total default test", a._total,
                                       a.get_total(), DBL_EPSILON);
  a.add_value(0.1f);
  a.add_value(0.9f);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("get_total set test", a._total,
                                       a.get_total(), DBL_EPSILON);
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_total_count() {
  r2_bin a;
  CPPUNIT_ASSERT_EQUAL_MESSAGE("get_total_count default test", a._total_count,
                               a.get_total_count());
  a.add_value(0.1f);
  a.add_value(0.45f);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("get_total_count set test", a._total_count,
                               a.get_total_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_filtered_count() {
  r2_bin a;
  CPPUNIT_ASSERT_EQUAL_MESSAGE("get_filtered_count default test",
                               a._filtered_count, a.get_filtered_count());
  a.add_value(0.1f);
  a.add_value(0.25f);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("get_filtered_count set test", a._filtered_count,
                               a.get_filtered_count());
}

CPPUNIT_TEST_SUITE_REGISTRATION(imputed_data_dynamic_threshold::r2_bin_test);
