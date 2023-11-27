/*!
 \file cargs_test.cc
 \brief test of command line interface.
 \copyright Released under the MIT License.
 Copyright 2023 Lightning Auriga
 */

#include "imputed-data-dynamic-threshold/cargs.h"

#include "gtest/gtest.h"
#include "unit_tests/cargs_test.h"

namespace iddt = imputed_data_dynamic_threshold;

cargsTest::cargsTest()
    : testing::Test(),
      _argv1(NULL),
      _argv2(NULL),
      _argv3(NULL),
      _argv4(NULL),
      _argv5(NULL),
      _argv6(NULL),
      _argv7(NULL),
      _argv8(NULL),
      _tmp_dir(boost::filesystem::unique_path().native()) {
  std::string test1 = "progname -h";
  populate(test1, &_argvec1, &_argv1);
  std::string test2 =
      "progname -i " + _tmp_dir + "/file1.gz " + _tmp_dir +
      "/file2.gz -m 0.01 0.1 -r 0.75 --baseline-r2 0.4 "
      "-s --filter-info-files targetdir -o summary.txt -l list.txt";
  populate(test2, &_argvec2, &_argv2);
  std::string test3 = "progname -v " + _tmp_dir +
                      "/file1.vcf.gz "
                      "--vcf-info-r2-tag r2 "
                      "--vcf-info-af-tag af "
                      "--vcf-info-imputed-indicator imp "
                      "-s";
  populate(test3, &_argvec3, &_argv3);
  std::string test4 = "progname -i " + _tmp_dir +
                      "/file1.gz -o summary.txt "
                      "--filter-info-files targetdir";
  populate(test4, &_argvec4, &_argv4);
  std::string test5 = "progname --version";
  populate(test5, &_argvec5, &_argv5);
  std::string test6 =
      "progname --maf-bin-boundaries -1 0.5 -i blergh.info.gz "
      "-v blergh.vcf.gz --target-average-r2 -1 --baseline-r2 -1";
  populate(test6, &_argvec6, &_argv6);
  std::string test7 =
      "progname --maf-bin-boundaries 0.01 0.6 "
      "--target-average-r2 20 --baseline-r2 55";
  populate(test7, &_argvec7, &_argv7);
  std::string test8 = "progname --maf-bin-boundaries 0.3 0.4 0.2";
  populate(test8, &_argvec8, &_argv8);
  boost::filesystem::create_directory(_tmp_dir);
}

cargsTest::~cargsTest() {
  if (_argv1) {
    delete[] _argv1;
  }
  if (_argv2) {
    delete[] _argv2;
  }
  if (_argv3) {
    delete[] _argv3;
  }
  if (_argv4) {
    delete[] _argv4;
  }
  if (_argv5) {
    delete[] _argv5;
  }
  if (_argv6) {
    delete[] _argv6;
  }
  if (_argv7) {
    delete[] _argv7;
  }
  if (_argv8) {
    delete[] _argv8;
  }
  if (boost::filesystem::exists(_tmp_dir)) {
    boost::filesystem::remove_all(_tmp_dir);
  }
}

void cargsTest::populate(const std::string &str, std::vector<std::string> *vec,
                         const char ***arr) {
  if (!vec || !arr) {
    throw std::runtime_error("populate: null pointer(s)");
  }
  vec->clear();
  std::istringstream strm1(str);
  std::string token = "";
  while (strm1 >> token) {
    vec->push_back(token);
  }
  if (*arr) {
    delete[] *arr;
    *arr = 0;
  }
  *arr = new const char *[vec->size()];
  for (unsigned i = 0; i < vec->size(); ++i) {
    (*arr)[i] = vec->at(i).c_str();
  }
}

TEST_F(cargsTest, basicInitialization) {
  iddt::cargs ap(_argvec1.size(), _argv1);
}

TEST_F(cargsTest, copyConstructor) {
  iddt::cargs ap1(_argvec1.size(), _argv1);
  iddt::cargs ap2(ap1);
}

TEST_F(cargsTest, printHelp) {
  std::ostringstream o;
  iddt::cargs ap(_argvec1.size(), _argv1);
  ap.print_help(o);
  EXPECT_EQ(o.str().find("Recognized options:"), 0UL);
}

TEST_F(cargsTest, printVersion) {
  std::ostringstream o;
  iddt::cargs ap(_argvec5.size(), _argv5);
  ap.print_version(o);
  EXPECT_EQ(o.str().find("imputed-data-dynamic-threshold"), 0UL);
}

TEST_F(cargsTest, detectHelp) {
  iddt::cargs ap1(_argvec1.size(), _argv1);
  EXPECT_TRUE(ap1.help());
  iddt::cargs ap2(_argvec2.size(), _argv2);
  EXPECT_FALSE(ap2.help());
}

TEST_F(cargsTest, detectVersion) {
  iddt::cargs ap1(_argvec5.size(), _argv5);
  EXPECT_TRUE(ap1.version());
  iddt::cargs ap2(_argvec1.size(), _argv1);
  EXPECT_FALSE(ap2.version());
}

TEST_F(cargsTest, infoModeBasicAccessors) {
  iddt::cargs ap(_argvec2.size(), _argv2);
  std::ofstream output;
  output.open((_tmp_dir + "/file1.gz").c_str());
  output.close();
  output.clear();
  output.open((_tmp_dir + "/file2.gz").c_str());
  output.close();
  EXPECT_TRUE(ap.second_pass());
  EXPECT_EQ(ap.get_filter_info_files_dir(), "targetdir");
  std::vector<double> expected_bins, observed_bins;
  expected_bins.push_back(0.01);
  expected_bins.push_back(0.1);
  observed_bins = ap.get_maf_bin_boundaries();
  EXPECT_EQ(expected_bins.size(), observed_bins.size());
  EXPECT_DOUBLE_EQ(expected_bins.at(0), observed_bins.at(0));
  EXPECT_DOUBLE_EQ(expected_bins.at(1), observed_bins.at(1));
  std::vector<std::string> observed_files;
  observed_files = ap.get_info_gz_files();
  EXPECT_EQ(observed_files.size(), 2UL);
  EXPECT_EQ(observed_files.at(0), _tmp_dir + "/file1.gz");
  EXPECT_EQ(observed_files.at(1), _tmp_dir + "/file2.gz");
  EXPECT_DOUBLE_EQ(ap.get_target_average_r2(), 0.75);
  EXPECT_FLOAT_EQ(ap.get_baseline_r2(), 0.4f);
  EXPECT_EQ(ap.get_output_table_filename(), "summary.txt");
  EXPECT_EQ(ap.get_output_list_filename(), "list.txt");
}

TEST_F(cargsTest, vcfModeBasicAccessors) {
  iddt::cargs ap(_argvec3.size(), _argv3);
  std::ofstream output;
  output.open((_tmp_dir + "/file1.vcf.gz").c_str());
  output.close();
  std::vector<std::string> observed_files;
  observed_files = ap.get_vcf_files();
  EXPECT_EQ(observed_files.size(), 1UL);
  EXPECT_EQ(observed_files.at(0), _tmp_dir + "/file1.vcf.gz");
  EXPECT_EQ(ap.get_vcf_info_r2_tag(), "r2");
  EXPECT_EQ(ap.get_vcf_info_af_tag(), "af");
  EXPECT_EQ(ap.get_vcf_info_imputed_indicator(), "imp");
}

TEST_F(cargsTest, defaultFrequencyBins) {
  iddt::cargs ap(_argvec4.size(), _argv4);
  std::vector<double> observed_bins;
  observed_bins = ap.get_maf_bin_boundaries();
  EXPECT_EQ(observed_bins.size(), 6UL);
  EXPECT_DOUBLE_EQ(observed_bins.at(0), 0.001);
  EXPECT_DOUBLE_EQ(observed_bins.at(1), 0.005);
  EXPECT_DOUBLE_EQ(observed_bins.at(2), 0.01);
  EXPECT_DOUBLE_EQ(observed_bins.at(3), 0.03);
  EXPECT_DOUBLE_EQ(observed_bins.at(4), 0.05);
  EXPECT_DOUBLE_EQ(observed_bins.at(5), 0.5);
}

TEST_F(cargsTest, filterInfoFilesDirOptional) {
  iddt::cargs ap(_argvec5.size(), _argv5);
  EXPECT_EQ(ap.get_filter_info_files_dir(), "");
}

TEST_F(cargsTest, mafBinBoundariesCheckedForValidity) {
  iddt::cargs ap1(_argvec6.size(), _argv6);
  EXPECT_THROW(ap1.get_maf_bin_boundaries(), std::runtime_error);
  iddt::cargs ap2(_argvec7.size(), _argv7);
  EXPECT_THROW(ap2.get_maf_bin_boundaries(), std::runtime_error);
}

TEST_F(cargsTest, mafBinBoundariesCheckedForOrder) {
  iddt::cargs ap(_argvec8.size(), _argv8);
  EXPECT_THROW(ap.get_maf_bin_boundaries(), std::runtime_error);
}

TEST_F(cargsTest, infoGzFilesMustExist) {
  iddt::cargs ap(_argvec6.size(), _argv6);
  EXPECT_THROW(ap.get_info_gz_files(), std::runtime_error);
}

TEST_F(cargsTest, vcfFilesMustExist) {
  iddt::cargs ap(_argvec6.size(), _argv6);
  EXPECT_THROW(ap.get_vcf_files(), std::runtime_error);
}

TEST_F(cargsTest, infoGzFilesOptional) {
  iddt::cargs ap(_argvec7.size(), _argv7);
  EXPECT_EQ(ap.get_info_gz_files(), std::vector<std::string>());
}

TEST_F(cargsTest, vcfFilesOptional) {
  iddt::cargs ap(_argvec7.size(), _argv7);
  EXPECT_EQ(ap.get_vcf_files(), std::vector<std::string>());
}

TEST_F(cargsTest, targetR2CheckedForValidity) {
  iddt::cargs ap1(_argvec6.size(), _argv6);
  EXPECT_THROW(ap1.get_target_average_r2(), std::runtime_error);
  iddt::cargs ap2(_argvec7.size(), _argv7);
  EXPECT_THROW(ap2.get_target_average_r2(), std::runtime_error);
}

TEST_F(cargsTest, baselineR2CheckedForValidity) {
  iddt::cargs ap1(_argvec6.size(), _argv6);
  EXPECT_THROW(ap1.get_baseline_r2(), std::runtime_error);
  iddt::cargs ap2(_argvec7.size(), _argv7);
  EXPECT_THROW(ap2.get_baseline_r2(), std::runtime_error);
}

TEST_F(cargsTest, outputTableFilenameOptional) {
  iddt::cargs ap(_argvec5.size(), _argv5);
  EXPECT_EQ(ap.get_output_table_filename(), "");
}

TEST_F(cargsTest, outputListFilenameOptional) {
  iddt::cargs ap(_argvec5.size(), _argv5);
  EXPECT_EQ(ap.get_output_list_filename(), "");
}
