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
      _tmp_dir(boost::filesystem::unique_path().native()) {
  std::string test1 = "progname -h";
  populate(test1, &_argvec1, &_argv1);
  std::string test2 =
      "progname -i " + _tmp_dir + "/file1.gz " + _tmp_dir +
      "/file2.gz -m 0.01 0.1 -r 0.75 "
      "-s --filter-info-files targetdir -o summary.txt -l list.txt";
  populate(test2, &_argvec2, &_argv2);
  std::string test3 = "progname -v " + _tmp_dir +
                      "/file1.vcf.gz "
                      "--vcf-info-r2-tag r2 "
                      "--vcf-info-af-tag af "
                      "--vcf-info-imputed-indicator imp "
                      "-s --filter-vcf-files outdir";
  populate(test3, &_argvec3, &_argv3);
  std::string test4 = "progname -i " + _tmp_dir +
                      "/file1.gz -o summary.txt "
                      "--filter-info-files targetdir";
  populate(test4, &_argvec4, &_argv4);
  std::string test5 = "progname --version";
  populate(test5, &_argvec5, &_argv5);
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
  EXPECT_EQ(ap.get_filter_vcf_files_dir(), "outdir");
}
