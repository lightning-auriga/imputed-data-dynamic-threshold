/*!
  \file executor.h
  \brief wrapped main execution of program for testing.
  \copyright Released under the MIT License. Copyright
  2023 Lightning Auriga
 */

#ifndef IMPUTED_DATA_DYNAMIC_THRESHOLD_EXECUTOR_H_
#define IMPUTED_DATA_DYNAMIC_THRESHOLD_EXECUTOR_H_

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "imputed-data-dynamic-threshold/cargs.h"
#include "imputed-data-dynamic-threshold/r2_bins.h"

namespace imputed_data_dynamic_threshold {
/*!
 * \class executor
 * \brief class handling primary program execution
 */
class executor {
 public:
  /*!
   * \brief basic constructor
   */
  executor();
  /*!
   * \brief destructor
   */
  ~executor() throw();
  /*!
   * \brief run primary program logic
   * \param maf_bin_boundaries maf values defining filtering regions
   * \param info_files names of input files containing variant r2 data
   * \param target_r2 target average r2 per bin after filtering
   * \param baseline_r2 minimum r2 for any imputed variant
   * \param output_table_filename name of file to which to report summary report
   * \param output_list_filename name of file to which to report passing
   * variants \param second_pass whether to save RAM and run in two passes
   * \param filter_info_files_dir directory to which to write filtered files
   */
  void run(const std::vector<double> &maf_bin_boundaries,
           const std::vector<std::string> &info_files, const double &target_r2,
           const float &baseline_r2, const std::string &output_table_filename,
           const std::string &output_list_filename, bool second_pass,
           const std::string &filter_info_files_dir);
};
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_EXECUTOR_H_
