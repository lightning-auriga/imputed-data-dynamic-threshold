/*!
  \file cargs.cc
  \brief method implementation for command line argument parser class
  \copyright Released under the MIT License.
  Copyright 2023 Lightning Auriga
*/

#include "imputed-data-dynamic-threshold/cargs.h"

namespace iddt = imputed_data_dynamic_threshold;

void imputed_data_dynamic_threshold::cargs::initialize_options() {
  _desc.add_options()("help,h", "emit this help message")(
      "version", "emit program version and exit")(
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
      boost::program_options::value<std::string>()->default_value("0.9"),
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

iddt::cargs::cargs(int argc, const char **const argv)
    : _desc("Recognized options") {
  initialize_options();
  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, _desc), _vm);
  boost::program_options::notify(_vm);
}
iddt::cargs::cargs(const cargs &obj) : _desc(obj._desc), _vm(obj._vm) {}

iddt::cargs::~cargs() throw() {}

void iddt::cargs::initialize_options();

bool iddt::cargs::version() const { return compute_flag("version"); }

bool iddt::cargs::help() const { return compute_flag("help"); }

bool iddt::cargs::second_pass() const { return compute_flag("second-pass"); }

std::string iddt::cargs::get_filter_info_files_dir() const {
  if (_vm.count("filter-info-files"))
    return compute_parameter<std::string>("filter-info-files");
  return "";
}
std::string iddt::cargs::get_filter_vcf_files_dir() const {
  if (_vm.count("filter-vcf-files"))
    return compute_parameter<std::string>("filter-vcf-files");
  return "";
}
std::vector<double> iddt::cargs::get_maf_bin_boundaries() const {
  std::string tag = "maf-bin-boundaries";
  std::vector<double> res;
  if (_vm.count(tag)) {
    res = compute_parameter<std::vector<double> >(tag);
    for (unsigned i = 0; i < res.size(); ++i) {
      if (res.at(i) < 0.0 || res.at(i) > 0.5) {
        throw std::runtime_error(
            "invalid minor allele frequency "
            "specified to --maf-bin-boundaries");
      }
      if (i > 0 && res.at(i) <= res.at(i - 1)) {
        throw std::runtime_error(
            "--maf-bin-boundaries: values are "
            "interpreted as sequential MAF bin "
            "limits and should be sorted in "
            "strictly increasing order");
      }
    }
  } else {
    res.push_back(0.001);
    res.push_back(0.005);
    res.push_back(0.01);
    res.push_back(0.03);
    res.push_back(0.05);
    res.push_back(0.5);
  }
  return res;
}
std::vector<std::string> iddt::cargs::get_info_gz_files() const {
  std::vector<std::string> vec;
  vec = compute_parameter<std::vector<std::string> >("info-gz-files");
  for (std::vector<std::string>::const_iterator iter = vec.begin();
       iter != vec.end(); ++iter) {
    if (!boost::filesystem::is_regular_file(*iter)) {
      throw std::runtime_error("argument of -i is not a regular file: \"" +
                               *iter + "\"");
    }
  }
  return vec;
}
std::vector<std::string> iddt::cargs::get_vcf_files() const {
  std::vector<std::string> vec;
  vec = compute_parameter<std::vector<std::string> >("vcf-files");
  for (std::vector<std::string>::const_iterator iter = vec.begin();
       iter != vec.end(); ++iter) {
    if (!boost::filesystem::is_regular_file(*iter)) {
      throw std::runtime_error("argument of -v is not a regular file: \"" +
                               *iter + "\"");
    }
  }
  return vec;
}
std::string iddt::cargs::get_vcf_info_r2_tag() const {
  return compute_parameter<std::string>("vcf-info-r2-tag");
}
std::string iddt::cargs::get_vcf_info_af_tag() const {
  return compute_parameter<std::string>("vcf-info-af-tag");
}
std::string iddt::cargs::get_vcf_info_imputed_indicator() const {
  return compute_parameter<std::string>("vcf-info-imputed-indicator");
}
double iddt::cargs::get_target_average_r2() const {
  double res =
      from_string<double>(compute_parameter<std::string>("target-average-r2"));
  if (res < 0.0 || res > 1.0)
    throw std::runtime_error(
        "invalid r2 value provided to "
        "--target-average-r2");
  return res;
}
std::string iddt::cargs::get_output_table_filename() const {
  if (_vm.count("output-table"))
    return compute_parameter<std::string>("output-table");
  return "";
}
std::string iddt::cargs::get_output_list_filename() const {
  if (_vm.count("output-list"))
    return compute_parameter<std::string>("output-list");
  return "";
}
bool iddt::cargs::compute_flag(const std::string &tag) const {
  return _vm.count(tag);
}
void iddt::cargs::print_help(std::ostream &out) {
  if (!(out << _desc))
    throw std::domain_error("cargs::print_help: unable to write to stream");
}

iddt::cargs::cargs() {
  throw std::domain_error("cargs: do not use default constructor");
}

void iddt::cargs::print_version(std::ostream &out) {
  if (!(out << IMPUTED_DATA_DYNAMIC_THRESHOLD_PACKAGE_STRING << std::endl))
    throw std::domain_error("cargs::print_version: unable to write to stream");
}
