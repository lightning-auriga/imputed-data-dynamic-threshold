/*!
  \file r2_bins.cc
  \brief implementation of r2_bin and r2_bins aggregator classes
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include "imputed-data-dynamic-threshold/r2_bins.h"

void imputed_data_dynamic_threshold::r2_bin::add_value(const std::string &id,
                                                       const float &val) {
  _data.push_back(std::pair<std::string, float>(id, val));
  _total += val;
  ++_total_count;
  ++_filtered_count;
}

void imputed_data_dynamic_threshold::r2_bin::compute_threshold(
    const double &target) {
  std::sort(_data.begin(), _data.end(), string_float_less_than);
  double target_sum = _total_count * target;
  while (_filtered_count && _total < target_sum) {
    _total -= _data.at(_total_count - _filtered_count).second;
    --_filtered_count;
    target_sum -= target;
  }
  if (_filtered_count < _total_count) {
    // check to be sure that reporting a particular filter doesn't imply
    // removing a few more variants that exactly tie that value
    while (fabs(_data.at(_total_count - _filtered_count).second -
                _data.at(_total_count - _filtered_count - 1).second) <
           DBL_EPSILON) {
      _total -= _data.at(_total_count - _filtered_count).second;
      --_filtered_count;
    }
  }
}

float imputed_data_dynamic_threshold::r2_bin::report_stored_threshold() const {
  if (_threshold != _threshold || _threshold >= 0.3f) return _threshold;
  throw std::logic_error(
      "report_stored_threshold called before report_threshold");
}

void imputed_data_dynamic_threshold::r2_bin::report_threshold(
    std::ostream &out) {
  // threshold defaults to 0.3; may be higher if anything was removed
  _threshold = 0.3;
  if (_filtered_count) {
    _threshold = std::max<float>(
        _threshold, _data.at(_total_count - _filtered_count).second);
  } else {
    // if everything is filtered, there is no threshold that attains the desired
    // average, alas
    _threshold = 1.0 / 0.0;
  }
  if (!(out << _bin_min << '\t' << _bin_max << '\t' << _total_count << '\t'
            << _threshold << '\t' << _filtered_count << '\t'
            << (static_cast<double>(_filtered_count) /
                static_cast<double>(_total_count))
            << std::endl))
    throw std::runtime_error("cannot write to file; out of disk space?");
}

void imputed_data_dynamic_threshold::r2_bin::report_passing_variants(
    std::ostream &out) const {
  for (unsigned i = _total_count - _filtered_count; i < _total_count; ++i) {
    out << _data.at(i).first << '\n';
  }
}

bool imputed_data_dynamic_threshold::r2_bin::operator==(
    const r2_bin &obj) const {
  if (fabs(_bin_max - obj._bin_max) > DBL_EPSILON) return false;
  if (fabs(_bin_min - obj._bin_min) > DBL_EPSILON) return false;
  if (_data.size() != obj._data.size()) return false;
  for (unsigned i = 0; i < _data.size(); ++i) {
    if (_data.at(i).first.compare(obj._data.at(i).first)) return false;
    if (fabs(_data.at(i).second - obj._data.at(i).second) > FLT_EPSILON)
      return false;
  }
  if (fabs(_total - obj._total) > DBL_EPSILON) return false;
  if (_total_count != obj._total_count) return false;
  if (_filtered_count != obj._filtered_count) return false;
  if (!(fabs(_threshold - obj._threshold) < FLT_EPSILON ||
        (_threshold != _threshold && obj._threshold != obj._threshold)))
    return false;
  return true;
}

bool imputed_data_dynamic_threshold::r2_bin::operator!=(
    const r2_bin &obj) const {
  return !(*this == obj);
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
    // handle the situation where the value is exactly the upper bound
    // and double comparison happened to sort it above
    if (fabs(maf - _bin_upper_bounds.rbegin()->first) < DBL_EPSILON) {
      return _bin_upper_bounds.rbegin()->second;
    } else {
      return _bins.size();
    }
  }
  // handle: somehow the thing that was found was inconsistent
  if (lower_finder->second != upper_finder->second) {
    throw std::logic_error(
        "bin location failed for MAF = " + std::to_string(maf) + ": " +
        std::to_string(lower_finder->second) + " " +
        std::to_string(upper_finder->second));
  }
  // handle the situation that the value is exactly the lower bound
  // and double comparison happened to sort it below
  if (fabs(maf - lower_finder->first) < DBL_EPSILON) {
    if (lower_finder == _bin_lower_bounds.begin()) {
      return _bins.size();
    } else {
      --lower_finder;
      return lower_finder->second;
    }
  }
  // otherwise, this worked evidently
  return lower_finder->second;
}

void imputed_data_dynamic_threshold::r2_bins::load_info_file(
    const std::string &filename, bool store_ids) {
  gzFile input = 0;
  char *buffer = 0;
  unsigned buffer_size = 100000, index = 0;
  std::string line = "", id = "", a0 = "", a1 = "", catcher = "", imputed = "",
              maf = "", r2 = "";
  float r2f = 0.0;
  try {
    input = gzopen(filename.c_str(), "rb");
    if (!input) {
      throw std::runtime_error("info file \"" + filename + "\" does not exist");
    }
    buffer = new char[buffer_size];
    gzgets(input, buffer, buffer_size - 1);
    while (gzgets(input, buffer, buffer_size - 1) != Z_NULL) {
      line = std::string(buffer);
      if (*line.rbegin() != '\n') {
        throw std::runtime_error(
            "load_info_file: line is longer than supported "
            "lazy buffer of 100KB; file bug report");
      }
      std::istringstream strm1(line);
      if (!(strm1 >> id >> a0 >> a1 >> catcher >> maf >> catcher >> r2 >>
            imputed)) {
        throw std::runtime_error("cannot parse info file \"" + filename +
                                 "\" line \"" + line + "\"");
      }
      if (imputed.compare("Imputed")) continue;
      r2f = from_string<float>(r2);
      if (r2f < 0.3) continue;
      index = find_maf_bin(from_string<double>(maf));
      if (index < _bins.size()) {
        _bins.at(index).add_value(store_ids ? id : "", r2f);
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
    std::ostream &out) {
  if (!(out << "bin_min\tbin_max\ttotal_variants\tthreshold\tvariants_after_"
               "filter\tproportion_passing"
            << std::endl))
    throw std::runtime_error("cannot write to file; out of disk space?");
  for (std::vector<r2_bin>::iterator iter = _bins.begin(); iter != _bins.end();
       ++iter) {
    iter->report_threshold(out);
  }
}

void imputed_data_dynamic_threshold::r2_bins::report_passing_variants(
    std::ostream &out) const {
  for (std::vector<r2_bin>::const_iterator iter = _bins.begin();
       iter != _bins.end(); ++iter) {
    iter->report_passing_variants(out);
  }
}

void imputed_data_dynamic_threshold::r2_bins::report_passing_variants(
    const std::string &filename, std::ostream &out) const {
  gzFile input = 0;
  char *buffer = 0;
  unsigned buffer_size = 100000;
  std::string line = "", id = "", catcher = "", r2 = "";
  double maf = 0.0;
  float r2f = 0.0f;
  unsigned bin_index = 0u;
  try {
    input = gzopen(filename.c_str(), "rb");
    if (!input)
      throw std::runtime_error("cannot read file \"" + filename + "\"");
    buffer = new char[buffer_size];
    gzgets(input, buffer, buffer_size - 1);
    while (gzgets(input, buffer, buffer_size - 1) != Z_NULL) {
      line = std::string(buffer);
      std::istringstream strm1(line);
      if (!(strm1 >> id >> catcher >> catcher >> catcher >> maf >> catcher >>
            r2 >> catcher))
        throw std::runtime_error("cannot parse info file \"" + filename +
                                 "\" line \"" + line + "\"");
      if (!catcher.compare("Imputed")) {
        r2f = from_string<float>(r2);
        if (r2f < 0.3f) continue;
        bin_index = find_maf_bin(maf);
        if (bin_index < _bins.size()) {
          if (r2f >= _bins.at(bin_index).report_stored_threshold()) {
            out << id << '\n';
          }
        }
      } else {
        out << id << '\n';
      }
    }
    gzclose(input);
    input = 0;
    delete[] buffer;
    buffer = 0;
  } catch (...) {
    if (input) gzclose(input);
    if (buffer) delete[] buffer;
  }
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

bool imputed_data_dynamic_threshold::r2_bins::operator!=(
    const r2_bins &obj) const {
  return !(*this == obj);
}
