/*!
  \file r2_bins.cc
  \brief implementation of r2_bin and r2_bins aggregator classes
  \copyright Released under the MIT License. Copyright
  2023 Lightning Auriga
 */

#include "imputed-data-dynamic-threshold/r2_bins.h"

namespace iddt = imputed_data_dynamic_threshold;

iddt::r2_bin::r2_bin()
    : _bin_min(0.0),
      _bin_max(0.0),
      _total(0.0),
      _total_count(0u),
      _filtered_count(0u),
      _threshold(0.0f) {}
iddt::r2_bin::r2_bin(const r2_bin &obj)
    : _bin_min(obj._bin_min),
      _bin_max(obj._bin_max),
      _data(obj._data),
      _total(obj._total),
      _total_count(obj._total_count),
      _filtered_count(obj._filtered_count),
      _threshold(obj._threshold) {}
iddt::r2_bin::~r2_bin() throw() {}

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
  if (_filtered_count < _total_count && _filtered_count) {
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
  _threshold = 0.3f;
  if (_filtered_count) {
    _threshold = std::max<float>(
        _threshold, _data.at(_total_count - _filtered_count).second);
  } else {
    // if everything is filtered, there is no threshold that attains the desired
    // average, alas
    _threshold = 1.0f / 0.0f;
  }
  if (!(out << _bin_min << '\t' << _bin_max << '\t' << _total_count << '\t'
            << _threshold << '\t'
            << (_total / static_cast<float>(_filtered_count)) << '\t'
            << _filtered_count << '\t'
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
void iddt::r2_bin::set_bin_bounds(const double &minimum,
                                  const double &maximum) {
  _bin_min = minimum;
  _bin_max = maximum;
}
const double &iddt::r2_bin::get_bin_min() const { return _bin_min; }
const double &iddt::r2_bin::get_bin_max() const { return _bin_max; }
const std::vector<std::pair<std::string, float> > &iddt::r2_bin::get_data()
    const {
  return _data;
}
const double &iddt::r2_bin::get_total() const { return _total; }
unsigned iddt::r2_bin::get_total_count() const { return _total_count; }
unsigned iddt::r2_bin::get_filtered_count() const { return _filtered_count; }

iddt::r2_bins::r2_bins() {}
iddt::r2_bins::r2_bins(const r2_bins &obj)
    : _bins(obj._bins),
      _bin_lower_bounds(obj._bin_lower_bounds),
      _bin_upper_bounds(obj._bin_upper_bounds),
      _typed_variants(obj._typed_variants) {}
iddt::r2_bins::~r2_bins() throw() {}
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
      if (*line.rbegin() != '\n' && !gzeof(input)) {
        throw std::runtime_error(
            "load_info_file: line is longer than supported "
            "lazy buffer of 100KB: \"" +
            line + "\"; file bug report");
      }
      std::istringstream strm1(line);
      if (!(strm1 >> id >> a0 >> a1 >> catcher >> maf >> catcher >> r2 >>
            imputed)) {
        throw std::runtime_error("cannot parse info file \"" + filename +
                                 "\" line \"" + line + "\"");
      }
      if (imputed.compare("Imputed")) {
        if (store_ids) {
          _typed_variants.push_back(id);
        }
        continue;
      }
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

void imputed_data_dynamic_threshold::r2_bins::load_vcf_file(
    const std::string &filename, const std::string &r2_info_field,
    const std::string &maf_info_field, const std::string &imputed_info_field,
    bool store_ids) {
  bcf_srs_t *sr = 0;
  std::string varid = "";
  float *ptr_r2 = 0, *ptr_maf = 0;
  int n_r2 = 0, n_maf = 0, n_imputed = 0;
  unsigned index = 0;
  bool is_imputed = false;
  try {
    sr = bcf_sr_init();
    hts_set_log_level(HTS_LOG_OFF);
    if (!bcf_sr_add_reader(sr, filename.c_str())) {
      throw std::runtime_error("r2_bins::load_vcf_file: " +
                               std::string(bcf_sr_strerror(sr->errnum)));
    }
    hts_set_log_level(HTS_LOG_WARNING);
    ptr_r2 = new float;
    ptr_maf = new float;
    while (bcf_sr_next_line(sr)) {
      bcf_get_info_float(bcf_sr_get_header(sr, 0), bcf_sr_get_line(sr, 0),
                         r2_info_field.c_str(), &ptr_r2, &n_r2);
      bcf_get_info_float(bcf_sr_get_header(sr, 0), bcf_sr_get_line(sr, 0),
                         maf_info_field.c_str(), &ptr_maf, &n_maf);
      is_imputed =
          bcf_get_info_flag(bcf_sr_get_header(sr, 0), bcf_sr_get_line(sr, 0),
                            imputed_info_field.c_str(), NULL, &n_imputed);
      if (store_ids) {
        bcf_unpack(bcf_sr_get_line(sr, 0), BCF_UN_STR);
        varid = std::string(bcf_sr_get_line(sr, 0)->d.id);
      }
      if (!is_imputed) {
        if (store_ids) {
          _typed_variants.push_back(varid);
        }
        continue;
      }
      if (*ptr_r2 < 0.3) continue;
      index = find_maf_bin(*ptr_maf > 0.5 ? 1.0 - *ptr_maf : *ptr_maf);
      if (index < _bins.size()) {
        _bins.at(index).add_value(store_ids ? varid : "", *ptr_r2);
      }
    }
    delete ptr_r2;
    ptr_r2 = 0;
    delete ptr_maf;
    ptr_maf = 0;
    bcf_sr_destroy(sr);
    sr = 0;
  } catch (...) {
    if (sr) {
      bcf_sr_destroy(sr);
    }
    if (ptr_r2) {
      delete ptr_r2;
    }
    if (ptr_maf) {
      delete ptr_maf;
    }
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
  if (!(out
        << "bin_min\tbin_max\ttotal_variants\tthreshold\taverage_after_filter"
           "\tvariants_after_filter\tproportion_passing"
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
  for (std::vector<std::string>::const_iterator iter = _typed_variants.begin();
       iter != _typed_variants.end(); ++iter) {
    out << *iter << '\n';
  }
}

void imputed_data_dynamic_threshold::r2_bins::report_passing_variants(
    const std::string &filename, const std::string &filter_info_files_dir,
    std::ostream &out) const {
  gzFile input = 0;
  gzFile output = 0;
  char *buffer = 0;
  unsigned buffer_size = 100000;
  std::string line = "", id = "", catcher = "", r2 = "", out_line = "";
  double maf = 0.0;
  float r2f = 0.0f;
  unsigned bin_index = 0u;

  bool emit_output = !filter_info_files_dir.empty();
  boost::filesystem::path output_dir;
  if (emit_output) {
    // output directory need not initially exist
    output_dir = boost::filesystem::path(filter_info_files_dir);
    boost::filesystem::create_directory(output_dir);
  }
  try {
    input = gzopen(filename.c_str(), "rb");
    if (!input)
      throw std::runtime_error("cannot read file \"" + filename + "\"");
    if (emit_output) {
      output_dir = output_dir / boost::filesystem::canonical(
                                    boost::filesystem::path(filename))
                                    .filename();
      output = gzopen(output_dir.string().c_str(), "wb");
      if (!output) {
        throw std::runtime_error(
            "cannot report filtered info file in second pass");
      }
      out_line =
          "SNP\tREF(0)\tALT(1)\tALT_"
          "Frq\tMAF\tAvgCall\tRsq\tGenotyped\tLooRsq\tEmpR\tEmpRsq\tDose0\tDose"
          "1"
          "\n";
      if (gzputs(output, out_line.c_str()) < 0) {
        throw std::runtime_error("cannot write to output info file, disk full");
      }
    }
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
            if (output && gzputs(output, line.c_str()) < 0) {
              throw std::runtime_error(
                  "cannot write to output info file, disk full");
            }
          }
        }
      } else {
        out << id << '\n';
        if (output && gzputs(output, line.c_str()) < 0) {
          throw std::runtime_error(
              "cannot write to output info file, disk full");
        }
      }
    }
    gzclose(input);
    input = 0;
    if (output) {
      gzclose(output);
      output = 0;
    }
    delete[] buffer;
    buffer = 0;
  } catch (...) {
    if (input) gzclose(input);
    if (output) gzclose(output);
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

const std::vector<iddt::r2_bin> &iddt::r2_bins::get_bins() const {
  return _bins;
}
std::vector<iddt::r2_bin> &iddt::r2_bins::get_bins() { return _bins; }
const std::map<double, unsigned> &iddt::r2_bins::get_bin_lower_bounds() const {
  return _bin_lower_bounds;
}
std::map<double, unsigned> &iddt::r2_bins::get_bin_lower_bounds() {
  return _bin_lower_bounds;
}
const std::map<double, unsigned> &iddt::r2_bins::get_bin_upper_bounds() const {
  return _bin_upper_bounds;
}
std::map<double, unsigned> &iddt::r2_bins::get_bin_upper_bounds() {
  return _bin_upper_bounds;
}
const std::vector<std::string> &iddt::r2_bins::get_typed_variants() const {
  return _typed_variants;
}
std::vector<std::string> &iddt::r2_bins::get_typed_variants() {
  return _typed_variants;
}
void iddt::r2_bins::set_bin_data(
    const std::vector<iddt::r2_bin> &bins,
    const std::map<double, unsigned> &bin_lower_bounds,
    const std::map<double, unsigned> &bin_upper_bounds) {
  _bins = bins;
  _bin_lower_bounds = bin_lower_bounds;
  _bin_upper_bounds = bin_upper_bounds;
}
void iddt::r2_bins::add_typed_variant(const std::string &str) {
  _typed_variants.push_back(str);
}
