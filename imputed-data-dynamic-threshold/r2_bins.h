/*!
  \file r2_bins.h
  \brief collect r2 data within MAF bins
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#ifndef IMPUTED_DATA_DYNAMIC_THRESHOLD_R2_BINS_H_
#define IMPUTED_DATA_DYNAMIC_THRESHOLD_R2_BINS_H_

#include <zlib.h>
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "imputed-data-dynamic-threshold/utilities.h"

namespace imputed_data_dynamic_threshold {
/*!
  \brief aggregate variant data and apply
  dynamic filtering to reach a target average r2
 */
class r2_bin {
 public:
  /*!
    \brief default constructor
  */
  r2_bin()
      : _bin_min(0.0),
        _bin_max(0.0),
        _total(0.0),
        _total_count(0u),
        _filtered_count(0u),
        _threshold(0.0f) {}
  /*!
    \brief copy constructor
    @param obj existing r2_bin object
   */
  r2_bin(const r2_bin &obj)
      : _bin_min(obj._bin_min),
        _bin_max(obj._bin_max),
        _data(obj._data),
        _total(obj._total),
        _total_count(obj._total_count),
        _filtered_count(obj._filtered_count),
        _threshold(obj._threshold) {}
  /*!
    \brief destructor
   */
  ~r2_bin() throw() {}
  /*!
    \brief record the MAF bounds of this bin
    @param minimum min MAF for this bin
    @param maximum max MAF for this bin

    note that this is really just for recordkeeping,
    as this class does not enforce this restriction here
   */
  void set_bin_bounds(const double &minimum, const double &maximum) {
    _bin_min = minimum;
    _bin_max = maximum;
  }
  /*!
    \brief add a variant r2 to existing aggregation
    @param id variant ID
    @param val r2 from a variant fitting into this bin
   */
  void add_value(const std::string &id, const float &val);
  /*!
    \brief compute r2 threshold required to meet a given average r2 target
    @param target desired average r2 after additional filtering is applied
   */
  void compute_threshold(const double &target);
  /*!
    \brief report r2 threshold applied and attrition due to the threshold
    @param out target output stream for writing content
   */
  void report_threshold(std::ostream &out);
  /*!
    \brief report stored r2 threshold
    \return stored r2 threshold

    note that this requires report_threshold to have been called first,
    and will except otherwise
   */
  float report_stored_threshold() const;
  /*!
    \brief report variant IDs for variants passing threshold
    @param out output stream for data reporting
   */
  void report_passing_variants(std::ostream &out) const;
  /*!
    \brief test for equality between objects of this class
    @param obj object to compare to *this
    \return whether all entries in the object are (approximately) equal
   */
  bool operator==(const r2_bin &obj) const;
  /*!
    \brief test for inequality between objects of this class
    @param obj object to compare to *this
    \return whether any entry in this object is not equal to the corresponding
    one
   */
  bool operator!=(const r2_bin &obj) const;
  /*!
    \brief get stored bin minimum frequency value
    \return bin minimum frequency value
   */
  const double &get_bin_min() const { return _bin_min; }
  /*!
    \brief get stored bin maximum frequency value
    \return bin maximum frequency value
   */
  const double &get_bin_max() const { return _bin_max; }
  /*!
    \brief get stored r2 data vector
    \return stored r2 data vector
   */
  const std::vector<std::pair<std::string, float> > &get_data() const {
    return _data;
  }
  /*!
    \brief get stored running r2 sum
    \return stored running r2 sum
   */
  const double &get_total() const { return _total; }
  /*!
    \brief get total number of stored datapoints
    \return total number of stored datapoints
  */
  unsigned get_total_count() const { return _total_count; }
  /*!
    \brief get number of datapoints remaining after current filter
    \return number of datapoints remaining after current filter
   */
  unsigned get_filtered_count() const { return _filtered_count; }
  friend class r2_bin_test;

 protected:
  double _bin_min;  //!< minimum MAF in this bin, exclusive
  double _bin_max;  //!< maximum MAF in this bin, inclusive
  std::vector<std::pair<std::string, float> > _data;  //!< aggregated r2 data
  double _total;             //!< running sum of all loaded r2 at current filter
  unsigned _total_count;     //!< total number of loaded variants
  unsigned _filtered_count;  //!< number of variants left with current filter
  float _threshold;          //!< stored r2 threshold to meet target
};
/*!
  \brief dispatch variants to bins by MAF and handle I/O
 */
class r2_bins {
 public:
  /*!
    \brief default constructor
   */
  r2_bins() {}
  /*!
    \brief copy constructor
    @param obj existing r2_bins object
   */
  r2_bins(const r2_bins &obj)
      : _bins(obj._bins),
        _bin_lower_bounds(obj._bin_lower_bounds),
        _bin_upper_bounds(obj._bin_upper_bounds) {}
  /*!
    \brief destructor
   */
  ~r2_bins() throw() {}
  /*!
    \brief create MAF bins based on user-specified boundaries
    @param boundaries sequential min/max bounds for MAF bins
   */
  void set_bin_boundaries(const std::vector<double> &boundaries);
  /*!
    \brief using boundary maps, find appropriate bin for a MAF
    @param maf query MAF
    \return index of target bin, or invalid index if MAF doesn't fit
    in a generated bin
   */
  unsigned find_maf_bin(const double &maf) const;
  /*!
    \brief load r2 and MAF data from minimac4 info.gz file
    @param filename name of info.gz file to load
    @param store_ids whether to store variant IDs for later reporting
   */
  void load_info_file(const std::string &filename, bool store_ids);
  /*!
    \brief compute bin-specific r2 thresholds
    @param target desired final per-bin average r2
   */
  void compute_thresholds(const double &target);
  /*!
    \brief report computed bin r2 thresholds to file
    @param out open write stream to which to report this information
   */
  void report_thresholds(std::ostream &out);
  /*!
    \brief report variants passing threshold
    @param out output stream for data reporting

    this function assumes variant IDs have been stored during first
    pass, so all that needs to happen is a pass through the bins
  */
  void report_passing_variants(std::ostream &out) const;
  /*!
    \brief report variants passing threshold
    @param filename name of info file
    @param out output stream for data reporting

    this function assumes variant IDs have not been stored during first
    pass, so it needs to process the info file again but this time
    simply report IDs that already pass the filters in the relevant bins
   */
  void report_passing_variants(const std::string &filename,
                               std::ostream &out) const;
  /*!
    \brief test for equality between objects of this class
    @param obj object to compare to *this
    \return whether all entries in the object are (approximately) equal
   */
  bool operator==(const r2_bins &obj) const;
  /*!
    \brief test for inequality between objects of this class
    @param obj object to compare to *this
    \return whether any entry in this object is not equal to the corresponding
    one
   */
  bool operator!=(const r2_bins &obj) const;
  /*!
    \brief get vector of r2_bin objects
    \return vector of r2_bin objects
   */
  const std::vector<r2_bin> &get_bins() const { return _bins; }
  /*!
    \brief get mapping structure for bin lower bounds
    \return mapping structure for bin lower bounds
   */
  const std::map<double, unsigned> &get_bin_lower_bounds() const {
    return _bin_lower_bounds;
  }
  /*!
    \brief get mapping structure for bin upper bounds
    \return mapping structure for bin upper bounds
  */
  const std::map<double, unsigned> &get_bin_upper_bounds() const {
    return _bin_upper_bounds;
  }
  friend class r2_bins_test;

 protected:
  std::vector<r2_bin> _bins;                     //!< MAF bins for aggregation
  std::map<double, unsigned> _bin_lower_bounds;  //!< MAF lower bound lookup
  std::map<double, unsigned> _bin_upper_bounds;  //!< MAF upper bound lookup
};
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_R2_BINS_H_
