# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.10)

project(MdtDeployUtils_Test)

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  conan_basic_setup(NO_OUTPUT_DIRS)
endif()

enable_testing()

################################################
# Check that deploy utils can be used
################################################

find_package(Mdt0 COMPONENTS DeployUtils REQUIRED)

add_test(NAME RunMdtDeployUtils COMMAND Mdt0::DeployUtilsExecutable --help)
