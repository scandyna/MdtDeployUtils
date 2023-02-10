# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.15)

project(MdtDeployUtils_Test)

enable_testing()

find_package(Mdt0 COMPONENTS DeployUtils REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)

################################################
# Check that deploy utils can be used
################################################

add_test(NAME RunMdtDeployUtils COMMAND Mdt0::DeployUtilsExecutable --help)


################################################
# Check that deploy utils CMake API works
################################################

include(GNUInstallDirs)
include(GenerateExportHeader)
include(MdtDeployApplication)

add_library(deployUtilsLib_Test SHARED
  DeployUtilsLib_Test.cpp
)

generate_export_header(deployUtilsLib_Test)

target_include_directories(deployUtilsLib_Test
  PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(deployUtilsLib_Test
  PUBLIC
    Qt5::Core
)

add_executable(deployUtils_Test DeployUtils_Test.cpp)

target_link_libraries(deployUtils_Test
  PRIVATE
    deployUtilsLib_Test
)

mdt_deploy_application(
  TARGET deployUtils_Test
  RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  CONAN_BUILD_INFO_FILE_PATH "${CMAKE_BINARY_DIR}/conanbuildinfo.txt"
  EXPORT_NAME DeployUtilsTest
  EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
  RUNTIME_COMPONENT ${PROJECT_NAME}_Runtime
  DEVELOPMENT_COMPONENT ${PROJECT_NAME}_Devel
)

add_test(NAME DeployUtils_Test COMMAND "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/deployUtils_Test")
# mdt_set_test_library_env_path(NAME DeployUtils_Test TARGET deployUtils_Test)
