# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.10)

# CMAKE_PREFIX_PATH is empty before any call to project()
# Because set_project_version() does not exist,
# we call the project command twice (is there a cleaner solution?)
project(MdtDeployUtils)

#######################
# Conan
#######################

if(EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  conan_basic_setup(NO_OUTPUT_DIRS)
endif()

#######################################################
# Project definition with versionning got from git tag
#######################################################

find_package(Git REQUIRED)
find_package(MdtCMakeModules REQUIRED)

include(MdtVersionUtils)

set(INITIAL_PROJECT_VERSION)
if(FROM_CONAN_PROJECT_VERSION)
  set(INITIAL_PROJECT_VERSION ${FROM_CONAN_PROJECT_VERSION})
else()
  mdt_cmake_project_version_from_git_tag(INITIAL_PROJECT_VERSION DEFAULT_VERSION 0.1.0)
endif()

project(MdtDeployUtils VERSION ${INITIAL_PROJECT_VERSION} LANGUAGES CXX)

unset(INITIAL_PROJECT_VERSION)

#######################
# Options
#######################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Modules/")

option(BUILD_DOCS "Build the documentations" OFF)



#######################
# Dependencies
#######################

if(BUILD_DOCS)
  find_package(Sphinx REQUIRED)

  include(FindDoxygen)
  find_package(Doxygen REQUIRED dot)
endif()

#######################
# Sources
#######################

if(BUILD_DOCS)
  add_subdirectory("doc")
endif()
