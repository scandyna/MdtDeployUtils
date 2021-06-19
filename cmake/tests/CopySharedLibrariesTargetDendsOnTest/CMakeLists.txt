# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.
cmake_minimum_required(VERSION 3.10)
project(TestApp)

# TODO: at the end, should install 2 libraries in separate paths, with RPATH set to $ORIGIN
#       this should check searching libraries not found by ldd
#       Also add Qt as dependency

message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")


find_package(MdtCMakeModules REQUIRED)
find_package(Mdt0DeployUtilsCMake REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH)
# find_package(Mdt0 COMPONENTS DeployUtilsCMake REQUIRED)
# find_package(MdtCMakeModules REQUIRED)
# find_package(Threads REQUIRED)
# find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
# find_package(Mdt0 COMPONENTS PlainText REQUIRED)
find_package(Mdt0ItemModel REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH)

message("CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

add_executable(testApp src/TestApp.cpp)
target_link_libraries(testApp PRIVATE Mdt0::ItemModel)

include(GNUInstallDirs)
include(MdtFindDeployUtilsTools)
include(MdtSharedLibrariesDepencyHelpers)

install(
  TARGETS testApp
  RUNTIME DESTINATION bin
)

mdt_find_deploy_utils_tools()

mdt_copy_shared_libraries_target_depends_on(
  TARGET testApp
  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  OVERWRITE_BEHAVIOR OVERWRITE
  REMOVE_RPATH FALSE
)

# This test aims to check if all required shared libraries are properly copied
# On a developer machine, Qt is probably installed.
# So, as first pass, we create a test library that will be installed.
# If this library is not installed properly, the test executable will not run.
# The second library is to check this problem:
# https://scandyna.gitlab.io/mdt-cmake-modules/Modules/MdtRuntimeEnvironment.html
#
# To also check that Qt, and its dependencies, are copied,
# this test should run in a environment without Qt installed on the system.
# This is done on the CI with a specific docker image.

