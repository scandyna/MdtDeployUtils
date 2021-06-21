##############################################################
#  Copyright Philippe Steinmann 2020 - 2020.
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt)
##############################################################
include(GenerateExportHeader)
include(MdtInstallLibrary)

add_library(Mdt_DeployUtils
  Mdt/DeployUtils/QRuntimeError.cpp
  Mdt/DeployUtils/FileOpenError.cpp
  Mdt/DeployUtils/Platform.cpp
  Mdt/DeployUtils/AbstractMessageLoggerBackend.cpp
  Mdt/DeployUtils/MessageLogger.cpp
  Mdt/DeployUtils/ConsoleMessageLogger.cpp
  Mdt/DeployUtils/CMakeStyleMessageLogger.cpp
  Mdt/DeployUtils/Algorithm.cpp
  Mdt/DeployUtils/PathList.cpp
  Mdt/DeployUtils/SearchPathList.cpp
  Mdt/DeployUtils/LibraryVersion.cpp
  Mdt/DeployUtils/LibraryNameExtension.cpp
  Mdt/DeployUtils/Impl/LibraryNameImpl.cpp
  Mdt/DeployUtils/LibraryName.cpp
  Mdt/DeployUtils/LibraryInfo.cpp
  Mdt/DeployUtils/LibraryInfoList.cpp
  Mdt/DeployUtils/ExecutableFileReadError.cpp
  Mdt/DeployUtils/AbstractExecutableFileReaderEngine.cpp
  Mdt/DeployUtils/Impl/NotNullTerminatedStringError.cpp
  Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.cpp
  Mdt/DeployUtils/Impl/ByteArraySpan.cpp
  Mdt/DeployUtils/Impl/FileMapper.cpp
  Mdt/DeployUtils/Impl/Elf/Ident.cpp
  Mdt/DeployUtils/Impl/Elf/SectionHeader.cpp
  Mdt/DeployUtils/Impl/Elf/FileHeader.cpp
  Mdt/DeployUtils/Impl/Elf/DynamicSection.cpp
  Mdt/DeployUtils/Impl/Elf/FileReader.cpp
  Mdt/DeployUtils/Impl/Elf/Debug.cpp
  Mdt/DeployUtils/ElfFileReader.cpp
  Mdt/DeployUtils/Impl/Pe/Exceptions.cpp
  Mdt/DeployUtils/Impl/Pe/SectionHeader.cpp
  Mdt/DeployUtils/Impl/Pe/ImportDirectory.cpp
  Mdt/DeployUtils/Impl/Pe/FileHeader.cpp
  Mdt/DeployUtils/Impl/Pe/FileReader.cpp
  Mdt/DeployUtils/Impl/Pe/Debug.cpp
  Mdt/DeployUtils/PeFileReader.cpp
  Mdt/DeployUtils/ExecutableFileReader.cpp
#   Mdt/DeployUtils/BinaryDependenciesImplementationInterface.cpp
#   Mdt/DeployUtils/BinaryDependencies.cpp
  Mdt/DeployUtils/CopySharedLibrariesTargetDependsOn.cpp
  Mdt/DeployUtils/CopySharedLibrariesTargetDependsOnRequest.cpp
)
add_library(Mdt::DeployUtils ALIAS Mdt_DeployUtils)

target_compile_features(Mdt_DeployUtils PUBLIC cxx_std_14)

if(BUILD_USE_IPO_LTO)
  set_target_properties(Mdt_DeployUtils
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_include_directories(Mdt_DeployUtils
  PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(Mdt_DeployUtils
  PUBLIC
    Qt5::Core
)

generate_export_header(Mdt_DeployUtils)

mdt_install_library(
  TARGET Mdt_DeployUtils
  RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES_DIRECTORY .
  INCLUDES_FILE_WITHOUT_EXTENSION
  ADDITIONAL_INCLUDES_FILES "${CMAKE_CURRENT_BINARY_DIR}/mdt_deployutils_export.h"
  INCLUDES_DESTINATION ${MDT_INSTALL_INCLUDEDIR}
  EXPORT_NAME DeployUtils
  EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}::
  INSTALL_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
  FIND_PACKAGE_PATHS ..
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION_COMPATIBILITY ExactVersion
  RUNTIME_COMPONENT ${PROJECT_NAME}_Runtime
  DEVELOPMENT_COMPONENT ${PROJECT_NAME}_Dev
)
