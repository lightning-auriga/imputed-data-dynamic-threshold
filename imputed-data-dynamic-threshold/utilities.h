/*!
  \file utilities.h
  \brief general function utilities and templates
  \copyright Released under the MIT License. Copyright
  2023 Lightning Auriga
 */

#ifndef IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_
#define IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_

#include <cfloat>
#include <cmath>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace imputed_data_dynamic_threshold {
/*!
  \brief convert object string representation to object
  @tparam value_type object type
  @param s string representation
  \return object version of string
 */
template <class value_type>
value_type from_string(const std::string &s) {
  std::istringstream strm1(s);
  value_type res;
  if (!(strm1 >> res))
    throw std::runtime_error(
        "cannot convert string "
        "to object: \"" +
        s + "\"");
  return res;
}

/*!
  \brief compare two pair(string, float) vectors for approximate equality
  @param v1 first vector for comparison
  @param v2 second vector for comparison

  comparison permissible delta is FLT_EPSILON from cfloat
 */
bool string_float_vector_equals(
    const std::vector<std::pair<std::string, float> > &v1,
    const std::vector<std::pair<std::string, float> > &v2);
/*!
  \brief compare two files for exact identity
  @param f1 name of first file
  @param f2 name of second file
 */
bool files_equal(const std::string &f1, const std::string &f2);
/*!
  \brief compare pairs of strings and floats by their float entry
  @param p1 first pair to compare
  @param p2 second pair to compare
  \return whether float entry of first pair is less than that of the second
 */
bool string_float_less_than(const std::pair<std::string, float> &p1,
                            const std::pair<std::string, float> &p2);
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_
