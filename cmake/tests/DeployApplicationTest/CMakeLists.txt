# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.
cmake_minimum_required(VERSION 3.14)
project(TestApp)

message("CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

find_package(MdtCMakeModules REQUIRED)
find_package(Mdt0DeployUtils REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY)
# find_package(Threads REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Mdt0ItemModel REQUIRED NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH)


# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/bin")
# set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# set(CMAKE_INSTALL_RPATH $ORIGIN)

add_executable(testApp1 src/TestApp1.cpp)
target_link_libraries(testApp1 PRIVATE Mdt0::ItemModel)

add_executable(testApp2 src/TestApp2.cpp)
target_link_libraries(testApp2 PRIVATE Mdt0::ItemModel Qt5::Widgets)

include(GNUInstallDirs)
include(MdtDeployApplication)

mdt_deploy_application(
  TARGET testApp1
  RUNTIME_DESTINATION "testApp1/bin"
  LIBRARY_DESTINATION "testApp1/lib"
)

mdt_deploy_application(
  TARGET testApp2
  RUNTIME_DESTINATION "testApp2/bin"
  LIBRARY_DESTINATION "testApp2/lib"
  QT_PLUGINS_SET "imageformats:jpeg,svg|platforms:xcb,vnc,eglfs,windows,direct2d"
)

# enable_testing()
# add_test(NAME RunTestApp1 COMMAND "${CMAKE_INSTALL_PREFIX}/testApp1/bin/testApp1")
# add_test(NAME RunTestApp2 COMMAND "${CMAKE_INSTALL_PREFIX}/testApp2/bin/testApp2")
