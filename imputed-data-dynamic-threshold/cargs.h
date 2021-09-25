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
    \brief access first imputed data info file
    \return name of first imputed data info file, if specified

    Note that this option automatically supports .gz and .bz2 file extensions
   */
  std::string get_set1_imputed_info() const {
    return compute_parameter<std::string>("set1-imputed-info");
  }
  /*!
    \brief access first imputed data vcf file
    \return name of first imputed data vcf file

    Note that this option automatically supports .gz and .bz2 file extensions
   */
  std::string get_set1_imputed_vcf() const {
    return compute_parameter<std::string>("set1-imputed-vcf");
  }
  /*!
    \brief access second imputed data info file
    \return name of second imputed data info file

    Note that this option automatically supports .gz and .bz2 file extensions
   */
  std::string get_set2_imputed_info() const {
    return compute_parameter<std::string>("set2-imputed-info");
  }
  /*!
    \brief access second imputed data vcf file
    \return name of second imputed data vcf file

    Note that this option automatically supports .gz and .bz2 file extensions
   */
  std::string get_set2_imputed_vcf() const {
    return compute_parameter<std::string>("set2-imputed-vcf");
  }
  /*!
    \brief access reference dataset vcf file
    \return name of reference dataset vcf file

    Note that this option automatically supports .gz and .bz2 file extensions
   */
  std::string get_reference_vcf() const {
    return compute_parameter<std::string>("reference-vcf");
  }
  /*!
    \brief access requested prefix of output file(s)
    \return prefix of output file(s)
   */
  std::string get_output_prefix() const {
    return compute_parameter<std::string>("output-prefix");
  }
  /*!
    \brief find out whether user wants output run through gzip
    \return whether user wants output run through gzip

    Will automatically append appropriate suffix. Note that gzip < bzip2
    most of the time and so will be overruled if both are specified.
   */
  bool requests_gzip() const { return compute_flag("gzip"); }
  /*!
    \brief find out whether user wants output run through bzip2
    \return whether user wants output run through bzip2

    Will automatically append appropriate suffix. Note that bzip2 > gzip2
    most of the time and so will take precedence if both are specified.
   */
  bool requests_bzip2() const { return compute_flag("bzip2"); }
  /*!
    \brief access requested user-friendly name of first imputed dataset
    \return user-friendly name of first imputed dataset

    Defaults to "GSA" based on initial testcase for this software
   */
  std::string get_set1_pretty_name() const {
    return compute_parameter<std::string>("set1-pretty-name");
  }
  /*!
    \brief access requested user-friendly name of second imputed dataset
    \return user-friendly name of second imputed dataset

    Defaults to "MEGA" based on initial testcase for this software
   */
  std::string get_set2_pretty_name() const {
    return compute_parameter<std::string>("set2-pretty-name");
  }

  /*!
    \brief for debugging purposes, turn off probability access mode by default
    \return whether to turn probability consistency mode back on
   */
  bool enable_probability_mode() const {
    return compute_flag("enable-probability-mode");
  }

  /*!
    \brief access requested number of fine-grain bins for probability analysis
    \return requested number of bins

    \warning this can really increase the file size terribly
    \warning this is used both in initial and merge modes
   */
  unsigned get_probability_bin_count() const {
    return compute_parameter<unsigned>("probability-bin-count");
  }

  /*!
    \brief access requested number of bins for frequency analysis
    \return requested number of bins

    \warning this is only used in merge mode. it is ignored in initial
    analysis mode.
   */
  unsigned get_frequency_bin_count() const {
    return compute_parameter<unsigned>("frequency-bin-count");
  }
  /*!
    \brief get minimum permissible imputed variant r2
    \return minimum permissible imputed variant r2
   */
  double get_minimum_r2() const {
    return compute_parameter<double>("r2-lower-bound");
  }
  /*!
    \brief get maximum permissible imputed variant r2
    \return maximum permissible imputed variant r2
   */
  double get_maximum_r2() const {
    return compute_parameter<double>("r2-upper-bound");
  }
  /*!
    \brief determine whether the user wants chip variants removed from output
    files \return whether the user wants chip variants removed from output
    files
   */
  bool remove_chip_variants() const {
    return compute_flag("remove-chip-variants");
  }
  /*!
    \brief determine whether the user has requested help documentation
    \return whether the user has requested help documentation

    This test is separate from whether the user has run the software with no
    flags
   */
  bool help() const { return compute_flag("help"); }

  /*!
    \brief determine whether merge mode on previous run's output is requested
    \return whether merge mode is requested
   */
  bool get_merge_mode() const { return compute_flag("merge-mode"); }

  /*!
    \brief get the arbitrary number of first round files that should be merged
    \return all requested merge files, in a vector
   */
  std::vector<std::string> get_merge_files() const {
    return compute_parameter<std::vector<std::string> >("merge-files");
  }

  /*!
    \brief get the optional list of variants to include for set 1 during the
    merge \return specified file, or "" if none specified

    This facilitates the use of dynamic r2 thresholding
   */
  std::string get_merge_set1_inclusion_variant_filename() const {
    return compute_parameter<std::string>("merge-set1-inclusion-variants");
  }

  /*!
    \brief get the optional list of variants to include for set 2 during the
    merge \return specified file, or "" if none specified

    This facilitates the use of dynamic r2 thresholding
   */
  std::string get_merge_set2_inclusion_variant_filename() const {
    return compute_parameter<std::string>("merge-set2-inclusion-variants");
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
