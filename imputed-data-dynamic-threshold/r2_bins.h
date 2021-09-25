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
  r2_bin() : _bin_min(0.0), _bin_max(0.0), _total(0.0), _total_count(0) {}
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
        _filtered_count(obj._filtered_count) {}
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
    @param val r2 from a variant fitting into this bin
   */
  void add_value(const float &val);
  /*!
    \brief compute r2 threshold required to meet a given average r2 target
    @param target desired average r2 after additional filtering is applied
   */
  void compute_threshold(const double &target);
  /*!
    \brief report r2 threshold applied and attrition due to the threshold
    @param out target output stream for writing content
   */
  void report_threshold(std::ostream &out) const;

 private:
  double _bin_min;           //!< minimum MAF in this bin, exclusive
  double _bin_max;           //!< maximum MAF in this bin, inclusive
  std::vector<float> _data;  //!< aggregated r2 data
  double _total;             //!< running sum of all loaded r2 at current filter
  unsigned _total_count;     //!< total number of loaded variants
  unsigned _filtered_count;  //!< number of variants left with current filter
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
   */
  void load_info_file(const std::string &filename);
  /*!
    \brief compute bin-specific r2 thresholds
    @param target desired final per-bin average r2
   */
  void compute_thresholds(const double &target);
  /*!
    \brief report computed bin r2 thresholds to file
    @param filename name of file to which to report this information
   */
  void report_thresholds(const std::string &filename) const;

 private:
  std::vector<r2_bin> _bins;                     //!< MAF bins for aggregation
  std::map<double, unsigned> _bin_lower_bounds;  //!< MAF lower bound lookup
  std::map<double, unsigned> _bin_upper_bounds;  //!< MAF upper bound lookup
};
}  // namespace imputed_data_dynamic_threshold

#endif  // IMPUTED_DATA_DYNAMIC_THRESHOLD_R2_BINS_H_
