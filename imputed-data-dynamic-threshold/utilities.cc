/*!
  \file utilities.cc
  \brief implementations for global functions
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
*/

#include "imputed-data-dynamic-threshold/utilities.h"

bool imputed_data_dynamic_threshold::float_vector_equals(
    const std::vector<float> &v1, const std::vector<float> &v2) {
  if (v1.size() != v2.size()) return false;
  for (unsigned i = 0; i < v1.size(); ++i) {
    if (fabs(v1.at(i) - v2.at(i)) >= FLT_EPSILON) return false;
  }
  return true;
}

bool imputed_data_dynamic_threshold::files_equal(const std::string &f1,
                                                 const std::string &f2) {
  std::ifstream if1, if2;
  if1.open(f1.c_str(), std::ios::binary);
  if2.open(f2.c_str(), std::ios::binary);
  if (!if1.is_open() && !if2.is_open()) return true;
  if (!if1.is_open() || !if2.is_open()) return false;
  std::istream_iterator<char> it1(if1), end1;
  std::istream_iterator<char> it2(if2), end2;
  while (it1 != end1 && it2 != end2) {
    if (*it1 != *it2) return false;
    ++it1;
    ++it2;
  }
  if (it1 != end1 || it2 != end2) return false;
  return true;
}

bool imputed_data_dynamic_threshold::string_float_less_than(
    const std::pair<std::string, float> &p1,
    const std::pair<std::string, float> &p2) {
  return p1.second > p2.second;
}
