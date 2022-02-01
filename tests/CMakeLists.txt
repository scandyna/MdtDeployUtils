# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.
cmake_minimum_required(VERSION 3.14)

# project(QtCoreApp VERSION 0.0.1)
project(SystemTests  LANGUAGES CXX)

find_package(MdtCMakeModules REQUIRED)
# find_package(Mdt0ItemModel 0.0.1 REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH)
find_package(Qt5 COMPONENTS Core REQUIRED)

# set(MDT_INSTALL_PACKAGE_NAME Mdt0)
include(GNUInstallDirs)
include(MdtInstallDirs)

# set(CMAKE_SKIP_RPATH TRUE)

enable_testing()

# add_subdirectory(libs)
add_subdirectory(apps)
add_subdirectory(system)
