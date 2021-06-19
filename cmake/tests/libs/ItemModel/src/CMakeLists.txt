# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.
cmake_minimum_required(VERSION 3.10)

project(MdtItemModel VERSION 0.0.1)

find_package(MdtCMakeModules REQUIRED)

include(GenerateExportHeader)
include(MdtInstallLibrary)

add_library(Mdt_ItemModel
  Mdt/ItemModel.cpp
)
add_library(Mdt::ItemModel ALIAS Mdt_ItemModel)

target_include_directories(Mdt_ItemModel
  PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

generate_export_header(Mdt_ItemModel)

set(MDT_INSTALL_PACKAGE_NAME Mdt0)
include(GNUInstallDirs)
include(MdtInstallDirs)

mdt_install_library(
  TARGET Mdt_ItemModel
  RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES_DIRECTORY .
  INCLUDES_FILE_WITHOUT_EXTENSION
  ADDITIONAL_INCLUDES_FILES "${CMAKE_CURRENT_BINARY_DIR}/mdt_itemmodel_export.h"
  INCLUDES_DESTINATION ${MDT_INSTALL_INCLUDEDIR}
  EXPORT_NAME ItemModel
  EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}::
  INSTALL_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
  FIND_PACKAGE_PATHS ..
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION_COMPATIBILITY ExactVersion
)
