/*!
  \file main.cc
  \brief main entry/exit for software. interprets command line arguments,
  dispatches tasks, exits
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

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
  std::vector<double> maf_bin_boundaries = ap.get_maf_bin_boundaries();
  std::vector<std::string> info_files = ap.get_info_gz_files();
  double target_r2 = ap.get_target_average_r2();
  std::string output_filename = ap.get_output_filename();
  std::cout << "all done woo!" << std::endl;
  return 0;
}
