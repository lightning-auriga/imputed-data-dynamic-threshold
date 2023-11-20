/*!
  \file r2_bin_test.cc
  \brief implementations for r2_bin class
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include "gtest/gtest.h"
#include "imputed-data-dynamic-threshold/r2_bins.h"

namespace iddt = imputed_data_dynamic_threshold;

TEST(r2BinTest, defaultConstructor) {
  iddt::r2_bin a;
  EXPECT_DOUBLE_EQ(a.get_bin_min(), 0.0);
  EXPECT_DOUBLE_EQ(a.get_bin_max(), 0.0);
  EXPECT_TRUE(iddt::string_float_vector_equals(
      std::vector<std::pair<std::string, float> >(), a.get_data()));
  EXPECT_DOUBLE_EQ(a.get_total(), 0.0);
  EXPECT_EQ(a.get_total_count(), 0u);
  EXPECT_EQ(a.get_filtered_count(), 0u);
}

TEST(r2BinTest, copyConstructor) {
  iddt::r2_bin a;
  a.add_value("a", 0.5);
  a.add_value("b", 0.6);
  a.compute_threshold(0.58);
  iddt::r2_bin b(a);
  EXPECT_DOUBLE_EQ(a.get_bin_min(), b.get_bin_min());
  EXPECT_DOUBLE_EQ(a.get_bin_max(), b.get_bin_max());
  EXPECT_TRUE(iddt::string_float_vector_equals(a.get_data(), b.get_data()));
  EXPECT_DOUBLE_EQ(a.get_total(), b.get_total());
  EXPECT_EQ(a.get_total_count(), b.get_total_count());
  EXPECT_EQ(a.get_filtered_count(), b.get_filtered_count());
}

TEST(r2BinTest, setBinBounds) {
  iddt::r2_bin a;
  a.set_bin_bounds(0.1, 0.2);
  EXPECT_DOUBLE_EQ(a.get_bin_min(), 0.1);
  EXPECT_DOUBLE_EQ(a.get_bin_max(), 0.2);
}

TEST(r2BinTest, addValue) {
  iddt::r2_bin a;
  a.add_value("a", 0.4f);
  std::vector<std::pair<std::string, float> > vec;
  vec.push_back(std::pair<std::string, float>("a", 0.4f));
  EXPECT_TRUE(iddt::string_float_vector_equals(a.get_data(), vec));
  EXPECT_FLOAT_EQ(a.get_total(), 0.4f);
  EXPECT_EQ(a.get_total_count(), 1u);
  a.add_value("b", 0.35f);
  vec.push_back(std::pair<std::string, float>("b", 0.35f));
  EXPECT_TRUE(iddt::string_float_vector_equals(a.get_data(), vec));
  EXPECT_FLOAT_EQ(a.get_total(), 0.75f);
  EXPECT_EQ(a.get_total_count(), 2u);
  EXPECT_EQ(a.get_filtered_count(), 2u);
}

TEST(r2BinTest, computeThreshold) {
  iddt::r2_bin a;
  a.add_value("a", 0.4f);
  a.add_value("b", 0.35f);
  a.compute_threshold(0.385f);
  std::vector<std::pair<std::string, float> > vec;
  vec.push_back(std::pair<std::string, float>("b", 0.35f));
  vec.push_back(std::pair<std::string, float>("a", 0.4f));
  EXPECT_TRUE(iddt::string_float_vector_equals(a.get_data(), vec));
  EXPECT_FLOAT_EQ(a.get_total(), 0.4f);
  EXPECT_EQ(a.get_total_count(), 2u);
  EXPECT_EQ(a.get_filtered_count(), 1u);
  iddt::r2_bin b;
  b.add_value("a", 0.5f);
  b.add_value("b", 0.35f);
  b.add_value("c", 0.35f);
  b.compute_threshold(0.41f);
  vec.clear();
  vec.push_back(std::pair<std::string, float>("b", 0.35f));
  vec.push_back(std::pair<std::string, float>("c", 0.35f));
  vec.push_back(std::pair<std::string, float>("a", 0.5f));
  EXPECT_TRUE(iddt::string_float_vector_equals(b.get_data(), vec));
  EXPECT_FLOAT_EQ(b.get_total(), 0.5f);
  EXPECT_EQ(b.get_total_count(), 3u);
  EXPECT_EQ(b.get_filtered_count(), 1u);
  iddt::r2_bin c;
  c.add_value("a", 0.5f);
  c.compute_threshold(0.6f);
  EXPECT_EQ(c.get_filtered_count(), 0u);
}

TEST(r2BinTest, reportThreshold) {
  iddt::r2_bin a;
  a.set_bin_bounds(0.1, 0.2);
  a.add_value("a", 0.4f);
  a.add_value("b", 0.5f);
  a.add_value("c", 0.6f);
  a.add_value("d", 0.7f);
  a.compute_threshold(0.58);
  std::ostringstream o;
  a.report_threshold(o);
  EXPECT_EQ(std::string("0.1\t0.2\t4\t0.5\t0.6\t3\t0.75\n"), o.str());
  EXPECT_FLOAT_EQ(a.report_stored_threshold(), 0.5f);
}

TEST(r2BinTest, reportStoredThreshold) {
  iddt::r2_bin a;
  EXPECT_THROW(a.report_stored_threshold(), std::logic_error);
  a.add_value("a", 0.5f);
  a.add_value("b", 0.6f);
  a.compute_threshold(0.57f);
  std::ostringstream o;
  a.report_threshold(o);
  EXPECT_FLOAT_EQ(a.report_stored_threshold(), 0.6f);
}

TEST(r2BinTest, reportPassingVariants) {
  iddt::r2_bin a;
  std::ostringstream o1, o2, o3;
  a.report_passing_variants(o1);
  EXPECT_EQ(o1.str(), std::string(""));
  a.add_value("a", 0.5f);
  a.add_value("b", 0.6f);
  a.compute_threshold(0.57f);
  a.report_threshold(o2);
  a.report_passing_variants(o3);
  EXPECT_EQ(o3.str(), std::string("b\n"));
}

TEST(r2BinTest, equalityOperator) {
  iddt::r2_bin a;
  a.add_value("a", 0.5f);
  a.add_value("b", 0.6f);
  a.compute_threshold(0.58f);
  std::ostringstream o;
  a.report_threshold(o);
  iddt::r2_bin b(a);
  EXPECT_EQ(a, b);
}

TEST(r2BinTest, getBinMin) {
  iddt::r2_bin a;
  // ??
  EXPECT_DOUBLE_EQ(a.get_bin_min(), 0.0);
  a.set_bin_bounds(0.1, 0.2);
  EXPECT_DOUBLE_EQ(a.get_bin_min(), 0.1);
}

TEST(r2BinTest, getBinMax) {
  iddt::r2_bin a;
  // ??
  EXPECT_DOUBLE_EQ(a.get_bin_max(), 0.0);
  a.set_bin_bounds(0.1, 0.2);
  EXPECT_DOUBLE_EQ(a.get_bin_max(), 0.2);
}

TEST(r2BinTest, getData) {
  iddt::r2_bin a;
  EXPECT_TRUE(iddt::string_float_vector_equals(
      a.get_data(), std::vector<std::pair<std::string, float> >()));
  a.add_value("a", 0.1f);
  a.add_value("b", 0.5f);
  a.add_value("c", 0.3f);
  std::vector<std::pair<std::string, float> > vec;
  vec.push_back(std::make_pair("a", 0.1f));
  vec.push_back(std::make_pair("b", 0.5f));
  vec.push_back(std::make_pair("c", 0.3f));
  EXPECT_TRUE(iddt::string_float_vector_equals(a.get_data(), vec));
}

TEST(r2BinTest, getTotal) {
  iddt::r2_bin a;
  EXPECT_FLOAT_EQ(a.get_total(), 0.0);
  a.add_value("a", 0.1f);
  a.add_value("b", 0.9f);
  EXPECT_FLOAT_EQ(a.get_total(), 1.0);
}

TEST(r2BinTest, getTotalCount) {
  iddt::r2_bin a;
  EXPECT_EQ(a.get_total_count(), 0u);
  a.add_value("a", 0.1f);
  a.add_value("b", 0.45f);
  EXPECT_EQ(a.get_total_count(), 2u);
}

TEST(r2BinTest, getFilteredCount) {
  iddt::r2_bin a;
  EXPECT_EQ(a.get_filtered_count(), 0u);
  a.add_value("a", 0.1f);
  a.add_value("b", 0.25f);
  EXPECT_EQ(a.get_filtered_count(), 2u);
}
