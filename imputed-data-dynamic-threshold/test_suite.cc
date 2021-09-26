/*!
  \file test_suite.cc
  \brief run class unit tests for
  imputed-data-dynamic-threshold
  \author Lightning Auriga
  \copyright Released under the MIT License.
  Copyright 2021 Lightning Auriga.
 */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main(int argc, char **argv) {
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry =
      CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  bool wasSuccessful = runner.run("", false);
  return !wasSuccessful;
}
