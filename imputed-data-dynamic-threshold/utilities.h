/*!
  \file utilities.h
  \brief general function utilities and templates
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#ifndef IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_
#define IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_

#include <sstream>
#include <string>

namespace imputed_data_dynamic_threshold {
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
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_UTILITIES_H_
