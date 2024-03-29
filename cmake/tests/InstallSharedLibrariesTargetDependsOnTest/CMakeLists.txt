# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.
cmake_minimum_required(VERSION 3.14)
project(TestApp)

# TODO: add a dependency to Qt

message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

find_package(MdtCMakeModules REQUIRED)
find_package(Mdt0DeployUtils REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY)
# find_package(Threads REQUIRED)
# find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Mdt0ItemModel REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH)


# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/bin")
# set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")

add_executable(testApp1 src/TestApp1.cpp)
target_link_libraries(testApp1 PRIVATE Mdt0::ItemModel)

add_executable(testApp2 src/TestApp2.cpp)
target_link_libraries(testApp2 PRIVATE Mdt0::ItemModel)

include(GNUInstallDirs)
include(MdtSharedLibrariesDepencyHelpers)

# mdt_install_shared_libraries_target_depends_on(
#   TARGET testApp1
#   RUNTIME_DESTINATION "${CMAKE_BINARY_DIR}/$<CONFIG>/bin"
#   LIBRARY_DESTINATION "${CMAKE_BINARY_DIR}/$<CONFIG>/lib"
# )

install(
  TARGETS testApp1
  DESTINATION "testApp1/bin"
)

install(
  TARGETS testApp2
  DESTINATION "testApp2/bin"
)

mdt_install_shared_libraries_target_depends_on(
  TARGET testApp1
  RUNTIME_DESTINATION "testApp1/bin"
  LIBRARY_DESTINATION "testApp1/lib"
)

mdt_install_shared_libraries_target_depends_on(
  TARGET testApp2
  RUNTIME_DESTINATION "testApp2/bin"
  LIBRARY_DESTINATION "testApp2/lib"
)

# enable_testing()
# add_test(NAME RunTestApp1 COMMAND "${CMAKE_INSTALL_PREFIX}/testApp1/bin/testApp1")
# add_test(NAME RunTestApp2 COMMAND "${CMAKE_INSTALL_PREFIX}/testApp2/bin/testApp2")
