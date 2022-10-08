# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.14)

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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules/")

include(MdtBuildOptionsUtils)
include(MdtSanitizers)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

option(BUILD_APPS "Build the applications (tools)" ON)
option(BUILD_TESTS "Build the tests" OFF)
option(BUILD_BENCHMARKS "Build the benchmarks" OFF)
option(BUILD_DOCS "Build the documentations" OFF)
mdt_set_available_build_types(Release Debug RelWithDebInfo MinSizeRel Instrumented)
option(WARNING_AS_ERROR "Treat warnings as errors" OFF)

if(BUILD_TESTS)
  option(
    BUILD_REAL_INSTALL_TESTS
    "Build tests that installs components in CMAKE_INSTALL_PREFIX. This should only be enabled in virtualized environment, like a Docker image."
    OFF
  )
endif()

option(BUILD_USE_IPO_LTO_IF_AVAILABLE "Use link-time optimization if available on the platform" OFF)
mark_as_advanced(BUILD_USE_IPO_LTO_IF_AVAILABLE)
# Provide LPO/LTO option if supported
# Note: CMake before 3.9 does only support Intel compiler on Linux.
#       Check documentation of the CheckIPOSupported module,
#       and also CMP0069 policy.
include(CheckIPOSupported)
check_ipo_supported(RESULT HAVE_IPO_LTO)
if(HAVE_IPO_LTO)
  option(BUILD_USE_IPO_LTO "Use link-time optimization" ${BUILD_USE_IPO_LTO_IF_AVAILABLE})
endif()

mdt_get_available_optimization_levels(AVAILABLE_OPTIMIZATION_LEVELS)
set(BUILD_TYPE_INSTRUMENTED_OPTIMIZATION_LEVEL "" CACHE STRING "Set optimization level for Instrumented build")
set_property(CACHE BUILD_TYPE_INSTRUMENTED_OPTIMIZATION_LEVEL PROPERTY STRINGS ${AVAILABLE_OPTIMIZATION_LEVELS})
option(BUILD_TYPE_INSTRUMENTED_USE_DEBUG_SYMBOLS "Add debug symbols (-g on Gcc/Clang, /DEBUG on MSVC)" ON)
option(BUILD_TYPE_INSTRUMENTED_DEFINE_NDEBUG "Set NDEBUG definition for Instrumented build (will disable assert)" OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

mdt_add_address_sanitizer_option_if_available(SANITIZER_ENABLE_ADDRESS
  HELP_STRING "Enable address sanitizer for Instrumented build"
  INITIAL_VALUE OFF
)
mdt_add_memory_sanitizer_option_if_available(SANITIZER_ENABLE_MEMORY
  HELP_STRING "Enable memory sanitizer for Instrumented build"
  INITIAL_VALUE OFF
)
mdt_add_undefined_sanitizer_option_if_available(SANITIZER_ENABLE_UNDEFINED
  HELP_STRING "Enable undefined behaviour sanitizer for Instrumented build"
  INITIAL_VALUE OFF
)
mdt_add_thread_sanitizer_option_if_available(SANITIZER_ENABLE_THREAD
  HELP_STRING "Enable thread sanitizer for Instrumented build (can be incompatible with other sanitizers)"
  INITIAL_VALUE OFF
)

message(DEBUG "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#######################
# Warnings
#######################

if(MSVC)
  add_compile_options(/W3)
  if(WARNING_AS_ERROR)
    add_compile_options(/WX)
  endif()
  # C4996: 'strcpy': This function or variable may be unsafe. Consider using strcpy_s instead (which not works with gcc/clang)
  add_compile_options(/wd4996)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wunused)
  if(WARNING_AS_ERROR)
    add_compile_options(-Werror)
  endif()
endif()

# GCC: mangled name for 'xx' will change in C++17 because the exception specification is part of a function type [-Wnoexcept-type]
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80985
if( (${CMAKE_CXX_COMPILER_ID} MATCHES "GNU") AND (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.0) )
  add_compile_options(-Wno-noexcept-type)
endif()

#######################
# Paths to dependencies
#######################

include(AddQt5ToCMakePrefixPath)

set(QT_PREFIX_PATH CACHE PATH "Path to the root of Qt distribution. (For example: /opt/qt/Qt5/5.13.0/gcc_64). If empty, system distribution is considered.")
add_qt5_to_cmake_prefix_path("${QT_PREFIX_PATH}")

#######################
# Dependencies
#######################

find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Mdt0 COMPONENTS CommandLineArguments ConsoleApplication REQUIRED)

# CommandLineParser uses Boost optional
# For header only library,no component is defined.
# https://cmake.org/pipermail/cmake/2013-September/055941.html
find_package(Boost REQUIRED)


if(BUILD_TESTS)
  find_package(Catch2 REQUIRED)
  find_package(Qt5 COMPONENTS Test Gui REQUIRED)
endif()

if(BUILD_BENCHMARKS)
  find_package(Qt5 COMPONENTS Test REQUIRED)
endif()

if(BUILD_TESTS AND BUILD_APPS)
  find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

if(BUILD_DOCS)
  find_package(Sphinx REQUIRED)

  include(FindDoxygen)
  find_package(Doxygen REQUIRED dot)
endif()

#######################
# Sanitizers
#######################

if(SANITIZER_ENABLE_ADDRESS)
  mdt_build_with_address_sanitizer(BUILD_TYPES Instrumented)
endif()
if(SANITIZER_ENABLE_MEMORY)
  mdt_build_with_memory_sanitizer(BUILD_TYPES Instrumented)
endif()
if(SANITIZER_ENABLE_UNDEFINED)
  mdt_build_with_undefined_sanitizer(BUILD_TYPES Instrumented)
endif()
if(SANITIZER_ENABLE_THREAD)
  mdt_build_with_thread_sanitizer(BUILD_TYPES Instrumented)
endif()

#######################
# Instrumented build
#######################

if(BUILD_TYPE_INSTRUMENTED_OPTIMIZATION_LEVEL)
  add_compile_options($<$<CONFIG:Instrumented>:${BUILD_TYPE_INSTRUMENTED_OPTIMIZATION_LEVEL}>)
endif()

if(BUILD_TYPE_INSTRUMENTED_USE_DEBUG_SYMBOLS)
  mdt_add_debug_symbols_compile_option(BUILD_TYPES Instrumented)
endif()

if(BUILD_TYPE_INSTRUMENTED_DEFINE_NDEBUG)
  add_definitions($<$<CONFIG:Instrumented>:NDEBUG>)
endif()

#######################
# Install
#######################

set(MDT_INSTALL_PACKAGE_NAME Mdt${PROJECT_VERSION_MAJOR})
include(GNUInstallDirs)
include(MdtInstallDirs)
include(MdtPackageConfigHelpers)

# TODO: is this required ?
set(DeployUtilsPackageConfigFileName ${MDT_INSTALL_PACKAGE_NAME}DeployUtilsConfig.cmake)

configure_file(
  MdtDeployUtilsConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${DeployUtilsPackageConfigFileName}"
  @ONLY
)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${DeployUtilsPackageConfigFileName}"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${MDT_INSTALL_PACKAGE_NAME}DeployUtils"
  COMPONENT ${PROJECT_NAME}_Runtime
)

unset(DeployUtilsPackageConfigFileName)

mdt_install_namespace_package_config_file(
  INSTALL_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${MDT_INSTALL_PACKAGE_NAME}"
  COMPONENT ${PROJECT_NAME}_Runtime
)

#######################
# Qt
#######################

# TODO: not a target property, will it be propagated trough projects ??
add_definitions(-DQT_NO_CAST_DEFINITIONS -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

######################
# Tests and benchmarks
######################

if(BUILD_TESTS OR BUILD_BENCHMARKS)
  add_library(Mdt_Catch2Qt STATIC Catch2QString.cpp)
  target_include_directories(Mdt_Catch2Qt
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  )
  target_link_libraries(Mdt_Catch2Qt PUBLIC Qt5::Core)
  add_library(Mdt::Catch2Qt ALIAS Mdt_Catch2Qt)

  add_library(Mdt_Catch2Main STATIC Catch2Main.cpp)
  target_link_libraries(Mdt_Catch2Main PUBLIC Catch2::Catch2)
  add_library(Mdt::Catch2Main ALIAS Mdt_Catch2Main)

  enable_testing()
endif()

if(BUILD_BENCHMARKS)
  target_compile_definitions(Mdt_Catch2Main
    PUBLIC
      CATCH_CONFIG_ENABLE_BENCHMARKING
  )
endif()

#######################
# Sources
#######################

add_subdirectory(libs/DeployUtils_Core)

add_subdirectory(libs/CommandLineParser)

add_subdirectory(cmake)

if(BUILD_APPS)
  add_subdirectory(apps/DeployUtils_Cli)
  if(UNIX)
    add_subdirectory(apps/DeployUtils_Cli_BashCompletion)
  endif()
endif()

if(BUILD_TESTS AND BUILD_APPS)
  add_subdirectory(tests)
endif()

if(BUILD_DOCS)
  add_subdirectory("doc")
endif()
