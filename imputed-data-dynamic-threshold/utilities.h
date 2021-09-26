/*!
  \file utilities.h
  \brief general function utilities and templates
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#ifndef IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_
#define IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_

#include <cfloat>
#include <cmath>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
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
  \brief compare two float vectors for approximate equality
  @param v1 first vector for comparison
  @param v2 second vector for comparison

  comparison permissible delta is FLT_EPSILON from cfloat
 */
bool float_vector_equals(const std::vector<float> &v1,
                         const std::vector<float> &v2);
/*!
  \brief compare two files for exact identity
  @param f1 name of first file
  @param f2 name of second file
 */
bool files_equal(const std::string &f1, const std::string &f2);
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_
