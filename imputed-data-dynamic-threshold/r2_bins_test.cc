/*!
  \file r2_bins_test.cc
  \brief implementations for r2_bins class
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include "imputed-data-dynamic-threshold/r2_bins_test.h"

void imputed_data_dynamic_threshold::r2_bins_test::setUp() {
  // allocate temporary directory for test files
  unsigned buffer_size =
      std::filesystem::temp_directory_path().string().size() + 20;
  _tmp_dir = new char[buffer_size];
  strncpy(_tmp_dir,
          (std::filesystem::temp_directory_path().string() + "/iddtRBTXXXXXX")
              .c_str(),
          buffer_size);
  char *res = mkdtemp(_tmp_dir);
  if (!res) {
    throw std::runtime_error("r2_bins_test mkdtemp failed");
  }
}

void imputed_data_dynamic_threshold::r2_bins_test::tearDown() {
  if (_tmp_dir) {
    std::filesystem::remove_all(std::filesystem::path(_tmp_dir));
    delete[] _tmp_dir;
  }
}

void imputed_data_dynamic_threshold::r2_bins_test::test_default_constructor() {
  r2_bins a;
  std::vector<r2_bin> vec;
  std::map<double, unsigned> m;
  std::vector<std::string> svec;
  CPPUNIT_ASSERT_MESSAGE("r2_bins default constructor: data", a._bins == vec);
  CPPUNIT_ASSERT_MESSAGE("r2_bins default constructor: _bin_lower_bounds",
                         a._bin_lower_bounds == m);
  CPPUNIT_ASSERT_MESSAGE("r2_bins default constructor: _bin_upper_bounds",
                         a._bin_upper_bounds == m);
  CPPUNIT_ASSERT_MESSAGE("r2_bins default constructor: _typed_variants",
                         a._typed_variants == svec);
}

void imputed_data_dynamic_threshold::r2_bins_test::test_copy_constructor() {
  r2_bins a;
  a._bins.push_back(r2_bin());
  a._bin_lower_bounds[0.1] = 2;
  a._bin_upper_bounds[0.2] = 40;
  a._typed_variants.push_back("hello world");
  r2_bins b(a);
  CPPUNIT_ASSERT_MESSAGE("r2_bins copy constructor: _bins",
                         a.get_bins() == b.get_bins());
  CPPUNIT_ASSERT_MESSAGE("r2_bins copy constructor: _bin_lower_bounds",
                         a.get_bin_lower_bounds() == b.get_bin_lower_bounds());
  CPPUNIT_ASSERT_MESSAGE("r2_bins copy constructor: _bin_upper_bounds",
                         a.get_bin_upper_bounds() == b.get_bin_upper_bounds());
  CPPUNIT_ASSERT_MESSAGE("r2_bins copy constructor: _typed_variants",
                         a._typed_variants == b._typed_variants);
}

void imputed_data_dynamic_threshold::r2_bins_test::test_set_bin_boundaries() {
  r2_bins a, b;
  std::vector<double> bounds;
  bounds.push_back(0.001);
  bounds.push_back(0.03);
  bounds.push_back(0.5);
  a.set_bin_boundaries(bounds);
  b._bin_lower_bounds[0.001] = 0;
  b._bin_lower_bounds[0.03] = 1;
  b._bin_upper_bounds[0.03] = 0;
  b._bin_upper_bounds[0.5] = 1;
  r2_bin bin;
  bin.set_bin_bounds(0.001, 0.03);
  b._bins.push_back(bin);
  bin.set_bin_bounds(0.03, 0.5);
  b._bins.push_back(bin);
  CPPUNIT_ASSERT_MESSAGE("r2_bins set_bin_boundaries", a == b);
}

void imputed_data_dynamic_threshold::r2_bins_test::test_find_maf_bin() {
  r2_bins a;
  a._bin_lower_bounds[0.1] = 0;
  a._bin_lower_bounds[0.2] = 1;
  a._bin_upper_bounds[0.2] = 0;
  a._bin_upper_bounds[0.5] = 1;
  r2_bin bin;
  bin.set_bin_bounds(0.1, 0.2);
  a._bins.push_back(bin);
  bin.set_bin_bounds(0.2, 0.5);
  a._bins.push_back(bin);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bins find_maf_bin: below smallest bin", 2u,
                               a.find_maf_bin(0.001));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bins find_maf_bin: smallest bin lower bound",
                               2u, a.find_maf_bin(0.1));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bins find_maf_bin: smallest bin", 0u,
                               a.find_maf_bin(0.15));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bins find_maf_bin: smallest bin upper bound",
                               0u, a.find_maf_bin(0.2));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bins find_maf_bin: largest bin", 1u,
                               a.find_maf_bin(0.25));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bins find_maf_bin: largest bin upper bound",
                               1u, a.find_maf_bin(0.5));
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bins find_maf_bin: above largest bin", 2u,
                               a.find_maf_bin(0.55));
}

void imputed_data_dynamic_threshold::r2_bins_test::test_load_info_file() {
  r2_bins a, b, c, d;
  std::vector<double> bounds;
  bounds.push_back(0.001);
  bounds.push_back(0.03);
  bounds.push_back(0.5);
  a.set_bin_boundaries(bounds);
  // create a test info file for load test
  boost::filesystem::path tmpdir =
      boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path good_file = tmpdir / "r2_bins_test_example.info.gz";
  boost::filesystem::path bad_file = tmpdir / "r2_bins_line_too_long.info.gz";
  gzFile output = NULL;
  try {
    output = gzopen(good_file.string().c_str(), "wb");
    if (!output) {
      throw std::runtime_error(
          "r2_bins test_load_info_file: cannot write test file");
    }
    std::string line = "";
    line =
        "SNP\tREF(0)\tALT(1)\tALT_"
        "Frq\tMAF\tAvgCall\tRsq\tGenotyped\tLooRsq\tEmpR\tEmpRsq\tDose0\tDose1"
        "\n";
    gzputs(output, line.c_str());
    line = "chr1:1:A:T\tA\tT\t0.1\t0.1\t0.1\t0.44231\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:2:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line =
        "chr1:3:G:A\tG\tA\t0.02\t0.02\t0.02\t0.99991\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:4:T:A\tT\tA\t0.4\t0.4\t0.4\t0.34113\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:5:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:6:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:7:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    gzclose(output);
    output = NULL;
    output = gzopen(bad_file.string().c_str(), "wb");
    if (!output) {
      throw std::runtime_error(
          "r2_bins test_load_info_file: cannot write bad test file");
    }
    line =
        "SNP\tREF(0)\tALT(1)\tALT_"
        "Frq\tMAF\tAvgCall\tRsq\tGenotyped\tLooRsq\tEmpR\tEmpRsq\tDose0\tDose1"
        "\n";
    gzputs(output, line.c_str());
    line = std::string(100010, 'c');
    gzputs(output, line.c_str());
    gzclose(output);
    output = NULL;
  } catch (...) {
    if (output) gzclose(output);
    throw;
  }

  a.load_info_file(good_file.string().c_str(), true);
  b.set_bin_boundaries(bounds);
  b._bins.at(1).add_value("chr1:1:A:T", 0.44231f);
  b._bins.at(0).add_value("chr1:3:G:A", 0.99991f);
  b._bins.at(1).add_value("chr1:4:T:A", 0.34113f);
  b._typed_variants.push_back("chr1:6:A:C");
  b._typed_variants.push_back("chr1:7:A:C");
  CPPUNIT_ASSERT_MESSAGE("r2_bins load info file, store IDs", a == b);
  CPPUNIT_ASSERT_THROW_MESSAGE(
      "r2_bins info line exceeds 100KB",
      a.load_info_file(bad_file.string().c_str(), true), std::runtime_error);
  c.set_bin_boundaries(bounds);
  c.load_info_file(good_file.string().c_str(), false);
  d.set_bin_boundaries(bounds);
  d._bins.at(1).add_value("", 0.44231f);
  d._bins.at(0).add_value("", 0.99991f);
  d._bins.at(1).add_value("", 0.34113f);
  CPPUNIT_ASSERT_MESSAGE("r2_bins load info file, ignore IDs", c == d);
  CPPUNIT_ASSERT_MESSAGE("r2_bins load info file, detect ID storage difference",
                         !(a == c));
}

void imputed_data_dynamic_threshold::r2_bins_test::test_compute_thresholds() {
  r2_bins a, b;
  r2_bin bin1, bin2;
  a.compute_thresholds(0.65);
  CPPUNIT_ASSERT_MESSAGE("r2_bins compute_thresholds: without bins", a == b);
  bin1.set_bin_bounds(0.1, 0.2);
  bin1.add_value("a", 0.5);
  bin1.add_value("b", 0.6);
  bin1.add_value("c", 0.7);
  a._bins.push_back(bin1);
  bin1.compute_threshold(0.65);
  b._bins.push_back(bin1);
  bin2.set_bin_bounds(0.2, 0.5);
  bin2.add_value("d", 0.5);
  bin2.add_value("e", 0.6);
  bin2.add_value("f", 0.7);
  bin2.add_value("g", 0.8);
  a._bins.push_back(bin2);
  bin2.compute_threshold(0.65);
  b._bins.push_back(bin2);
  a._bin_lower_bounds[0.1] = 0;
  a._bin_lower_bounds[0.2] = 1;
  a._bin_upper_bounds[0.2] = 0;
  a._bin_upper_bounds[0.5] = 1;
  b._bin_lower_bounds[0.1] = 0;
  b._bin_lower_bounds[0.2] = 1;
  b._bin_upper_bounds[0.2] = 0;
  b._bin_upper_bounds[0.5] = 1;
  a.compute_thresholds(0.65);
  CPPUNIT_ASSERT_MESSAGE("r2_bins compute_thresholds, with bins", a == b);
}

void imputed_data_dynamic_threshold::r2_bins_test::test_report_thresholds() {
  r2_bins a;
  r2_bin bin1, bin2;
  std::ostringstream o1, o2;
  a.report_thresholds(o1);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("r2_bins report_thresholds: without bins",
                               std::string("bin_min\tbin_max\ttotal_variants\t"
                                           "threshold\taverage_after_filter\t"
                                           "variants_after_filter\t"
                                           "proportion_passing\n"),
                               o1.str());
  bin1.set_bin_bounds(0.1, 0.2);
  bin1.add_value("a", 0.5);
  bin1.add_value("b", 0.6);
  bin1.add_value("c", 0.7);
  bin1.add_value("d", 0.8);
  bin1.compute_threshold(0.66);
  a._bins.push_back(bin1);
  bin2.set_bin_bounds(0.2, 0.5);
  bin2.add_value("e", 0.5);
  bin2.add_value("f", 0.6);
  bin2.add_value("g", 0.7);
  bin2.add_value("h", 0.8);
  bin2.add_value("i", 0.8);
  bin2.compute_threshold(0.66);
  a._bins.push_back(bin2);
  a._bin_lower_bounds[0.1] = 0;
  a._bin_lower_bounds[0.2] = 1;
  a._bin_upper_bounds[0.2] = 0;
  a._bin_upper_bounds[0.5] = 1;
  a.report_thresholds(o2);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "r2_bins report_thresholds: with bins",
      std::string(
          "bin_min\tbin_max\ttotal_variants\tthreshold\t"
          "average_after_filter\tvariants_after_filter\tproportion_passing\n"
          "0.1\t0.2\t4\t0.6\t0.7\t3\t0.75\n0.2\t0.5\t5\t0.5\t0.68\t5\t1\n"),
      o2.str());
}

void imputed_data_dynamic_threshold::r2_bins_test::
    test_report_passing_variants_from_ram() {
  r2_bins a;
  r2_bin bin1, bin2;
  std::ostringstream o1, o2;
  bin1.set_bin_bounds(0.1, 0.2);
  bin1.add_value("a", 0.5);
  bin1.add_value("b", 0.6);
  bin1.add_value("c", 0.7);
  bin1.add_value("d", 0.8);
  bin1.compute_threshold(0.66);
  a._bins.push_back(bin1);
  bin2.set_bin_bounds(0.2, 0.5);
  bin2.add_value("e", 0.5);
  bin2.add_value("f", 0.6);
  bin2.add_value("g", 0.7);
  bin2.add_value("h", 0.8);
  bin2.add_value("i", 0.8);
  bin2.compute_threshold(0.66);
  a._bins.push_back(bin2);
  a._bin_lower_bounds[0.1] = 0;
  a._bin_lower_bounds[0.2] = 1;
  a._bin_upper_bounds[0.2] = 0;
  a._bin_upper_bounds[0.5] = 1;
  a.report_thresholds(o1);
  a.report_passing_variants(o2);
  CPPUNIT_ASSERT_EQUAL_MESSAGE("report_passing_variants_from_raw",
                               std::string("b\nc\nd\ne\nf\ng\nh\ni\n"),
                               o2.str());
}

void imputed_data_dynamic_threshold::r2_bins_test::
    test_report_passing_variants_from_file() {
  r2_bins a;
  std::vector<double> bounds;
  bounds.push_back(0.001);
  bounds.push_back(0.03);
  bounds.push_back(0.5);
  a.set_bin_boundaries(bounds);
  // create a test info file for load test
  boost::filesystem::path tmpdir =
      boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path good_file = tmpdir / "r2_bins_test_example.info.gz";
  gzFile output = NULL;
  try {
    output = gzopen(good_file.string().c_str(), "wb");
    if (!output) {
      throw std::runtime_error(
          "r2_bins test_report_passing_variants_from_file: cannot write test "
          "file");
    }
    std::string line = "";
    line =
        "SNP\tREF(0)\tALT(1)\tALT_"
        "Frq\tMAF\tAvgCall\tRsq\tGenotyped\tLooRsq\tEmpR\tEmpRsq\tDose0\tDose1"
        "\n";
    gzputs(output, line.c_str());
    line = "chr1:1:A:T\tA\tT\t0.1\t0.1\t0.1\t0.44231\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:2:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line =
        "chr1:3:G:A\tG\tA\t0.02\t0.02\t0.02\t0.99991\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:4:T:A\tT\tA\t0.4\t0.4\t0.4\t0.34113\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:5:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:6:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    line = "chr1:7:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    gzclose(output);
    output = NULL;
  } catch (...) {
    if (output) gzclose(output);
    throw;
  }
  a.load_info_file(good_file.string().c_str(), false);
  a.compute_thresholds(0.42f);
  std::ostringstream o0, o1, o2;
  CPPUNIT_ASSERT_THROW_MESSAGE(
      "report_passing_variants from file: "
      "enforces call order",
      a.report_passing_variants(good_file.string().c_str(), o0),
      std::logic_error);
  a.report_thresholds(o1);
  a.report_passing_variants("tests/r2_bins_test_example.info.gz", o2);
  CPPUNIT_ASSERT_EQUAL_MESSAGE(
      "report_passing_variants from file",
      std::string("chr1:3:G:A\nchr1:1:A:T\nchr1:6:A:C\nchr1:7:A:C\n"),
      o2.str());
}

void imputed_data_dynamic_threshold::r2_bins_test::test_equality_operator() {
  r2_bins a, b;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: default", a == b);
  a._bins.push_back(r2_bin());
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: different length _bins", !(a == b));
  b._bins.push_back(r2_bin());
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: single bin", a == b);
  a._bin_lower_bounds[0.1] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: uneven lower bounds", !(a == b));
  b._bin_lower_bounds[0.1] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: single lower bound", a == b);
  a._bin_upper_bounds[0.2] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: uneven upper bound", !(a == b));
  b._bin_upper_bounds[0.2] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: single upper bound", a == b);
}

void imputed_data_dynamic_threshold::r2_bins_test::test_inequality_operator() {
  r2_bins a, b;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: default", !(a != b));
  a._bins.push_back(r2_bin());
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: different length _bins", a != b);
  b._bins.push_back(r2_bin());
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: single bin", !(a != b));
  a._bin_lower_bounds[0.1] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: uneven lower bounds", a != b);
  b._bin_lower_bounds[0.1] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: single lower bound", !(a != b));
  a._bin_upper_bounds[0.2] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: uneven upper bound", a != b);
  b._bin_upper_bounds[0.2] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins equality: single upper bound", !(a != b));
}

void imputed_data_dynamic_threshold::r2_bins_test::test_get_bins() {
  r2_bins a;
  std::vector<r2_bin> vec;
  CPPUNIT_ASSERT_MESSAGE("r2_bins get_bins empty vector", a.get_bins() == vec);
  a._bins.push_back(r2_bin());
  vec.push_back(r2_bin());
  CPPUNIT_ASSERT_MESSAGE("r2_bins get_bins single entry vector",
                         a.get_bins() == vec);
}

void imputed_data_dynamic_threshold::r2_bins_test::test_get_bin_lower_bounds() {
  r2_bins a;
  std::map<double, unsigned> m;
  CPPUNIT_ASSERT_MESSAGE("r2_bins get_bin_lower_bounds empty map",
                         a.get_bin_lower_bounds() == m);
  a._bin_lower_bounds[0.1] = 0;
  m[0.1] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins get_bin_lower_bounds single entry map",
                         a.get_bin_lower_bounds() == m);
}

void imputed_data_dynamic_threshold::r2_bins_test::test_get_bin_upper_bounds() {
  r2_bins a;
  std::map<double, unsigned> m;
  CPPUNIT_ASSERT_MESSAGE("r2_bins get_bin_upper_bounds empty map",
                         a.get_bin_upper_bounds() == m);
  a._bin_upper_bounds[0.1] = 0;
  m[0.1] = 0;
  CPPUNIT_ASSERT_MESSAGE("r2_bins get_bin_upper_bounds single entry map",
                         a.get_bin_upper_bounds() == m);
}

/// @cond registration
CPPUNIT_TEST_SUITE_REGISTRATION(imputed_data_dynamic_threshold::r2_bins_test);
/// @endcond
