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

  # TODO: remove ?
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
