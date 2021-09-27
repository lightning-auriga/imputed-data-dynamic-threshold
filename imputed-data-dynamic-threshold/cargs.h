/*!
 \file cargs.h
 \brief command line argument handling
 \author Lightning Auriga
 \note requires boost::program_options library + headers
 \copyright Released under the MIT License.
 Copyright 2021 Lightning Auriga

 Thanks to
 https://www.boost.org/doc/libs/1_70_0/doc/html/program_options/tutorial.html#id-1.3.32.4.3
 */

#ifndef IMPUTED_DATA_DYNAMIC_THRESHOLD_CARGS_H_
#define IMPUTED_DATA_DYNAMIC_THRESHOLD_CARGS_H_

#include <stdexcept>
#include <string>
#include <vector>
#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

namespace imputed_data_dynamic_threshold {
/*!
  \class cargs
  \brief command line argument parser using boost::program_options
 */
class cargs {
 public:
  /*!
    \brief constructor with program arguments
    @param argc number of arguments including program name
    @param argv string array containing actual arguments
   */
  cargs(int argc, char **argv) : _desc("Recognized options") {
    initialize_options();
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, _desc), _vm);
    boost::program_options::notify(_vm);
  }
  /*!
    \brief copy constructor
    @param obj existing cargs object
   */
  cargs(const cargs &obj) : _desc(obj._desc), _vm(obj._vm) {}
  /*!
    \brief destructor
   */
  ~cargs() throw() {}

  /*!
    \brief set user help documentation and default values for parameters as
    needed

    Note the weird syntax with overloaded () operators. The lists are not
    separated by commas.
   */
  void initialize_options();

  /*!
    \brief determine whether the user has requested help documentation
    \return whether the user has requested help documentation

    This test is separate from whether the user has run the software with no
    flags
   */
  bool help() const { return compute_flag("help"); }

  /*!
    \brief determine whether the user has requested a second read pass
    to report passing variant IDs
    \return whether the user has requested this run mode

    this flag is provided in case the number of total IDs in the input
    info files (e.g. for TOPMed) exceeds a user's available RAM
  */
  bool second_pass() const { return compute_flag("second-pass"); }

  /*!
    \brief get boundaries of minor allele frequency bins for r2 calculations
    \return MAF bin boundaries from command line

    all entries should be valid minor allele frequencies for biallelic
    variants and sorted from smallest to largest.
    these boundaries are interpreted as complete bounds on the considered
    frequencies; variants above or below the max and min values,
    respectively, are ignored. upper bin boundaries are inclusive,
    while lower bin boundaries are exclusive. these bins default to:
    (0.001, 0.005], (0.005, 0.01], (0.01, 0.03], (0.03, 0.05], (0.05, 0.5],
    according to doi:10.1002/gepi.21603.
   */
  std::vector<double> get_maf_bin_boundaries() const {
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

  /*!
    \brief get info filenames for parsing
    \return info filenames for parsing

    files are expected to be gzipped raw output from minimac4.
   */
  std::vector<std::string> get_info_gz_files() const {
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

  /*!
    \brief get target average r2 per bin
    \return target average r2 per bin from command line

    value should be on [0,1]. an automatic filter of 0.3 is
    unconditionally applied, so values on [0,0.3] will be
    automatically satisfied with a final threshold of 0.3.
   */
  double get_target_average_r2() const {
    double res = compute_parameter<double>("target-average-r2");
    if (res < 0.0 || res > 1.0)
      throw std::runtime_error(
          "invalid r2 value provided to "
          "--target-average-r2");
    return res;
  }

  /*!
    \brief get output tabular result filename
    \return output tabular result filename from command line

    output file will be unconditionally overwritten if present.
    target directory chain needs to exist, or the program
    will exit and complain of being unable to open the file.
   */
  std::string get_output_table_filename() const {
    return compute_parameter<std::string>("output-table");
  }

  /*!
    \brief get output list filename
    \return output list filename from command line

    this parameter is optional, and controls the emission
    of a variant list for variants passing thresholds. the
    output list will be unconditionally overwritten if present.
    target directory chain needs to exist, or the program
    will exit and complain of being unable to open the file.
   */
  std::string get_output_list_filename() const {
    return compute_parameter<std::string>("output-list");
  }

  /*!
    \brief find status of arbitrary flag
    @param tag name of flag
    \return whether the flag is set

    This is the underlying accessor function used by the custom flag
    accessors, and can be used for arbitrary flag additions if you don't want
    to type out the custom access functions or want to allow dynamic
    specification from a config file.
   */
  bool compute_flag(const std::string &tag) const { return _vm.count(tag); }
  /*!
    \brief find value of arbitrary parameter
    @tparam value_type class to which the value should be cast
    @param tag name of parameter
    \return value of parameter if specified

    \warning throws exception if parameter was not set and had no default
   */
  template <class value_type>
  value_type compute_parameter(const std::string &tag) const {
    if (_vm.count(tag)) {
      return _vm[tag].as<value_type>();
    }
    throw std::domain_error("cargs: requested parameter \"" + tag + "\" unset");
  }

  /*!
    \brief report help documentation to arbitrary output stream
    @param out stream to which to write help documentation

    Parameter should probably be std::cout or std::cerr at your preference.
   */
  void print_help(std::ostream &out) {
    if (!(out << _desc))
      throw std::domain_error("cargs::print_help: unable to write to stream");
  }

 private:
  /*!
    \brief default constructor
    \warning disabled
   */
  cargs() { throw std::domain_error("cargs: do not use default constructor"); }
  boost::program_options::options_description
      _desc;  //!< help documentation string
  boost::program_options::variables_map
      _vm;  //!< storage of parsed command line settings
};
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_CARGS_H_
