# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.15)

project(MdtDeployUtilsCore_Test)

find_package(MdtCMakeModules REQUIRED)

include(MdtAddTest)
include(MdtRuntimeEnvironment)

enable_testing()

################################################
# Check that we can create a tool
# based on the C++ API
################################################

find_package(Mdt0 COMPONENTS DeployUtilsCore REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)

add_executable(deployUtils_Test DeployUtils_Test.cpp)

target_link_libraries(deployUtils_Test
  PRIVATE
    Mdt0::DeployUtilsCore Qt5::Core
)

add_test(NAME DeployUtils_Test COMMAND deployUtils_Test)
mdt_set_test_library_env_path(NAME DeployUtils_Test TARGET deployUtils_Test)
