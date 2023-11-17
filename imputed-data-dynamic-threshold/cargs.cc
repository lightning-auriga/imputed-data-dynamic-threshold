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
      "filter-info-files", boost::program_options::value<std::string>(),
      "(optional) output filtered info file directory; only possible if "
      "second-pass mode is enabled (default: do not write filtered info "
      "files)")(
      "filter-vcf-files", boost::program_options::value<std::string>(),
      "(optional) output filtered vcf file directory; only possible if "
      "second-pass mode is enabled (default: do not write filtered vcf files)")(
      "target-average-r2,r",
      boost::program_options::value<double>()->default_value(0.9),
      "average r2 target for each minor allele frequency bin")(
      "vcf-files,v",
      boost::program_options::value<std::vector<std::string> >()->multitoken(),
      "vcf files containing imputation r2, allele frequency, and imputation "
      "status info fields")(
      "info-gz-files,i",
      boost::program_options::value<std::vector<std::string> >()->multitoken(),
      "info.gz output files from minimac4")(
      "maf-bin-boundaries,m",
      boost::program_options::value<std::vector<double> >()->multitoken(),
      "boundaries for minor allele frequency bins")(
      "vcf-info-r2-tag",
      boost::program_options::value<std::string>()->default_value("DR2"),
      "vcf INFO field tag corresponding to imputation r2")(
      "vcf-info-af-tag",
      boost::program_options::value<std::string>()->default_value("AF"),
      "vcf INFO field tag corresponding to variant allele frequency")(
      "vcf-info-imputed-indicator",
      boost::program_options::value<std::string>()->default_value("IMP"),
      "vcf INFO field tag indicating that a variant was imputed from a "
      "reference");
}
