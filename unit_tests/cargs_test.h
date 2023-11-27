/*!
 \file cargs_test.h
 \brief declaration of test suite for command line interface
 \author Lightning Auriga
 \copyright Released under the MIT License.
 Copyright 2023 Lightning Auriga
 */

#ifndef UNIT_TESTS_CARGS_TEST_H_
#define UNIT_TESTS_CARGS_TEST_H_

#include <cfloat>
#include <fstream>
#include <string>
#include <vector>

#include "boost/filesystem.hpp"
#include "gtest/gtest.h"
#include "imputed-data-dynamic-threshold/cargs.h"

class cargsTest : public testing::Test {
 protected:
  cargsTest();
  ~cargsTest() throw();
  void populate(const std::string &str, std::vector<std::string> *vec,
                const char ***arr);
  std::vector<std::string> _argvec1;
  std::vector<std::string> _argvec2;
  std::vector<std::string> _argvec3;
  std::vector<std::string> _argvec4;
  std::vector<std::string> _argvec5;
  std::vector<std::string> _argvec6;
  std::vector<std::string> _argvec7;
  std::vector<std::string> _argvec8;
  const char **_argv1;
  const char **_argv2;
  const char **_argv3;
  const char **_argv4;
  const char **_argv5;
  const char **_argv6;
  const char **_argv7;
  const char **_argv8;
  const std::string _tmp_dir;
};
#endif  // UNIT_TESTS_CARGS_TEST_H_
