/*!
 \file cargs.h
 \brief command line argument handling
 \author Lightning Auriga
 \note requires boost::program_options library + headers
 \copyright Released under the MIT License.
 Copyright 2023 Lightning Auriga

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
#include "imputed-data-dynamic-threshold/config.h"

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
  cargs(int argc, char **argv);
  /*!
    \brief copy constructor
    @param obj existing cargs object
   */
  cargs(const cargs &obj);
  /*!
    \brief destructor
   */
  ~cargs() throw();

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
  bool help() const;

  /*!
   * \brief determine whether the user has requested the program version
   * \return whether the user has requested the program version
   */
  bool version() const;

  /*!
    \brief determine whether the user has requested a second read pass
    to report passing variant IDs
    \return whether the user has requested this run mode

    this flag is provided in case the number of total IDs in the input
    info files (e.g. for TOPMed) exceeds a user's available RAM
  */
  bool second_pass() const;

  /*!
    \brief get optional output directory for filtered info files
    \return optional output directory for filtered info files

    this is an optional mode on top of second-pass mode that causes
    filtered info files to be reported to an output directory under
    the same filename as input
   */
  std::string get_filter_info_files_dir() const;
  /*!
    \brief get optional output directory for filtered vcf files
    \return optional output directory for filtered vcf files

    this is an optional mode on top of second-pass mode that causes
    filtered vcf files to be reported to an output directory under
    the same filename as input
   */
  std::string get_filter_vcf_files_dir() const;
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
  std::vector<double> get_maf_bin_boundaries() const;

  /*!
    \brief get info filenames for parsing
    \return info filenames for parsing

    files are expected to be gzipped raw output from minimac4.
   */
  std::vector<std::string> get_info_gz_files() const;
  /*!
    \brief get vcf filenames for parsing
    \return vcf filenames for parsing
   */
  std::vector<std::string> get_vcf_files() const;

  /*!
    \brief get INFO tag in input vcfs for imputation r2
    \return INFO tag in input vcfs for imputation r2
   */
  std::string get_vcf_info_r2_tag() const;
  /*!
    \brief get INFO tag in input vcfs for allele frequency
    \return INFO tag in input vcfs for allele frequency
   */
  std::string get_vcf_info_af_tag() const;
  /*!
    \brief get INFO tag in input vcfs for imputation indicator
    \return INFO tag in input vcfs for imputation indicator

    this is a binary indicator, such that its presence in an INFO field
    means that the corresponding record is purely imputed, and not present
    in the chip input data to imputation. chip input variants are ignored
    for the purpose of the dynamic threshold.
   */
  std::string get_vcf_info_imputed_indicator() const;

  /*!
    \brief get target average r2 per bin
    \return target average r2 per bin from command line

    value should be on [0,1]. an automatic filter of 0.3 is
    unconditionally applied, so values on [0,0.3] will be
    automatically satisfied with a final threshold of 0.3.
   */
  double get_target_average_r2() const;

  /*!
    \brief get output tabular result filename
    \return output tabular result filename from command line

    output file will be unconditionally overwritten if present.
    target directory chain needs to exist, or the program
    will exit and complain of being unable to open the file.
   */
  std::string get_output_table_filename() const;

  /*!
    \brief get output list filename
    \return output list filename from command line

    this parameter is optional, and controls the emission
    of a variant list for variants passing thresholds. the
    output list will be unconditionally overwritten if present.
    target directory chain needs to exist, or the program
    will exit and complain of being unable to open the file.
   */
  std::string get_output_list_filename() const;

  /*!
    \brief find status of arbitrary flag
    @param tag name of flag
    \return whether the flag is set

    This is the underlying accessor function used by the custom flag
    accessors, and can be used for arbitrary flag additions if you don't want
    to type out the custom access functions or want to allow dynamic
    specification from a config file.
   */
  bool compute_flag(const std::string &tag) const;
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
  void print_help(std::ostream &out);

  /*!
   * \brief report version information to arbitrary output stream
   * \param out stream to which to write version information
   */
  void print_version(std::ostream &out);

 private:
  /*!
    \brief default constructor
    \warning disabled
   */
  cargs();
  boost::program_options::options_description
      _desc;  //!< help documentation string
  boost::program_options::variables_map
      _vm;  //!< storage of parsed command line settings
};
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_CARGS_H_
