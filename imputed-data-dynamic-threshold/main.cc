/*!
  \file main.cc
  \brief main entry/exit for software. interprets command line arguments,
  dispatches tasks, exits
  \copyright Released under the MIT License. Copyright
  2023 Lightning Auriga
 */

#include <iostream>
#include <string>
#include <vector>

#include "imputed-data-dynamic-threshold/cargs.h"
#include "imputed-data-dynamic-threshold/executor.h"

/*!
  \brief main program implementation
  @param argc number of command line entries, including program name
  @param argv array of command line entries
 */
int main(int argc, char **argv) {
  // parse command line input
  imputed_data_dynamic_threshold::cargs ap(argc, argv);
  // if help is requested or no flags specified
  if (ap.help() || argc == 1) {
    // print a help message and exist
    ap.print_help(std::cout);
    return 0;
  }
  if (ap.version()) {
    // print version information and exit
    ap.print_version(std::cout);
    return 0;
  }
  std::vector<double> maf_bin_boundaries = ap.get_maf_bin_boundaries();
  std::vector<std::string> info_files = ap.get_info_gz_files();
  double target_r2 = ap.get_target_average_r2();
  std::string output_table_filename = ap.get_output_table_filename();
  std::string output_list_filename = ap.get_output_list_filename();
  bool second_pass = ap.second_pass();
  std::string filter_info_files_dir = "";
  if (second_pass) {
    filter_info_files_dir = ap.get_filter_info_files_dir();
  }
  imputed_data_dynamic_threshold::executor ex;
  ex.run(maf_bin_boundaries, info_files, target_r2, output_table_filename,
         output_list_filename, second_pass, filter_info_files_dir);

  std::cout << "all done woo!" << std::endl;
  return 0;
}
