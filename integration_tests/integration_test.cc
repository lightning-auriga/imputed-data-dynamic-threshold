/*!
 \file integration_test.cc
 \brief test of full interpolation passes
 \copyright Released under the MIT License.
 Copyright 2023 Lightning Auriga
 */

#include "integration_tests/integration_test.h"

#include "gtest/gtest.h"

namespace iddt = imputed_data_dynamic_threshold;

integrationTest::integrationTest()
    : testing::Test(),
      _in_info_tmpfile(boost::filesystem::unique_path().native()),
      _in_vcf_tmpfile(""),
      _out_table_tmpfile(boost::filesystem::unique_path().native()),
      _out_list_tmpfile(boost::filesystem::unique_path().native()),
      _out_tmpdir(boost::filesystem::unique_path().native()) {}

integrationTest::~integrationTest() {
  if (boost::filesystem::exists(_in_info_tmpfile)) {
    boost::filesystem::remove(_in_info_tmpfile);
  }
  if (boost::filesystem::exists(_out_table_tmpfile)) {
    boost::filesystem::remove(_out_table_tmpfile);
  }
  if (boost::filesystem::exists(_out_list_tmpfile)) {
    boost::filesystem::remove(_out_list_tmpfile);
  }
  if (boost::filesystem::exists(_out_tmpdir)) {
    boost::filesystem::remove_all(_out_tmpdir);
  }
}

std::string integrationTest::create_plaintext_file(
    const std::string &filename, const std::string &content) const {
  std::ofstream output;
  output.open(filename.c_str());
  if (!output.is_open()) {
    throw std::runtime_error(
        "create_plaintext_file: cannot open write connection");
  }
  if (!(output << content)) {
    throw std::runtime_error("create_plaintext_file: cannot write to file");
  }
  output.close();
  return content;
}

std::string integrationTest::create_compressed_file(
    const std::string &filename, const std::string &content) const {
  gzFile output = NULL;
  try {
    output = gzopen(filename.c_str(), "wb");
    if (!output) {
      throw std::runtime_error(
          "create_compressed_file: cannot open write connection");
    }
    if (gzwrite(output, content.c_str(), content.size()) !=
        static_cast<int>(content.size())) {
      throw std::runtime_error("create_compressed_file: cannot write to file");
    }
    gzclose(output);
    output = NULL;
  } catch (...) {
    if (output) {
      gzclose(output);
      output = NULL;
    }
    throw;
  }
  return content;
}

std::string integrationTest::load_plaintext_file(
    const std::string &filename) const {
  std::ifstream input;
  std::string res = "", line = "";
  input.open(filename.c_str());
  if (!input.is_open()) {
    throw std::runtime_error("load_plaintext_file: unable to open file");
  }
  while (input.peek() != EOF) {
    getline(input, line);
    res += line + "\n";
  }
  input.close();
  return res;
}

std::string integrationTest::load_compressed_file(
    const std::string &filename) const {
  gzFile input = NULL;
  std::string res = "";
  char *buffer = NULL;
  int buffer_size = 2000;
  try {
    buffer = new char[buffer_size];
    input = gzopen(filename.c_str(), "rb");
    if (!input) {
      throw std::runtime_error("load_compressed_file: unable to open file");
    }
    while (gzgets(input, buffer, buffer_size) != Z_NULL) {
      res += std::string(buffer);
    }
    gzclose(input);
    input = 0;
    delete[] buffer;
    buffer = 0;
    return res;
  } catch (...) {
    if (input) {
      gzclose(input);
    }
    if (buffer) {
      delete[] buffer;
    }
    throw;
  }
}

std::string integrationTest::get_info_content() const {
  std::string res =
      "SNP\tREF(0)\tALT(1)\tALT_Frq\tMAF\tAvgCall\tRsq\tGenotyped\t"
      "LooRsq\tEmpR\tEmpRsq\tDose0\tDose1\n"
      "chr1:1:A:T\tA\tT\t0.1\t0.1\t0.1\t0.44231\tImputed\t-\t-\t-\t-\t-\n"
      "chr1:2:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n"
      "chr1:3:G:A\tG\tA\t0.02\t0.02\t0.02\t0.99991\tImputed\t-\t-\t-\t-\t-\n"
      "chr1:4:T:A\tT\tA\t0.4\t0.4\t0.4\t0.34113\tImputed\t-\t-\t-\t-\t-\n"
      "chr1:5:A:T\tA\tT\t0.1\t0.1\t0.1\t0.1\tImputed\t-\t-\t-\t-\t-\n"
      "chr1:6:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n"
      "chr1:7:A:C\tA\tC\t0.1\t0.1\t1.0\t1.0\tGenotyped\t-\t-\t-\t-\t-\n";
  return res;
}

TEST_F(integrationTest, infoInputOnePass) {
  std::string input_info =
      create_plaintext_file(_in_info_tmpfile, get_info_content());
  iddt::executor ex;
  std::vector<double> maf_bin_boundaries;
  maf_bin_boundaries.push_back(0.001);
  maf_bin_boundaries.push_back(0.03);
  maf_bin_boundaries.push_back(0.5);
  std::vector<std::string> info_files, vcf_files;
  info_files.push_back(_in_info_tmpfile);
  double target_r2 = 0.43;
  float baseline_r2 = 0.3f;
  std::string output_table_filename = _out_table_tmpfile;
  std::string output_list_filename = _out_list_tmpfile;
  bool second_pass = false;
  std::string filter_info_files_dir = _out_tmpdir;
  ex.run(maf_bin_boundaries, info_files, vcf_files, target_r2, baseline_r2,
         output_table_filename, output_list_filename, second_pass,
         filter_info_files_dir, "", "", "");
  EXPECT_TRUE(boost::filesystem::exists(output_table_filename));
  EXPECT_TRUE(boost::filesystem::is_regular_file(output_table_filename));
  EXPECT_TRUE(boost::filesystem::exists(output_list_filename));
  EXPECT_TRUE(boost::filesystem::is_regular_file(output_list_filename));

  std::map<std::string, bool> expected_variants;
  expected_variants["chr1:1:A:T"] = false;
  expected_variants["chr1:3:G:A"] = false;
  expected_variants["chr1:6:A:C"] = false;
  expected_variants["chr1:7:A:C"] = false;
  std::ifstream input;
  std::string line = "", varid = "";
  input.open(_out_list_tmpfile.c_str());
  if (!input.is_open()) {
    throw std::runtime_error("output list file: cannot open");
  }
  while (input.peek() != EOF) {
    getline(input, line);
    std::istringstream strm1(line);
    if (!(strm1 >> varid)) {
      throw std::runtime_error("output list file: invalid format");
    }
    EXPECT_NE(expected_variants.find(varid), expected_variants.end());
    expected_variants[varid] = true;
  }
  input.close();
  for (std::map<std::string, bool>::const_iterator iter =
           expected_variants.begin();
       iter != expected_variants.end(); ++iter) {
    EXPECT_TRUE(iter->second);
  }
}

TEST_F(integrationTest, infoInputTwoPasses) {
  std::string input_info =
      create_plaintext_file(_in_info_tmpfile, get_info_content());
  iddt::executor ex;
  std::vector<double> maf_bin_boundaries;
  maf_bin_boundaries.push_back(0.001);
  maf_bin_boundaries.push_back(0.03);
  maf_bin_boundaries.push_back(0.5);
  std::vector<std::string> info_files, vcf_files;
  info_files.push_back(_in_info_tmpfile);
  double target_r2 = 0.43;
  float baseline_r2 = 0.3f;
  std::string output_table_filename = _out_table_tmpfile;
  std::string output_list_filename = _out_list_tmpfile;
  bool second_pass = true;
  std::string filter_info_files_dir = _out_tmpdir;
  ex.run(maf_bin_boundaries, info_files, vcf_files, target_r2, baseline_r2,
         output_table_filename, output_list_filename, second_pass,
         filter_info_files_dir, "", "", "");
  EXPECT_TRUE(boost::filesystem::exists(output_table_filename));
  EXPECT_TRUE(boost::filesystem::is_regular_file(output_table_filename));
  EXPECT_TRUE(boost::filesystem::exists(output_list_filename));
  EXPECT_TRUE(boost::filesystem::is_regular_file(output_list_filename));
  EXPECT_TRUE(boost::filesystem::exists(filter_info_files_dir));
  EXPECT_TRUE(boost::filesystem::is_directory(filter_info_files_dir));
  EXPECT_TRUE(boost::filesystem::exists(
      filter_info_files_dir /
      boost::filesystem::path(_in_info_tmpfile).filename()));
  EXPECT_TRUE(boost::filesystem::is_regular_file(
      filter_info_files_dir /
      boost::filesystem::path(_in_info_tmpfile).filename()));

  std::map<std::string, bool> expected_variants;
  expected_variants["chr1:1:A:T"] = false;
  expected_variants["chr1:3:G:A"] = false;
  expected_variants["chr1:6:A:C"] = false;
  expected_variants["chr1:7:A:C"] = false;
  std::ifstream input;
  std::string line = "", varid = "";
  input.open(_out_list_tmpfile.c_str());
  if (!input.is_open()) {
    throw std::runtime_error("output list file: cannot open");
  }
  while (input.peek() != EOF) {
    getline(input, line);
    std::istringstream strm1(line);
    if (!(strm1 >> varid)) {
      throw std::runtime_error("output list file: invalid format");
    }
    EXPECT_NE(expected_variants.find(varid), expected_variants.end());
    expected_variants[varid] = true;
  }
  input.close();
  for (std::map<std::string, bool>::const_iterator iter =
           expected_variants.begin();
       iter != expected_variants.end(); ++iter) {
    EXPECT_TRUE(iter->second);
  }
}

TEST_F(integrationTest, vcfInputOnePass) {
  std::string vcf1 = "unit_tests/test.vcf.gz";
  std::string vcf2 = "unit_tests/test2.vcf.gz";
  iddt::executor ex;
  std::vector<double> maf_bin_boundaries;
  maf_bin_boundaries.push_back(0.001);
  maf_bin_boundaries.push_back(0.03);
  maf_bin_boundaries.push_back(0.5);
  std::vector<std::string> info_files, vcf_files;
  vcf_files.push_back(vcf1);
  vcf_files.push_back(vcf2);
  double target_r2 = 0.43;
  float baseline_r2 = 0.3f;
  std::string output_table_filename = _out_table_tmpfile;
  std::string output_list_filename = _out_list_tmpfile;
  bool second_pass = false;
  std::string filter_info_files_dir = "";
  ex.run(maf_bin_boundaries, info_files, vcf_files, target_r2, baseline_r2,
         output_table_filename, output_list_filename, second_pass,
         filter_info_files_dir, "DR2", "AF", "IMP");
  EXPECT_TRUE(boost::filesystem::exists(output_table_filename));
  EXPECT_TRUE(boost::filesystem::is_regular_file(output_table_filename));
  EXPECT_TRUE(boost::filesystem::exists(output_list_filename));
  EXPECT_TRUE(boost::filesystem::is_regular_file(output_list_filename));

  std::map<std::string, bool> expected_variants;
  expected_variants["chr1:1:A:T"] = false;
  expected_variants["chr1:3:G:A"] = false;
  expected_variants["chr1:6:A:C"] = false;
  expected_variants["chr1:7:A:C"] = false;
  expected_variants["chr2:1:A:T"] = false;
  expected_variants["chr2:3:G:A"] = false;
  expected_variants["chr2:6:A:C"] = false;
  expected_variants["chr2:7:A:C"] = false;
  std::ifstream input;
  std::string line = "", varid = "";
  input.open(_out_list_tmpfile.c_str());
  if (!input.is_open()) {
    throw std::runtime_error("output list file: cannot open");
  }
  while (input.peek() != EOF) {
    getline(input, line);
    std::istringstream strm1(line);
    if (!(strm1 >> varid)) {
      throw std::runtime_error("output list file: invalid format");
    }
    EXPECT_NE(expected_variants.find(varid), expected_variants.end());
    expected_variants[varid] = true;
  }
  input.close();
  for (std::map<std::string, bool>::const_iterator iter =
           expected_variants.begin();
       iter != expected_variants.end(); ++iter) {
    EXPECT_TRUE(iter->second);
  }
}

TEST_F(integrationTest, vcfInputTwoPasses) {
  std::string vcf1 = "unit_tests/test.vcf.gz";
  std::string vcf2 = "unit_tests/test2.vcf.gz";
  iddt::executor ex;
  std::vector<double> maf_bin_boundaries;
  maf_bin_boundaries.push_back(0.001);
  maf_bin_boundaries.push_back(0.03);
  maf_bin_boundaries.push_back(0.5);
  std::vector<std::string> info_files, vcf_files;
  vcf_files.push_back(vcf1);
  vcf_files.push_back(vcf2);
  double target_r2 = 0.43;
  float baseline_r2 = 0.3f;
  std::string output_table_filename = _out_table_tmpfile;
  std::string output_list_filename = _out_list_tmpfile;
  bool second_pass = true;
  std::string filter_info_files_dir = "";
  ex.run(maf_bin_boundaries, info_files, vcf_files, target_r2, baseline_r2,
         output_table_filename, output_list_filename, second_pass,
         filter_info_files_dir, "DR2", "AF", "IMP");
  EXPECT_TRUE(boost::filesystem::exists(output_table_filename));
  EXPECT_TRUE(boost::filesystem::is_regular_file(output_table_filename));
  EXPECT_TRUE(boost::filesystem::exists(output_list_filename));
  EXPECT_TRUE(boost::filesystem::is_regular_file(output_list_filename));

  std::map<std::string, bool> expected_variants;
  expected_variants["chr1:1:A:T"] = false;
  expected_variants["chr1:3:G:A"] = false;
  expected_variants["chr1:6:A:C"] = false;
  expected_variants["chr1:7:A:C"] = false;
  expected_variants["chr2:1:A:T"] = false;
  expected_variants["chr2:3:G:A"] = false;
  expected_variants["chr2:6:A:C"] = false;
  expected_variants["chr2:7:A:C"] = false;
  std::ifstream input;
  std::string line = "", varid = "";
  input.open(_out_list_tmpfile.c_str());
  if (!input.is_open()) {
    throw std::runtime_error("output list file: cannot open");
  }
  while (input.peek() != EOF) {
    getline(input, line);
    std::istringstream strm1(line);
    if (!(strm1 >> varid)) {
      throw std::runtime_error("output list file: invalid format");
    }
    EXPECT_NE(expected_variants.find(varid), expected_variants.end());
    expected_variants[varid] = true;
  }
  input.close();
  for (std::map<std::string, bool>::const_iterator iter =
           expected_variants.begin();
       iter != expected_variants.end(); ++iter) {
    EXPECT_TRUE(iter->second);
  }
}
