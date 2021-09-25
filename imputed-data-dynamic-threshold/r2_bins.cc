/*!
  \file r2_bins.cc
  \brief implementation of r2_bin and r2_bins aggregator classes
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include "imputed-data-dynamic-threshold/r2_bins.h"

void imputed_data_dynamic_threshold::r2_bin::add_value(const float &val) {
  _data.push_back(val);
  _total += val;
  ++_total_count;
  ++_filtered_count;
}

void imputed_data_dynamic_threshold::r2_bin::compute_threshold(
    const double &target) {
  std::sort(_data.begin(), _data.end());
  double target_sum = _total_count * target;
  while (_filtered_count && _total < target_sum) {
    _total -= _data.at(_total_count - _filtered_count);
    --_filtered_count;
    target_sum -= target;
  }
  if (_filtered_count < _total_count) {
    // check to be sure that reporting a particular filter doesn't imply
    // removing a few more variants that exactly tie that value
    while (fabs(_data.at(_total_count - _filtered_count) -
                _data.at(_total_count - _filtered_count - 1)) < DBL_EPSILON) {
      _total -= _data.at(_total_count - _filtered_count);
      --_filtered_count;
    }
  }
}

void imputed_data_dynamic_threshold::r2_bin::report_threshold(
    std::ostream &out) const {
  // threshold defaults to 0.3; may be higher if anything was removed
  float threshold = 0.3;
  if (_filtered_count) {
    threshold =
        std::max<float>(threshold, _data.at(_total_count - _filtered_count));
  } else {
    // if everything is filtered, there is no threshold that attains the desired
    // average, alas
    threshold = 1.0 / 0.0;
  }
  if (!(out << _bin_min << '\t' << _bin_max << '\t' << _total_count << '\t'
            << threshold << '\t' << _filtered_count << '\t'
            << (static_cast<double>(_filtered_count) /
                static_cast<double>(_total_count))
            << std::endl))
    throw std::runtime_error("cannot write to file; out of disk space?");
}

bool imputed_data_dynamic_threshold::r2_bin::operator==(
    const r2_bin &obj) const {
  if (fabs(_bin_max - obj._bin_max) > DBL_EPSILON) return false;
  if (fabs(_bin_min - obj._bin_min) > DBL_EPSILON) return false;
  if (_data.size() != obj._data.size()) return false;
  for (unsigned i = 0; i < _data.size(); ++i) {
    if (fabs(_data.at(i) - obj._data.at(i)) > FLT_EPSILON) return false;
  }
  if (fabs(_total - obj._total) > DBL_EPSILON) return false;
  if (_total_count != obj._total_count) return false;
  if (_filtered_count != obj._filtered_count) return false;
  return true;
}

void imputed_data_dynamic_threshold::r2_bins::set_bin_boundaries(
    const std::vector<double> &boundaries) {
  if (boundaries.size() < 2) {
    throw std::runtime_error(
        "MAF bin boundaries need all limits specified, so should be of at "
        "least length 2");
  }
  for (unsigned i = 0; i < boundaries.size() - 1; ++i) {
    _bin_lower_bounds[boundaries.at(i)] = i;
    _bin_upper_bounds[boundaries.at(i + 1)] = i;
    r2_bin bin;
    bin.set_bin_bounds(boundaries.at(i), boundaries.at(i + 1));
    _bins.push_back(bin);
  }
}

unsigned imputed_data_dynamic_threshold::r2_bins::find_maf_bin(
    const double &maf) const {
  std::map<double, unsigned>::const_iterator lower_finder, upper_finder;
  lower_finder = _bin_lower_bounds.upper_bound(maf);
  upper_finder = _bin_upper_bounds.upper_bound(maf);
  // handle: value comes before the beginning of the smallest bin
  if (lower_finder == _bin_lower_bounds.begin()) {
    return _bins.size();
  }
  --lower_finder;
  // handle: value comes after the end of the largest bin
  if (upper_finder == _bin_upper_bounds.end()) {
    return _bins.size();
  }
  // handle: somehow the thing that was found was inconsistent
  if (lower_finder->second != upper_finder->second) {
    throw std::logic_error(
        "bin location failed for MAF = " + std::to_string(maf) + ": " +
        std::to_string(lower_finder->second) + " " +
        std::to_string(upper_finder->second));
  }
  // otherwise, this worked evidently
  return lower_finder->second;
}

void imputed_data_dynamic_threshold::r2_bins::load_info_file(
    const std::string &filename) {
  gzFile input = 0;
  char *buffer = 0;
  unsigned buffer_size = 100000, index = 0;
  std::string line = "", id = "", a0 = "", a1 = "", catcher = "", imputed = "",
              maf = "", r2 = "";
  float r2f = 0.0;
  try {
    input = gzopen(filename.c_str(), "rb");
    buffer = new char[buffer_size];
    gzgets(input, buffer, buffer_size - 1);
    while (gzgets(input, buffer, buffer_size - 1) != Z_NULL) {
      line = std::string(buffer);
      std::istringstream strm1(line);
      if (!(strm1 >> id >> a0 >> a1 >> maf >> catcher >> catcher >> r2 >>
            imputed)) {
        throw std::runtime_error("cannot parse info file \"" + filename +
                                 "\" line \"" + line + "\"");
      }
      if (imputed.compare("Imputed")) continue;
      r2f = from_string<float>(r2);
      if (r2f < 0.3) continue;
      index = find_maf_bin(from_string<double>(maf));
      if (index < _bins.size()) {
        _bins.at(index).add_value(r2f);
      }
    }
    gzclose(input);
    input = 0;
    delete[] buffer;
    buffer = 0;
  } catch (...) {
    if (input) {
      gzclose(input);
    }
    if (buffer) delete[] buffer;
    throw;
  }
}

void imputed_data_dynamic_threshold::r2_bins::compute_thresholds(
    const double &target) {
  for (std::vector<r2_bin>::iterator iter = _bins.begin(); iter != _bins.end();
       ++iter) {
    iter->compute_threshold(target);
  }
}

void imputed_data_dynamic_threshold::r2_bins::report_thresholds(
    const std::string &filename) const {
  std::ofstream output;
  output.open(filename.c_str());
  if (!(output << "bin_min\tbin_max\ttotal_variants\tthreshold\tvariants_after_"
                  "filter\tproportion_passing"
               << std::endl))
    throw std::runtime_error("cannot write to file; out of disk space?");
  for (std::vector<r2_bin>::const_iterator iter = _bins.begin();
       iter != _bins.end(); ++iter) {
    iter->report_threshold(output);
  }
  output.close();
}

bool imputed_data_dynamic_threshold::r2_bins::operator==(
    const r2_bins &obj) const {
  if (_bins.size() != obj._bins.size()) return false;
  for (unsigned i = 0; i < _bins.size(); ++i) {
    if (_bins.at(i) != obj._bins.at(i)) return false;
  }
  std::map<double, unsigned>::const_iterator this_iter, that_iter;
  for (this_iter = _bin_lower_bounds.begin(),
      that_iter = obj._bin_lower_bounds.begin();
       this_iter != _bin_lower_bounds.end(); ++this_iter, ++that_iter) {
    if (fabs(this_iter->first - that_iter->first) > DBL_EPSILON ||
        this_iter->second != that_iter->second)
      return false;
  }
  for (this_iter = _bin_upper_bounds.begin(),
      that_iter = obj._bin_upper_bounds.begin();
       this_iter != _bin_upper_bounds.end(); ++this_iter, ++that_iter) {
    if (fabs(this_iter->first - that_iter->first) > DBL_EPSILON ||
        this_iter->second != that_iter->second)
      return false;
  }
  return true;
}
