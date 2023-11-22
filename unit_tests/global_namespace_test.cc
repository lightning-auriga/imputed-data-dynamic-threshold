/*!
  \file global_namespace_test.cc
  \brief implementations for global function tests
  \copyright Released under the MIT License. Copyright
  2023 Lightning Auriga
 */

#include "gtest/gtest.h"
#include "imputed-data-dynamic-threshold/utilities.h"

namespace iddt = imputed_data_dynamic_threshold;

TEST(utilitiesTest, fromString) {
  EXPECT_EQ(iddt::from_string<unsigned>("12"), 12u);
  EXPECT_DOUBLE_EQ(iddt::from_string<double>("1.2345"), 1.2345);
  EXPECT_FLOAT_EQ(iddt::from_string<float>("9.876"), 9.876f);
  // test: detects types that cannot be converted to floats
  EXPECT_THROW(iddt::from_string<float>("-"), std::runtime_error);
  EXPECT_THROW(iddt::from_string<float>("."), std::runtime_error);
  EXPECT_THROW(iddt::from_string<float>("Rsq"), std::runtime_error);
}

TEST(utilitiesTest, stringFloatVectorEquals) {
  std::vector<std::pair<std::string, float> > a, b;
  a.push_back(std::pair<std::string, float>("a", 0.1f));
  EXPECT_TRUE(iddt::string_float_vector_equals(a, a));
  EXPECT_FALSE(iddt::string_float_vector_equals(a, b));
  b.push_back(std::pair<std::string, float>("a", 0.1f));
  EXPECT_TRUE(iddt::string_float_vector_equals(a, b));
  a.push_back(std::pair<std::string, float>("b", 0.2f));
  b.push_back(std::pair<std::string, float>("b", 0.3f));
  EXPECT_FALSE(iddt::string_float_vector_equals(a, b));
  a.at(1).first = "c";
  b.at(1).second = 0.2f;
  EXPECT_FALSE(iddt::string_float_vector_equals(a, b));
}

TEST(utilitiesTest, filesEqual) {
  EXPECT_TRUE(iddt::files_equal("Makefile", "Makefile"));
  EXPECT_FALSE(iddt::files_equal("Makefile", "configure"));
}

TEST(utilitiesTest, stringFloatLessThan) {
  EXPECT_TRUE(iddt::string_float_less_than(std::make_pair("a", 0.1f),
                                           std::make_pair("b", 0.2f)));
  EXPECT_FALSE(iddt::string_float_less_than(std::make_pair("a", 0.2f),
                                            std::make_pair("b", 0.1f)));
  EXPECT_FALSE(iddt::string_float_less_than(std::make_pair("a", 0.1f),
                                            std::make_pair("b", 0.1f)));
  EXPECT_FALSE(iddt::string_float_less_than(std::make_pair("a", 1.0f / 0.0f),
                                            std::make_pair("b", 1.0f / 0.0f)));
}
