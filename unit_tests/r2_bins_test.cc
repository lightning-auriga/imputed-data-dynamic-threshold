/*!
  \file r2_bins_test.cc
  \brief implementations for r2_bins class
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#include "unit_tests/r2_bins_test.h"

namespace iddt = imputed_data_dynamic_threshold;

r2BinsTest::r2BinsTest() : _tmp_dir(boost::filesystem::unique_path().native()) {
  boost::filesystem::create_directory(_tmp_dir);
}

r2BinsTest::~r2BinsTest() throw() {
  if (boost::filesystem::exists(_tmp_dir)) {
    boost::filesystem::remove_all(_tmp_dir);
  }
}

TEST_F(r2BinsTest, r2BinsDefaultConstructor) {
  iddt::r2_bins a;
  std::vector<iddt::r2_bin> vec;
  std::map<double, unsigned> m;
  std::vector<std::string> svec;
  EXPECT_EQ(a.get_bins(), vec);
  EXPECT_EQ(a.get_bin_lower_bounds(), m);
  EXPECT_EQ(a.get_bin_upper_bounds(), m);
  EXPECT_EQ(a.get_typed_variants(), svec);
}

TEST_F(r2BinsTest, r2BinsCopyConstructor) {
  iddt::r2_bins a;
  std::map<double, unsigned> bin_lower_bounds, bin_upper_bounds;
  bin_lower_bounds[0.1] = 2;
  bin_upper_bounds[0.2] = 40;
  a.set_bin_data(std::vector<iddt::r2_bin>(), bin_lower_bounds,
                 bin_upper_bounds);
  a.add_typed_variant("hello world");
  iddt::r2_bins b(a);
  EXPECT_EQ(a.get_bins(), b.get_bins());
  EXPECT_EQ(a.get_bin_lower_bounds(), b.get_bin_lower_bounds());
  EXPECT_EQ(a.get_bin_upper_bounds(), b.get_bin_upper_bounds());
  EXPECT_EQ(a.get_typed_variants(), b.get_typed_variants());
}

TEST_F(r2BinsTest, r2BinsSetBinBoundaries) {
  iddt::r2_bins a, b;
  std::vector<double> bounds;
  bounds.push_back(0.001);
  bounds.push_back(0.03);
  bounds.push_back(0.5);
  a.set_bin_boundaries(bounds);
  std::map<double, unsigned> bin_lower_bounds, bin_upper_bounds;
  bin_lower_bounds[0.001] = 0;
  bin_lower_bounds[0.03] = 1;
  bin_upper_bounds[0.03] = 0;
  bin_upper_bounds[0.5] = 1;
  std::vector<iddt::r2_bin> bins;
  iddt::r2_bin bin1, bin2;
  bin1.set_bin_bounds(0.001, 0.03);
  bin2.set_bin_bounds(0.03, 0.5);
  bins.push_back(bin1);
  bins.push_back(bin2);
  b.set_bin_data(bins, bin_lower_bounds, bin_upper_bounds);
  EXPECT_EQ(a, b);
}

TEST_F(r2BinsTest, r2BinsFindMafBin) {
  iddt::r2_bins a;
  std::map<double, unsigned> bin_lower_bounds, bin_upper_bounds;
  bin_lower_bounds[0.1] = 0;
  bin_lower_bounds[0.2] = 1;
  bin_upper_bounds[0.2] = 0;
  bin_upper_bounds[0.5] = 1;
  std::vector<iddt::r2_bin> bins;
  iddt::r2_bin bin;
  bin.set_bin_bounds(0.1, 0.2);
  bins.push_back(bin);
  bin.set_bin_bounds(0.2, 0.5);
  bins.push_back(bin);
  a.set_bin_data(bins, bin_lower_bounds, bin_upper_bounds);
  EXPECT_EQ(a.find_maf_bin(0.001), 2u);
  EXPECT_EQ(a.find_maf_bin(0.1), 2u);
  EXPECT_EQ(a.find_maf_bin(0.15), 0u);
  EXPECT_EQ(a.find_maf_bin(0.2), 0u);
  EXPECT_EQ(a.find_maf_bin(0.25), 1u);
  EXPECT_EQ(a.find_maf_bin(0.5), 1u);
  EXPECT_EQ(a.find_maf_bin(0.55), 2u);
}

TEST_F(r2BinsTest, r2BinsLoadInfoFiles) {
  iddt::r2_bins a, b, c, d;
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
    std::string line =
        "SNP\tREF(0)\tALT(1)\tALT_Frq\tMAF\tAvgCall\tRsq\tGenotyped\t"
        "LooRsq\tEmpR\tEmpRsq\tDose0\tDose1\n"
        "chr1:1:A:T\tA\tT\t0.1\t0.1\t0.1\t0.44231\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:2:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:3:G:A\tG\tA\t0.02\t0.02\t0.02\t0.99991\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:4:T:A\tT\tA\t0.4\t0.4\t0.4\t0.34113\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:5:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:6:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n"
        "chr1:7:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    gzclose(output);
    output = NULL;
    output = gzopen(bad_file.string().c_str(), "wb");
    if (!output) {
      throw std::runtime_error(
          "r2_bins test_load_info_file: cannot write bad test file");
    }
    line =
        "SNP\tREF(0)\tALT(1)\tALT_Frq\tMAF\tAvgCall\tRsq\tGenotyped\tLooRsq\t"
        "EmpR\tEmpRsq\tDose0\tDose1\n";
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
  b.get_bins().at(1).add_value("chr1:1:A:T", 0.44231f);
  b.get_bins().at(0).add_value("chr1:3:G:A", 0.99991f);
  b.get_bins().at(1).add_value("chr1:4:T:A", 0.34113f);
  b.get_typed_variants().push_back("chr1:6:A:C");
  b.get_typed_variants().push_back("chr1:7:A:C");
  EXPECT_TRUE(a == b);
  EXPECT_THROW(a.load_info_file(bad_file.string().c_str(), true),
               std::runtime_error);
  c.set_bin_boundaries(bounds);
  c.load_info_file(good_file.string().c_str(), false);
  d.set_bin_boundaries(bounds);
  d.get_bins().at(1).add_value("", 0.44231f);
  d.get_bins().at(0).add_value("", 0.99991f);
  d.get_bins().at(1).add_value("", 0.34113f);
  EXPECT_TRUE(c == d);
  EXPECT_FALSE(a == c);
}

TEST_F(r2BinsTest, r2BinsComputeThresholds) {
  iddt::r2_bins a, b;
  iddt::r2_bin bin1, bin2;
  a.compute_thresholds(0.65);
  EXPECT_EQ(a, b);
  bin1.set_bin_bounds(0.1, 0.2);
  bin1.add_value("a", 0.5);
  bin1.add_value("b", 0.6);
  bin1.add_value("c", 0.7);
  a.get_bins().push_back(bin1);
  bin1.compute_threshold(0.65);
  b.get_bins().push_back(bin1);
  bin2.set_bin_bounds(0.2, 0.5);
  bin2.add_value("d", 0.5);
  bin2.add_value("e", 0.6);
  bin2.add_value("f", 0.7);
  bin2.add_value("g", 0.8);
  a.get_bins().push_back(bin2);
  bin2.compute_threshold(0.65);
  b.get_bins().push_back(bin2);
  std::vector<iddt::r2_bin> a_bins = a.get_bins(), b_bins = b.get_bins();
  std::map<double, unsigned> a_bin_lower_bounds = a.get_bin_lower_bounds();
  std::map<double, unsigned> b_bin_lower_bounds = b.get_bin_lower_bounds();
  std::map<double, unsigned> a_bin_upper_bounds = a.get_bin_upper_bounds();
  std::map<double, unsigned> b_bin_upper_bounds = b.get_bin_upper_bounds();

  a_bin_lower_bounds[0.1] = 0;
  a_bin_lower_bounds[0.2] = 1;
  a_bin_upper_bounds[0.2] = 0;
  a_bin_upper_bounds[0.5] = 1;
  b_bin_lower_bounds[0.1] = 0;
  b_bin_lower_bounds[0.2] = 1;
  b_bin_upper_bounds[0.2] = 0;
  b_bin_upper_bounds[0.5] = 1;
  a.set_bin_data(a_bins, a_bin_lower_bounds, a_bin_upper_bounds);
  b.set_bin_data(b_bins, b_bin_lower_bounds, b_bin_upper_bounds);
  a.compute_thresholds(0.65);
  EXPECT_EQ(a, b);
}

TEST_F(r2BinsTest, r2BinsReportThresholds) {
  iddt::r2_bins a;
  iddt::r2_bin bin1, bin2;
  std::ostringstream o1, o2;
  a.report_thresholds(o1);
  EXPECT_EQ(std::string("bin_min\tbin_max\ttotal_variants\t"
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
  a.get_bins().push_back(bin1);
  bin2.set_bin_bounds(0.2, 0.5);
  bin2.add_value("e", 0.5);
  bin2.add_value("f", 0.6);
  bin2.add_value("g", 0.7);
  bin2.add_value("h", 0.8);
  bin2.add_value("i", 0.8);
  bin2.compute_threshold(0.66);
  a.get_bins().push_back(bin2);
  a.get_bin_lower_bounds()[0.1] = 0;
  a.get_bin_lower_bounds()[0.2] = 1;
  a.get_bin_upper_bounds()[0.2] = 0;
  a.get_bin_upper_bounds()[0.5] = 1;
  a.report_thresholds(o2);
  EXPECT_EQ(
      std::string(
          "bin_min\tbin_max\ttotal_variants\tthreshold\t"
          "average_after_filter\tvariants_after_filter\tproportion_passing\n"
          "0.1\t0.2\t4\t0.6\t0.7\t3\t0.75\n0.2\t0.5\t5\t0.5\t0.68\t5\t1\n"),
      o2.str());
}

TEST_F(r2BinsTest, r2BinsReportPassingVariantsFromRam) {
  iddt::r2_bins a;
  iddt::r2_bin bin1, bin2;
  std::ostringstream o1, o2;
  bin1.set_bin_bounds(0.1, 0.2);
  bin1.add_value("a", 0.5);
  bin1.add_value("b", 0.6);
  bin1.add_value("c", 0.7);
  bin1.add_value("d", 0.8);
  bin1.compute_threshold(0.66);
  a.get_bins().push_back(bin1);
  bin2.set_bin_bounds(0.2, 0.5);
  bin2.add_value("e", 0.5);
  bin2.add_value("f", 0.6);
  bin2.add_value("g", 0.7);
  bin2.add_value("h", 0.8);
  bin2.add_value("i", 0.8);
  bin2.compute_threshold(0.66);
  a.get_bins().push_back(bin2);
  a.get_bin_lower_bounds()[0.1] = 0;
  a.get_bin_lower_bounds()[0.2] = 1;
  a.get_bin_upper_bounds()[0.2] = 0;
  a.get_bin_upper_bounds()[0.5] = 1;
  a.report_thresholds(o1);
  a.report_passing_variants(o2);
  EXPECT_EQ(std::string("b\nc\nd\ne\nf\ng\nh\ni\n"), o2.str());
}

TEST_F(r2BinsTest, r2BinsReportPassingVariantsFromFile) {
  iddt::r2_bins a;
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
    std::string line =
        "SNP\tREF(0)\tALT(1)\tALT_Frq\tMAF\tAvgCall\tRsq\tGenotyped\t"
        "LooRsq\tEmpR\tEmpRsq\tDose0\tDose1\n"
        "chr1:1:A:T\tA\tT\t0.1\t0.1\t0.1\t0.44231\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:2:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:3:G:A\tG\tA\t0.02\t0.02\t0.02\t0.99991\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:4:T:A\tT\tA\t0.4\t0.4\t0.4\t0.34113\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:5:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n"
        "chr1:6:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n"
        "chr1:7:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n";
    gzputs(output, line.c_str());
    gzclose(output);
    output = NULL;
  } catch (...) {
    if (output) gzclose(output);
    throw;
  }
  a.load_info_file(good_file.string().c_str(), false);
  a.compute_thresholds(0.42f);
  std::ostringstream o1, o2;
  a.report_thresholds(o1);
  a.report_passing_variants(good_file.string().c_str(), "", o2);
  EXPECT_EQ(std::string("chr1:1:A:T\nchr1:3:G:A\nchr1:6:A:C\nchr1:7:A:C\n"),
            o2.str());
}

TEST_F(r2BinsTest, r2BinsEqualityOperator) {
  iddt::r2_bins a, b;
  EXPECT_EQ(a, b);
  a.get_bins().push_back(iddt::r2_bin());
  EXPECT_FALSE(a == b);
  b.get_bins().push_back(iddt::r2_bin());
  EXPECT_EQ(a, b);
  a.get_bin_lower_bounds()[0.1] = 0;
  EXPECT_FALSE(a == b);
  b.get_bin_lower_bounds()[0.1] = 0;
  EXPECT_EQ(a, b);
  a.get_bin_upper_bounds()[0.2] = 0;
  EXPECT_FALSE(a == b);
  b.get_bin_upper_bounds()[0.2] = 0;
  EXPECT_EQ(a, b);
}

TEST_F(r2BinsTest, r2BinsInequalityOperator) {
  iddt::r2_bins a, b;
  EXPECT_FALSE(a != b);
  a.get_bins().push_back(iddt::r2_bin());
  EXPECT_NE(a, b);
  b.get_bins().push_back(iddt::r2_bin());
  EXPECT_FALSE(a != b);
  a.get_bin_lower_bounds()[0.1] = 0;
  EXPECT_NE(a, b);
  b.get_bin_lower_bounds()[0.1] = 0;
  EXPECT_FALSE(a != b);
  a.get_bin_upper_bounds()[0.2] = 0;
  EXPECT_NE(a, b);
  b.get_bin_upper_bounds()[0.2] = 0;
  EXPECT_FALSE(a != b);
}
