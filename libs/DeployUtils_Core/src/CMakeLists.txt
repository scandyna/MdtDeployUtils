##############################################################
#  Copyright Philippe Steinmann 2020 - 2023.
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt)
##############################################################
include(GenerateExportHeader)
include(MdtInstallLibrary)

add_library(Mdt_DeployUtilsCore
  Mdt/DeployUtils/QRuntimeError.cpp
  Mdt/DeployUtils/FileOpenError.cpp
  Mdt/DeployUtils/BuildType.cpp
  Mdt/DeployUtils/Platform.cpp
  Mdt/DeployUtils/AbstractMessageLoggerBackend.cpp
  Mdt/DeployUtils/MessageLogger.cpp
  Mdt/DeployUtils/ConsoleMessageLogger.cpp
  Mdt/DeployUtils/CMakeStyleMessageLogger.cpp
  Mdt/DeployUtils/Algorithm.cpp
  Mdt/DeployUtils/FileInfoUtils.cpp
  Mdt/DeployUtils/FileSystemUtils.cpp
  Mdt/DeployUtils/RPath.cpp
  Mdt/DeployUtils/PathList.cpp
  Mdt/DeployUtils/SearchPathList.cpp
  Mdt/DeployUtils/LibraryVersion.cpp
  Mdt/DeployUtils/LibraryNameExtension.cpp
  Mdt/DeployUtils/Impl/LibraryNameImpl.cpp
  Mdt/DeployUtils/LibraryName.cpp

  # TODO: should also remove those
  Mdt/DeployUtils/Impl/StringTableError.cpp
#   Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.cpp
  Mdt/DeployUtils/Impl/ByteArraySpan.cpp
  Mdt/DeployUtils/Impl/FileMapper.cpp

#   Mdt/DeployUtils/Impl/Elf/Exceptions.cpp
#   Mdt/DeployUtils/Impl/Elf/Algorithm.cpp
#   Mdt/DeployUtils/Impl/Elf/Ident.cpp
#   Mdt/DeployUtils/Impl/Elf/FileWriterUtils.cpp
#   Mdt/DeployUtils/Impl/Elf/ProgramHeader.cpp
#   Mdt/DeployUtils/Impl/Elf/ProgramHeaderTable.cpp
#   Mdt/DeployUtils/Impl/Elf/ProgramHeaderReaderWriterCommon.cpp
#   Mdt/DeployUtils/Impl/Elf/ProgramHeaderReader.cpp
#   Mdt/DeployUtils/Impl/Elf/ProgramHeaderWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/SectionIndexChangeMap.cpp
#   Mdt/DeployUtils/Impl/Elf/SectionHeader.cpp
#   Mdt/DeployUtils/Impl/Elf/SectionHeaderTable.cpp
#   Mdt/DeployUtils/Impl/Elf/SectionHeaderReaderWriterCommon.cpp
#   Mdt/DeployUtils/Impl/Elf/SectionHeaderWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/FileHeader.cpp
#   Mdt/DeployUtils/Impl/Elf/FileHeaderReaderWriterCommon.cpp
#   Mdt/DeployUtils/Impl/Elf/FileHeaderWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/SectionSegmentUtils.cpp
#   Mdt/DeployUtils/Impl/Elf/FileAllHeaders.cpp
#   Mdt/DeployUtils/Impl/Elf/FileAllHeadersReaderWriterCommon.cpp
#   Mdt/DeployUtils/Impl/Elf/FileAllHeadersReader.cpp
#   Mdt/DeployUtils/Impl/Elf/FileAllHeadersWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/FileWriterFileLayout.cpp
#   Mdt/DeployUtils/Impl/Elf/FileWriterFile.cpp
#   Mdt/DeployUtils/Impl/Elf/StringTable.cpp
#   Mdt/DeployUtils/Impl/Elf/StringTableWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/DynamicSection.cpp
#   Mdt/DeployUtils/Impl/Elf/DynamicSectionWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/SymbolTable.cpp
#   Mdt/DeployUtils/Impl/Elf/SymbolTableReader.cpp
#   Mdt/DeployUtils/Impl/Elf/SymbolTableWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/HashTable.cpp
#   Mdt/DeployUtils/Impl/Elf/HashTableReader.cpp
#   Mdt/DeployUtils/Impl/Elf/HashTableWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/GnuHashTable.cpp
#   Mdt/DeployUtils/Impl/Elf/GnuHashTableReader.cpp
#   Mdt/DeployUtils/Impl/Elf/GnuHashTableWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/GlobalOffsetTable.cpp
#   Mdt/DeployUtils/Impl/Elf/GlobalOffsetTableReaderWriterCommon.cpp
#   Mdt/DeployUtils/Impl/Elf/GlobalOffsetTableReader.cpp
#   Mdt/DeployUtils/Impl/Elf/GlobalOffsetTableWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/ProgramInterpreterSection.cpp
#   Mdt/DeployUtils/Impl/Elf/ProgramInterpreterSectionReader.cpp
#   Mdt/DeployUtils/Impl/Elf/ProgramInterpreterSectionWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/NoteSection.cpp
#   Mdt/DeployUtils/Impl/Elf/NoteSectionTable.cpp
#   Mdt/DeployUtils/Impl/Elf/NoteSectionReader.cpp
#   Mdt/DeployUtils/Impl/Elf/NoteSectionWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/OffsetRange.cpp
#   Mdt/DeployUtils/Impl/Elf/FileOffsetChanges.cpp
#   Mdt/DeployUtils/Impl/Elf/FileReader.cpp
#   Mdt/DeployUtils/Impl/Elf/FileWriter.cpp
#   Mdt/DeployUtils/Impl/Elf/FileIoEngine.cpp
#   Mdt/DeployUtils/Impl/Elf/Debug.cpp


#   Mdt/DeployUtils/Impl/Pe/Exceptions.cpp
#   Mdt/DeployUtils/Impl/Pe/SectionHeader.cpp
#   Mdt/DeployUtils/Impl/Pe/ImportDirectory.cpp
#   Mdt/DeployUtils/Impl/Pe/FileHeader.cpp
#   Mdt/DeployUtils/Impl/Pe/FileReader.cpp
#   Mdt/DeployUtils/Impl/Pe/Debug.cpp


  Mdt/DeployUtils/FindCompilerError.cpp
  Mdt/DeployUtils/CompilerLocationType.cpp
  Mdt/DeployUtils/CompilerLocationRequest.cpp
  Mdt/DeployUtils/AbstractCompilerFinderEngine.cpp
  Mdt/DeployUtils/MsvcVersion.cpp
  Mdt/DeployUtils/MsvcFinder.cpp
  Mdt/DeployUtils/CompilerFinder.cpp
  Mdt/DeployUtils/AbstractIsExistingValidSharedLibrary.cpp
  Mdt/DeployUtils/IsExistingValidSharedLibrary.cpp
  Mdt/DeployUtils/AbstractSharedLibraryFinder.cpp
  Mdt/DeployUtils/SharedLibraryFinderCommon.cpp
  Mdt/DeployUtils/SharedLibraryFinderLinux.cpp
  Mdt/DeployUtils/SharedLibraryFinderWindows.cpp
  Mdt/DeployUtils/Impl/BinaryDependencies/DiscoveredDependenciesList.cpp
  Mdt/DeployUtils/Impl/BinaryDependencies/FileComparison.cpp
  Mdt/DeployUtils/Impl/BinaryDependencies/GraphFile.cpp
  Mdt/DeployUtils/Impl/BinaryDependencies/GraphDef.cpp
  Mdt/DeployUtils/Impl/BinaryDependencies/GraphBuildVisitor.cpp
  Mdt/DeployUtils/Impl/BinaryDependencies/GraphResultVisitor.cpp
  Mdt/DeployUtils/Impl/BinaryDependencies/Graph.cpp
  Mdt/DeployUtils/BinaryDependenciesFile.cpp
  Mdt/DeployUtils/BinaryDependenciesResultLibrary.cpp
  Mdt/DeployUtils/BinaryDependenciesResult.cpp
  Mdt/DeployUtils/BinaryDependenciesResultList.cpp
  Mdt/DeployUtils/BinaryDependencies.cpp
  Mdt/DeployUtils/FileCopyError.cpp
  Mdt/DeployUtils/FileCopierFile.cpp
  Mdt/DeployUtils/FileCopier.cpp
  Mdt/DeployUtils/LogLevel.cpp
  Mdt/DeployUtils/DestinationDirectoryStructure.cpp
  Mdt/DeployUtils/DestinationDirectory.cpp
  Mdt/DeployUtils/ExecutableFileToInstall.cpp
  Mdt/DeployUtils/ExecutableFileToInstallList.cpp
  Mdt/DeployUtils/CopiedExecutableFile.cpp
  Mdt/DeployUtils/ExecutableFileInstaller.cpp
  Mdt/DeployUtils/SharedLibrariesDeployer.cpp
  Mdt/DeployUtils/CopySharedLibrariesTargetDependsOn.cpp
  Mdt/DeployUtils/CopySharedLibrariesTargetDependsOnRequest.cpp
  Mdt/DeployUtils/QtSharedLibraryError.cpp
  Mdt/DeployUtils/QtSharedLibraryFile.cpp
  Mdt/DeployUtils/QtSharedLibrary.cpp
  Mdt/DeployUtils/UnknownQtModuleError.cpp
  Mdt/DeployUtils/QtDistributionDirectory.cpp
  Mdt/DeployUtils/QtModule.cpp
  Mdt/DeployUtils/QtModuleList.cpp
  Mdt/DeployUtils/FindQtPluginError.cpp
  Mdt/DeployUtils/QtPluginFile.cpp
  Mdt/DeployUtils/QtPlugins.cpp
  Mdt/DeployUtils/QtPluginsSet.cpp
  Mdt/DeployUtils/QtModulePlugins.cpp
  Mdt/DeployUtils/QtConf.cpp
  Mdt/DeployUtils/ReadQtConfError.cpp
  Mdt/DeployUtils/QtConfReader.cpp
  Mdt/DeployUtils/WriteQtConfError.cpp
  Mdt/DeployUtils/QtConfWriter.cpp
  Mdt/DeployUtils/DestinationDirectoryQtConf.cpp
  Mdt/DeployUtils/DeployApplicationRequest.cpp
  Mdt/DeployUtils/DeployApplicationError.cpp
  Mdt/DeployUtils/DeployApplication.cpp
)
add_library(Mdt::DeployUtilsCore ALIAS Mdt_DeployUtilsCore)

target_compile_features(Mdt_DeployUtilsCore PUBLIC cxx_std_17)

if(BUILD_USE_IPO_LTO)
  set_target_properties(Mdt_DeployUtilsCore
    PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE
  )
endif()

target_include_directories(Mdt_DeployUtilsCore
  PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(Mdt_DeployUtilsCore
  PUBLIC
    Qt5::Core
    Mdt0::ExecutableFileCore
  PRIVATE
    Boost::boost
)

generate_export_header(Mdt_DeployUtilsCore)

mdt_install_library(
  TARGET Mdt_DeployUtilsCore
  RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES_DIRECTORY .
  INCLUDES_FILE_WITHOUT_EXTENSION
  ADDITIONAL_INCLUDES_FILES "${CMAKE_CURRENT_BINARY_DIR}/mdt_deployutilscore_export.h"
  INCLUDES_DESTINATION ${MDT_INSTALL_INCLUDEDIR}
  EXPORT_NAME DeployUtilsCore
  EXPORT_NAMESPACE Mdt${PROJECT_VERSION_MAJOR}::
  INSTALL_NAMESPACE ${MDT_INSTALL_PACKAGE_NAME}
  FIND_PACKAGE_PATHS ..
  INSTALL_IS_UNIX_SYSTEM_WIDE ${MDT_INSTALL_IS_UNIX_SYSTEM_WIDE}
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  VERSION_COMPATIBILITY ExactVersion
  RUNTIME_COMPONENT MdtDeployUtilsCore_Runtime
  DEVELOPMENT_COMPONENT MdtDeployUtilsCore_Dev
)
