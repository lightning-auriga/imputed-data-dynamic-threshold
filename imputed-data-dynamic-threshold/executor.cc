/*!
  \file executor.cc
  \brief implementation of wrapped program execution.
  \copyright Released under the MIT License. Copyright
  2023 Lightning Auriga
 */

#include "imputed-data-dynamic-threshold/executor.h"

namespace iddt = imputed_data_dynamic_threshold;

iddt::executor::executor() {}
iddt::executor::~executor() throw() {}

void iddt::executor::run(
    const std::vector<double> &maf_bin_boundaries,
    const std::vector<std::string> &info_files,
    const std::vector<std::string> &vcf_files, const double &target_r2,
    const float &baseline_r2, const std::string &output_table_filename,
    const std::string &output_list_filename, bool second_pass,
    const std::string &filter_info_files_dir, const std::string &vcf_r2_tag,
    const std::string &vcf_af_tag, const std::string &vcf_imp_indicator) {
  imputed_data_dynamic_threshold::r2_bins bins;
  bins.set_baseline_r2(baseline_r2);
  std::cout << "creating MAF bins" << std::endl;
  bins.set_bin_boundaries(maf_bin_boundaries);
  if (!info_files.empty()) {
    std::cout << "iterating through specified info files" << std::endl;
    for (std::vector<std::string>::const_iterator iter = info_files.begin();
         iter != info_files.end(); ++iter) {
      std::cout << "\t" << *iter << std::endl;
      bins.load_info_file(*iter, !second_pass);
    }
  }
  if (!vcf_files.empty()) {
    std::cout << "iterating through specified vcf files" << std::endl;
    for (std::vector<std::string>::const_iterator iter = vcf_files.begin();
         iter != vcf_files.end(); ++iter) {
      std::cout << "\t" << *iter << std::endl;
      bins.load_vcf_file(*iter, vcf_r2_tag, vcf_af_tag, vcf_imp_indicator,
                         !second_pass);
    }
  }

  std::cout << "computing bin-specific r2 thresholds" << std::endl;
  bins.compute_thresholds(target_r2);
  std::ofstream output;
  if (!output_table_filename.empty()) {
    std::cout << "reporting tabular results to \"" << output_table_filename
              << "\"" << std::endl;
    output.open(output_table_filename.c_str());
    if (!output.is_open())
      throw std::runtime_error("cannot write to file \"" +
                               output_table_filename + "\"");
  } else {
    std::cout << "reporting tabular results to terminal" << std::endl;
  }
  bins.report_thresholds(output_table_filename.empty() ? std::cout : output);
  output.close();
  output.clear();
  if (!output_list_filename.empty()) {
    output.open(output_list_filename.c_str());
    if (!output.is_open())
      throw std::runtime_error("cannot write to file \"" +
                               output_list_filename + "\"");
    std::cout << "reporting passing variants to \"" << output_list_filename
              << "\"" << std::endl;
    if (second_pass) {
      for (std::vector<std::string>::const_iterator iter = info_files.begin();
           iter != info_files.end(); ++iter) {
        std::cout << "\t" << *iter << std::endl;
        bins.report_passing_variants(*iter, filter_info_files_dir, output);
      }
    } else {
      bins.report_passing_variants(output);
    }
    output.close();
    output.clear();
  }
}
