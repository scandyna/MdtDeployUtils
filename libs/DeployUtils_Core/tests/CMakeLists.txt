##############################################################
#  Copyright Philippe Steinmann 2020 - 2023.
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt)
##############################################################
include(MdtAddTest)
include(MdtRuntimeEnvironment)
include(GenerateExportHeader)

# Using Conan with CMakeDeps breaks using $<TARGET_FILE:tgt>
# See https://gitlab.com/scandyna/mdt-cmake-modules/-/issues/12
if(WIN32)
  find_file(Qt5_CORE_SHARED_LIB_PATH Qt5Core.dll Qt5Cored.dll PATH_SUFFIXES "../../../bin" REQUIRED)
  find_file(Qt5_GUI_SHARED_LIB_PATH Qt5Gui.dll Qt5Guid.dll PATH_SUFFIXES "../../../bin" REQUIRED)
else()
  find_library(Qt5_CORE_SHARED_LIB_PATH libQt5Core.so REQUIRED)
  find_library(Qt5_GUI_SHARED_LIB_PATH libQt5Gui.so REQUIRED)
endif()

message(DEBUG "Qt5_CORE_SHARED_LIB_PATH: ${Qt5_CORE_SHARED_LIB_PATH}")
message(DEBUG "Qt5_GUI_SHARED_LIB_PATH: ${Qt5_GUI_SHARED_LIB_PATH}")

# MOC (or CMake?) does not support having ;
string(REPLACE ";" "," TEST_PREFIX_PATH "${CMAKE_PREFIX_PATH}")


add_library(TestLib STATIC
  src/TestUtils.cpp
  src/TestFileUtils.cpp
)

target_include_directories(TestLib
  PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(TestLib
  PUBLIC
    Mdt::DeployUtilsCore
    Qt5::Core
)

mdt_add_test(
  NAME AlgorithmTest
  TARGET algorithmTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/AlgorithmTest.cpp
)

mdt_add_test(
  NAME FileInfoUtilsTest
  TARGET fileInfoUtilsTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileInfoUtilsTest.cpp
)

mdt_add_test(
  NAME FileSystemUtilsTest
  TARGET fileSystemUtilsTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileSystemUtilsTest.cpp
)

mdt_add_test(
  NAME MessageLoggerTest
  TARGET messageLoggerTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/MessageLoggerTest.cpp
)

mdt_add_test(
  NAME PlatformTest
  TARGET platformTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/PlatformTest.cpp
)

mdt_add_test(
  NAME RPathTest
  TARGET rPathTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/RPathTest.cpp
)

mdt_add_test(
  NAME RPathErrorTest
  TARGET rPathErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/RPathErrorTest.cpp
)

mdt_add_test(
  NAME PathTest
  TARGET pathTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Qt5::Test
  SOURCE_FILES
    src/PathTest.cpp
)

mdt_add_test(
  NAME PathListTest
  TARGET pathListTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/PathListTest.cpp
)

mdt_add_test(
  NAME LibraryVersionTest
  TARGET libraryVersionTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LibraryVersionTest.cpp
)

mdt_add_test(
  NAME LibraryNameExtensionTest
  TARGET libraryNameExtensionTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LibraryNameExtensionTest.cpp
)

mdt_add_test(
  NAME LibraryNameTest
  TARGET libraryNameTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LibraryNameTest.cpp
)


# Create libraries to make some tests
# Force those libraries to be a shared/static library
add_library(
  testSharedLibrary SHARED
  src/TestSharedLibrary.cpp
)
generate_export_header(testSharedLibrary)
target_include_directories(testSharedLibrary
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)
target_link_libraries(testSharedLibrary
  PRIVATE
    Qt5::Core
)

add_library(
  testStaticLibrary STATIC
  src/TestStaticLibrary.cpp
)
target_link_libraries(testStaticLibrary
  PRIVATE
    Qt5::Core
)

add_executable(
  testExecutableDynamic
  src/TestExecutableDynamic.cpp
)
target_link_libraries(testExecutableDynamic
  PRIVATE
    testSharedLibrary
    Qt5::Core
)
# On MSVC, also check that delay load libraries works
if(MSVC)
  target_link_options(testExecutableDynamic
    PRIVATE "/DELAYLOAD:${Qt5_CORE_SHARED_LIB_PATH}"
  )
endif()


  # -static cannot be used with some sanitizers
  # requires some work to make this somewhat clean
#   add_executable(
#     testExecutableStatic
#     src/TestExecutableStatic.cpp
#   )
#   set_target_properties(testExecutableStatic
#     PROPERTIES LINK_FLAGS "-static"
#   )



mdt_add_test(
  NAME MsvcVersionTest
  TARGET msvcVersionTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/MsvcVersionTest.cpp
)

if(MSVC)
  mdt_add_test(
    NAME CompilerFinderTest_Msvc
    TARGET compilerFinderTest_Msvc
    DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/CompilerFinderTest_Msvc.cpp
  )
  target_compile_definitions(compilerFinderTest_Msvc PRIVATE CXX_COMPILER_PATH="${CMAKE_CXX_COMPILER}")
endif()

mdt_add_test(
  NAME QtDistributionDirectoryTest
  TARGET qtDistributionDirectoryTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtDistributionDirectoryTest.cpp
)

if(UNIX)
  mdt_add_test(
    NAME QtDistributionDirectoryLinuxHostSpecificTest
    TARGET qtDistributionDirectoryLinuxHostSpecificTest
    DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/QtDistributionDirectoryLinuxHostSpecificTest.cpp
  )
endif()

mdt_add_test(
  NAME QtDistributionDirectoryErrorTest
  TARGET qtDistributionDirectoryErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtDistributionDirectoryErrorTest.cpp
)

mdt_add_test(
  NAME SharedLibraryFinderLinuxTest
  TARGET sharedLibraryFinderLinuxTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/SharedLibraryFinderLinuxTest.cpp
)

mdt_add_test(
  NAME SharedLibraryFinderLinuxErrorTest
  TARGET sharedLibraryFinderLinuxErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/SharedLibraryFinderLinuxErrorTest.cpp
)

mdt_add_test(
  NAME SharedLibraryFinderWindowsTest
  TARGET sharedLibraryFinderWindowsTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/SharedLibraryFinderWindowsTest.cpp
)

mdt_add_test(
  NAME SharedLibraryFinderWindowsErrorTest
  TARGET sharedLibraryFinderWindowsErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/SharedLibraryFinderWindowsErrorTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesFileComparisonImplTest
  TARGET binaryDependenciesFileComparisonImplTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesFileComparisonImplTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesDiscoveredDependenciesListImplTest
  TARGET binaryDependenciesDiscoveredDependenciesListImplTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesDiscoveredDependenciesListImplTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesFileTest
  TARGET binaryDependenciesFileTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesFileTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesGraphFileImplTest
  TARGET binaryDependenciesGraphFileImplTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Boost::boost Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesGraphFileImplTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesGraphImplTest
  TARGET binaryDependenciesGraphImplTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Boost::boost Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesGraphImplTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesResultLibraryTest
  TARGET binaryDependenciesResultLibraryTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesResultLibraryTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesResultTest
  TARGET binaryDependenciesResultTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesResultTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesResultListTest
  TARGET binaryDependenciesResultListTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesResultListTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesTest
  TARGET binaryDependenciesTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesTest.cpp
)
target_compile_definitions(binaryDependenciesTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
target_compile_definitions(binaryDependenciesTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
target_compile_definitions(binaryDependenciesTest PRIVATE PREFIX_PATH="${TEST_PREFIX_PATH}")
if(MSVC)
  target_compile_definitions(binaryDependenciesTest PRIVATE COMPILER_IS_MSVC=1)
  target_compile_definitions(binaryDependenciesTest PRIVATE CXX_COMPILER_PATH="${CMAKE_CXX_COMPILER}")
endif()

mdt_add_test(
  NAME BinaryDependenciesErrorTest
  TARGET binaryDependenciesErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesErrorTest.cpp
)

mdt_add_test(
  NAME FileCopierFileTest
  TARGET fileCopierFileTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileCopierFileTest.cpp
)

mdt_add_test(
  NAME FileCopierTest
  TARGET fileCopierTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileCopierTest.cpp
)

mdt_add_test(
  NAME FileCopierErrorTest
  TARGET fileCopierErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileCopierErrorTest.cpp
)

mdt_add_test(
  NAME LogLevelTest
  TARGET logLevelTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LogLevelTest.cpp
)

mdt_add_test(
  NAME ExecutableFileToInstallTest
  TARGET executableFileToInstallTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ExecutableFileToInstallTest.cpp
)

mdt_add_test(
  NAME ExecutableFileToInstallListTest
  TARGET executableFileToInstallListTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ExecutableFileToInstallListTest.cpp
)

mdt_add_test(
  NAME CopiedExecutableFileTest
  TARGET copiedExecutableFileTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopiedExecutableFileTest.cpp
)

mdt_add_test(
  NAME ExecutableFileInstallerTest
  TARGET executableFileInstallerTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ExecutableFileInstallerTest.cpp
)
target_compile_definitions(executableFileInstallerTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

mdt_add_test(
  NAME SharedLibrariesDeployerTest
  TARGET sharedLibrariesDeployerTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/SharedLibrariesDeployerTest.cpp
)
target_compile_definitions(sharedLibrariesDeployerTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
target_compile_definitions(sharedLibrariesDeployerTest PRIVATE TEST_QT_CORE_FILE_PATH="${Qt5_CORE_SHARED_LIB_PATH}")
target_compile_definitions(sharedLibrariesDeployerTest PRIVATE PREFIX_PATH="${TEST_PREFIX_PATH}")
if(MSVC)
  target_compile_definitions(sharedLibrariesDeployerTest PRIVATE COMPILER_IS_MSVC=1)
  target_compile_definitions(sharedLibrariesDeployerTest PRIVATE CXX_COMPILER_PATH="${CMAKE_CXX_COMPILER}")
endif()

mdt_add_test(
  NAME CopySharedLibrariesTargetDependsOnTest
  TARGET copySharedLibrariesTargetDependsOnTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopySharedLibrariesTargetDependsOnTest.cpp
)
target_compile_definitions(copySharedLibrariesTargetDependsOnTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
target_compile_definitions(copySharedLibrariesTargetDependsOnTest PRIVATE PREFIX_PATH="${TEST_PREFIX_PATH}")
if(MSVC)
  target_compile_definitions(copySharedLibrariesTargetDependsOnTest PRIVATE COMPILER_IS_MSVC=1)
  target_compile_definitions(copySharedLibrariesTargetDependsOnTest PRIVATE CXX_COMPILER_PATH="${CMAKE_CXX_COMPILER}")
endif()

if(MSVC)
  mdt_add_test(
    NAME CopySharedLibrariesTargetDependsOnTest_Msvc
    TARGET copySharedLibrariesTargetDependsOnTest_Msvc
    DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/CopySharedLibrariesTargetDependsOnTest_Msvc.cpp
  )
  target_compile_definitions(copySharedLibrariesTargetDependsOnTest_Msvc PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
  target_compile_definitions(copySharedLibrariesTargetDependsOnTest_Msvc PRIVATE PREFIX_PATH="${TEST_PREFIX_PATH}")
  target_compile_definitions(copySharedLibrariesTargetDependsOnTest_Msvc PRIVATE CXX_COMPILER_PATH="${CMAKE_CXX_COMPILER}")
endif()

mdt_add_test(
  NAME CopySharedLibrariesTargetDependsOnErrorTest
  TARGET copySharedLibrariesTargetDependsOnErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopySharedLibrariesTargetDependsOnErrorTest.cpp
)

mdt_add_test(
  NAME DestinationDirectoryStructureTest
  TARGET destinationDirectoryStructureTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/DestinationDirectoryStructureTest.cpp
)

mdt_add_test(
  NAME DestinationDirectoryTest
  TARGET destinationDirectoryTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/DestinationDirectoryTest.cpp
)

mdt_add_test(
  NAME QtSharedLibraryFileTest
  TARGET qtSharedLibraryFileTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtSharedLibraryFileTest.cpp
)

mdt_add_test(
  NAME QtSharedLibraryTest
  TARGET qtSharedLibraryTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtSharedLibraryTest.cpp
)

mdt_add_test(
  NAME QtModuleListTest
  TARGET qtModuleListTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtModuleListTest.cpp
)

mdt_add_test(
  NAME QtModuleTest
  TARGET qtModuleTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtModuleTest.cpp
)

mdt_add_test(
  NAME QtModuleErrorTest
  TARGET qtModuleErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtModuleErrorTest.cpp
)

mdt_add_test(
  NAME QtPluginFileTest
  TARGET qtPluginFileTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtPluginFileTest.cpp
)

mdt_add_test(
  NAME QtPluginsTest
  TARGET qtPluginsTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtPluginsTest.cpp
)

mdt_add_test(
  NAME QtPluginsSetTest
  TARGET qtPluginsSetTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtPluginsSetTest.cpp
)

mdt_add_test(
  NAME QtModulePluginsTest
  TARGET qtModulePluginsTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtModulePluginsTest.cpp
)
target_compile_definitions(qtModulePluginsTest PRIVATE TEST_QT_GUI_FILE_PATH="${Qt5_GUI_SHARED_LIB_PATH}")

mdt_add_test(
  NAME QtModulePluginsErrorTest
  TARGET qtModulePluginsErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtModulePluginsErrorTest.cpp
)

mdt_add_test(
  NAME QtConfTest
  TARGET qtConfTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtConfTest.cpp
)

mdt_add_test(
  NAME DestinationDirectoryQtConfTest
  TARGET destinationDirectoryQtConfTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/DestinationDirectoryQtConfTest.cpp
)

mdt_add_test(
  NAME QtConfReaderTest
  TARGET qtConfReaderTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtConfReaderTest.cpp
)

mdt_add_test(
  NAME QtConfReaderErrorTest
  TARGET qtConfReaderErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/QtConfReaderErrorTest.cpp
)

mdt_add_test(
  NAME DeployApplicationTest
  TARGET deployApplicationTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/DeployApplicationTest.cpp
)

mdt_add_test(
  NAME DeployApplicationErrorTest
  TARGET deployApplicationErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/DeployApplicationErrorTest.cpp
)
