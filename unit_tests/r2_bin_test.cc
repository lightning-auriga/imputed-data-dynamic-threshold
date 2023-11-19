/*!
  \file r2_bin_test.cc
  \brief implementations for r2_bin class
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include "unit_tests/r2_bin_test.h"

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
      string_float_vector_equals(std::vector<std::pair<std::string, float> >(),
                                 a.get_data()));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin default constructor, _total",
                                       0.0, a.get_total(), DBL_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bin default constructor, _total_count", 0u,
                               a.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bin default constructor, _filtered_count",
                               0u, a.get_filtered_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_copy_constructor() {
  r2_bin a;
  a.add_value("a", 0.5);
  a.add_value("b", 0.6);
  a.compute_threshold(0.58);
  r2_bin b(a);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin copy constructor, _bin_min",
                                       a.get_bin_min(), b.get_bin_min(),
                                       DBL_EPSILON);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("r2_bin copy constructor, _bin_max",
                                       a.get_bin_max(), b.get_bin_max(),
                                       DBL_EPSILON);
  CPPUNIT_ASSERT_MESSAGE(
      "r2_bin copy constructor, _data",
      string_float_vector_equals(a.get_data(), b.get_data()));
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
  a.add_value("a", 0.4f);
  std::vector<std::pair<std::string, float> > vec;
  vec.push_back(std::pair<std::string, float>("a", 0.4f));
  CPPUNIT_ASSERT_MESSAGE("add_value: add single value, vector",
                         string_float_vector_equals(a.get_data(), vec));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("add_value: add single value, total",
                                       0.4f, a.get_total(), FLT_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("add_value: add single value, total count", 1u,
                               a.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("add_value: add single value, filtered count",
                               1u, a.get_filtered_count());
  a.add_value("b", 0.35f);
  vec.push_back(std::pair<std::string, float>("b", 0.35f));
  CPPUNIT_ASSERT_MESSAGE("add_value: add second value, vector",
                         string_float_vector_equals(a.get_data(), vec));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("add_value: add second value, total",
                                       0.75f, a.get_total(), FLT_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("add_value: add second value, total count", 2u,
                               a.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("add_value: add second value, filtered count",
                               2u, a.get_filtered_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_compute_threshold() {
  r2_bin a;
  a.add_value("a", 0.4f);
  a.add_value("b", 0.35f);
  a.compute_threshold(0.385f);
  std::vector<std::pair<std::string, float> > vec;
  vec.push_back(std::pair<std::string, float>("b", 0.35f));
  vec.push_back(std::pair<std::string, float>("a", 0.4f));
  CPPUNIT_ASSERT_MESSAGE("compute_threshold: two distinct values, data storage",
                         string_float_vector_equals(a.get_data(), vec));
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
  b.add_value("a", 0.5f);
  b.add_value("b", 0.35f);
  b.add_value("c", 0.35f);
  b.compute_threshold(0.41f);
  vec.clear();
  vec.push_back(std::pair<std::string, float>("b", 0.35f));
  vec.push_back(std::pair<std::string, float>("c", 0.35f));
  vec.push_back(std::pair<std::string, float>("a", 0.5f));
  CPPUNIT_ASSERT_MESSAGE("compute_threshold: ties on boundary, data storage",
                         string_float_vector_equals(b.get_data(), vec));
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
      "compute_threshold: ties on boundary, total", 0.5f, b.get_total(),
      FLT_EPSILON);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "compute_threshold: ties on boundary, total count", 3u,
      b.get_total_count());
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "compute_threshold: ties on boundary, filtered count", 1u,
      b.get_filtered_count());
  r2_bin c;
  c.add_value("a", 0.5f);
  c.compute_threshold(0.6f);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "compute_threshold: impossible request yields full removal", 0u,
      c.get_filtered_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_report_threshold() {
  r2_bin a;
  a.set_bin_bounds(0.1, 0.2);
  a.add_value("a", 0.4f);
  a.add_value("b", 0.5f);
  a.add_value("c", 0.6f);
  a.add_value("d", 0.7f);
  a.compute_threshold(0.58);
  std::ostringstream o;
  a.report_threshold(o);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("report_threshold: output stream",
                               std::string("0.1\t0.2\t4\t0.5\t0.6\t3\t0.75\n"),
                               o.str());
  CPPUNIT_ASSERT_EQUAL_MESSAGE("report_threshold: stored threshold", 0.5f,
                               a._threshold);
}

void imputed_data_dynamic_threshold::r2_bin_test::
    test_report_stored_threshold() {
  r2_bin a;
  CPPUNIT_ASSERT_THROW_MESSAGE(
      "report_stored_threshold: complains if called out of order",
      a.report_stored_threshold(), std::logic_error);
  a.add_value("a", 0.5f);
  a.add_value("b", 0.6f);
  a.compute_threshold(0.57f);
  std::ostringstream o;
  a.report_threshold(o);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("report_stored_threshold: report value",
                                       0.6f, a.report_stored_threshold(),
                                       FLT_EPSILON);
}

void imputed_data_dynamic_threshold::r2_bin_test::
    test_report_passing_variants() {
  r2_bin a;
  std::ostringstream o1, o2, o3;
  a.report_passing_variants(o1);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("report_passing_variants: empty",
                               std::string(""), o1.str());
  a.add_value("a", 0.5f);
  a.add_value("b", 0.6f);
  a.compute_threshold(0.57f);
  a.report_threshold(o2);
  a.report_passing_variants(o3);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("report_passing_variants: variant list",
                               std::string("b\n"), o3.str());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_equality_operator() {
  r2_bin a;
  a.add_value("a", 0.5f);
  a.add_value("b", 0.6f);
  a.compute_threshold(0.58f);
  std::ostringstream o;
  a.report_threshold(o);
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
                         string_float_vector_equals(a._data, a.get_data()));
  a.add_value("a", 0.1f);
  a.add_value("b", 0.5f);
  a.add_value("c", 0.3f);
  CPPUNIT_ASSERT_MESSAGE("get_data set test",
                         string_float_vector_equals(a._data, a.get_data()));
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_total() {
  r2_bin a;
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("get_total default test", a._total,
                                       a.get_total(), DBL_EPSILON);
  a.add_value("a", 0.1f);
  a.add_value("b", 0.9f);
  CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("get_total set test", a._total,
                                       a.get_total(), DBL_EPSILON);
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_total_count() {
  r2_bin a;
  CPPUNIT_ASSERT_EQUAL_MESSAGE("get_total_count default test", a._total_count,
                               a.get_total_count());
  a.add_value("a", 0.1f);
  a.add_value("b", 0.45f);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("get_total_count set test", a._total_count,
                               a.get_total_count());
}

void imputed_data_dynamic_threshold::r2_bin_test::test_get_filtered_count() {
  r2_bin a;
  CPPUNIT_ASSERT_EQUAL_MESSAGE("get_filtered_count default test",
                               a._filtered_count, a.get_filtered_count());
  a.add_value("a", 0.1f);
  a.add_value("b", 0.25f);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("get_filtered_count set test", a._filtered_count,
                               a.get_filtered_count());
}

/// @cond registration
CPPUNIT_TEST_SUITE_REGISTRATION(imputed_data_dynamic_threshold::r2_bin_test);
/// @endcond
