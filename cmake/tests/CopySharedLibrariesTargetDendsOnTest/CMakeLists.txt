# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.
cmake_minimum_required(VERSION 3.14)
project(TestApp)

# TODO: add a dependency to Qt

find_package(MdtCMakeModules REQUIRED)
find_package(Mdt0DeployUtils REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY)
# find_package(Threads REQUIRED)
# find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Mdt0ItemModel REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH $ORIGIN)

add_executable(testApp src/TestApp.cpp)
target_link_libraries(testApp PRIVATE Mdt0::ItemModel)

include(GNUInstallDirs)
include(MdtSharedLibrariesDepencyHelpers)

mdt_copy_shared_libraries_target_depends_on(
  TARGET testApp
  DESTINATION "${CMAKE_BINARY_DIR}/bin"
  OVERWRITE_BEHAVIOR OVERWRITE
  REMOVE_RPATH FALSE
)

enable_testing()
add_test(NAME RunTestApp COMMAND testApp)

# TODO: goes to InstallSharedLibrariesTargetDendsOnTest test
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

