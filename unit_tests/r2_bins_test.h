/*!
  \file r2_bins_test.h
  \brief tests for r2_bins class
  \copyright Released under the MIT License. Copyright
  2021 Lightning Auriga
 */

#ifndef UNIT_TESTS_R2_BINS_TEST_H_
#define UNIT_TESTS_R2_BINS_TEST_H_

#include <zlib.h>

#include <cfloat>
#include <cmath>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "boost/filesystem.hpp"
#include "gtest/gtest.h"
#include "imputed-data-dynamic-threshold/r2_bins.h"

class r2BinsTest : public testing::Test {
 protected:
  r2BinsTest();
  ~r2BinsTest() throw();
  const std::string _tmp_dir;
};

#endif  // UNIT_TESTS_R2_BINS_TEST_H_
