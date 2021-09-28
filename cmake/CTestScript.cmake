# Copyright (c) 2020, RTE (http://www.rte-france.com)
# See AUTHORS.txt
# All rights reserved.
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, you can obtain one at http://mozilla.org/MPL/2.0/.
# SPDX-License-Identifier: MPL-2.0
#

# It might be better than calling 'make Continuous' for CI such as travis since it will skip the update step while still doing the later steps
# It is also makes it easier to customize the test runs and show the output since we can use command-line arguments
set(CTEST_SOURCE_DIRECTORY "./")
set(CTEST_BINARY_DIRECTORY "./buildCoverage")

set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
#set(CTEST_USE_LAUNCHERS 1)
set(CTEST_PROJECT_NAME "DynaFlowLauncher")
set(CTEST_BUILD_CONFIGURATION "Coverage") # Only works for multi-config generators

if(NOT "$ENV{GCOV}" STREQUAL "")
    set(GCOV_NAME "$ENV{GCOV}")
else()
    set(GCOV_NAME "gcov")
endif()

find_program(LCOV_PATH lcov)
if (NOT LCOV_PATH)
  message(FATAL_ERROR "lcov not found")
endif()
set(GENHTML_OPTIONS "")
find_program(GENHTML_PATH genhtml)
if (NOT GENHTML_PATH)
  message(FATAL_ERROR "genhtml not found")
else()
  execute_process(COMMAND ${GENHTML_PATH} "--version" OUTPUT_VARIABLE GENHTML_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  # version is written as genhtml: LCOV version <version>
  # works on string to keep only version
  set( VERSION ${GENHTML_VERSION} )
  separate_arguments(VERSION)
  list(GET VERSION -1 VERSION_NUMBER)

  set(GENHTML_OPTIONS "--no-function-coverage" )
endif()

find_program(CTEST_COVERAGE_COMMAND NAMES "${GCOV_NAME}")

ctest_start("Continuous")
ctest_configure(OPTIONS -DCMAKE_BUILD_TYPE=${CTEST_BUILD_CONFIGURATION})
ctest_configure(APPEND OPTIONS -DDYNAWO_HOME=${DYNAWO_HOME})
ctest_configure(APPEND OPTIONS -DDYNAWO_ALGORITHMS_HOME=${DYNAWO_ALGORITHMS_HOME})
ctest_configure(APPEND OPTIONS -DBOOST_ROOT=${BOOST_ROOT})
ctest_configure(APPEND OPTIONS -DCMAKE_INSTALL_PREFIX=${CTEST_BINARY_DIRECTORY}/Install)
# Done by default when not using a script... Do it here since the CTestCustom.cmake file is generated by our CMakeLists.
ctest_read_custom_files( ${CTEST_BINARY_DIRECTORY} )
ctest_build()
ctest_test(RETURN_VALUE RET_VAL_TEST)
if(RET_VAL_TEST)
    # We need to send an error if we want to detect fails in CI
    message(FATAL_ERROR "Some tests failed !")
endif()
if(CTEST_COVERAGE_COMMAND)
    ctest_coverage()
else()
    message(WARNING "GCov not found, not running coverage")
endif()
make_directory(${CTEST_BINARY_DIRECTORY}/coverage)
execute_process(COMMAND ${LCOV_PATH} --directory ${CTEST_BINARY_DIRECTORY}/sources -c -o ${CTEST_BINARY_DIRECTORY}/coverage/lcovreport.info.tmp)
execute_process(COMMAND ${LCOV_PATH} --extract ${CTEST_BINARY_DIRECTORY}/coverage/lcovreport.info.tmp "*/sources/*" --output-file ${CTEST_BINARY_DIRECTORY}/coverage/lcovreport.info)
execute_process(COMMAND ${GENHTML_PATH} ${GENHTML_OPTIONS} -o ${CTEST_BINARY_DIRECTORY}/coverage ${CTEST_BINARY_DIRECTORY}/coverage/lcovreport.info)
#ctest_memcheck()
# ctest_submit() # Comment this if you want to use the script but not use CDash

