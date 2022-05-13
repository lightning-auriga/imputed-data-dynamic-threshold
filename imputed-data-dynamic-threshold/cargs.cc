/*!
  \file cargs.cc
  \brief method implementation for command line argument parser class
  \copyright Released under the MIT License.
  Copyright 2021 Lightning Auriga
*/

#include "imputed-data-dynamic-threshold/cargs.h"

void imputed_data_dynamic_threshold::cargs::initialize_options() {
  _desc.add_options()("help,h", "emit this help message")(
      "output-table,o", boost::program_options::value<std::string>(),
      "output summary table filename for reporting bin thresholds; if not "
      "specified, report table to terminal")(
      "output-list,l", boost::program_options::value<std::string>(),
      "(optional) output variant list filename for reporting variants passing "
      "dynamic thresholds")("second-pass,s",
                            "whether to pass through info files a second time "
                            "to report passing variant IDs, to save RAM "
                            "(default: no)")(
      "filter-files,f", boost::program_options::value<std::string>(),
      "(optional) output filtered info file directory; only possible if "
      "second-pass mode is enabled (default: do not write filtered files)")(
      "target-average-r2,r",
      boost::program_options::value<double>()->default_value(0.9),
      "average r2 target for each minor allele frequency bin")(
      "info-gz-files,i",
      boost::program_options::value<std::vector<std::string> >()->multitoken(),
      "info.gz output files from minimac4")(
      "maf-bin-boundaries,m",
      boost::program_options::value<std::vector<double> >()->multitoken(),
      "boundaries for minor allele frequency bins");
}
