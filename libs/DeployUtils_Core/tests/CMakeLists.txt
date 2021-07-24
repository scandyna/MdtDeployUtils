##############################################################
#  Copyright Philippe Steinmann 2020 - 2020.
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt)
##############################################################
include(MdtAddTest)
include(GenerateExportHeader)

# MOC (or CMake?) does not support having ;
string(REPLACE ";" "," TEST_PREFIX_PATH "${CMAKE_PREFIX_PATH}")


mdt_add_test(
  NAME AlgorithmTest
  TARGET algorithmTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main
  SOURCE_FILES
    src/AlgorithmTest.cpp
)

mdt_add_test(
  NAME MessageLoggerTest
  TARGET messageLoggerTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/MessageLoggerTest.cpp
)

mdt_add_test(
  NAME PlatformTest
  TARGET platformTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/PlatformTest.cpp
)

mdt_add_test(
  NAME PathTest
  TARGET pathTest
  DEPENDENCIES Mdt::DeployUtils Qt5::Test
  SOURCE_FILES
    src/PathTest.cpp
)

mdt_add_test(
  NAME LibraryVersionTest
  TARGET libraryVersionTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LibraryVersionTest.cpp
)

mdt_add_test(
  NAME LibraryNameExtensionTest
  TARGET libraryNameExtensionTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LibraryNameExtensionTest.cpp
)

mdt_add_test(
  NAME LibraryNameTest
  TARGET libraryNameTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LibraryNameTest.cpp
)

mdt_add_test(
  NAME LibraryInfoTest
  TARGET libraryInfoTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LibraryInfoTest.cpp
)

mdt_add_test(
  NAME LibraryInfoListTest
  TARGET libraryInfoListTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/LibraryInfoListTest.cpp
)

mdt_add_test(
  NAME ByteArraySpanTest
  TARGET byteArraySpanTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ByteArraySpanTest.cpp
)

mdt_add_test(
  NAME FileMapperTest
  TARGET fileMapperTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileMapperTest.cpp
)

mdt_add_test(
  NAME ExecutableFileReaderUtilsImplTest
  TARGET executableFileReaderUtilsImplTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ExecutableFileReaderUtilsImplTest.cpp
)

mdt_add_test(
  NAME ExecutableFileReaderUtilsImplErrorTest
  TARGET executableFileReaderUtilsImplErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ExecutableFileReaderUtilsImplErrorTest.cpp
)

# Create a libraries to make some tests
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
# On MSVC, also check that deley load libraries works
if(MSVC)
  target_link_options(testExecutableDynamic
    PRIVATE "/DELAYLOAD:$<TARGET_FILE_NAME:Qt5::Core>"
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
  NAME ElfFileReaderImplTest
  TARGET elfFileReaderImplTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfFileReaderImplTest.cpp
)

mdt_add_test(
  NAME ElfFileReaderTest
  TARGET elfFileReaderTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfFileReaderTest.cpp
)

if( UNIX AND (NOT APPLE) )

  mdt_add_test(
    NAME ElfFileReaderTest_Unix
    TARGET elfFileReaderTest_Unix
    DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/ElfFileReaderTest_Unix.cpp
  )

  # Make shure that the test libraries will be built before the test
  add_dependencies(elfFileReaderTest_Unix
    testSharedLibrary
    testStaticLibrary
    testExecutableDynamic
#     testExecutableStatic
  )

  target_compile_definitions(elfFileReaderTest_Unix PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
  target_compile_definitions(elfFileReaderTest_Unix PRIVATE QT5_CORE_FILE_PATH="$<TARGET_FILE:Qt5::Core>")
  target_compile_definitions(elfFileReaderTest_Unix PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
  target_compile_definitions(elfFileReaderTest_Unix PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

endif()

mdt_add_test(
  NAME ElfFileReaderErrorTest
  TARGET elfFileReaderErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfFileReaderErrorTest.cpp
)


mdt_add_test(
  NAME PeFileReaderImplTest
  TARGET peFileReaderImplTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/PeFileReaderImplTest.cpp
)

mdt_add_test(
  NAME PeFileReaderTest
  TARGET peFileReaderTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/PeFileReaderTest.cpp
)

if(WIN32)

  mdt_add_test(
    NAME PeFileReaderTest_Windows
    TARGET peFileReaderTest_Windows
    DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/PeFileReaderTest_Windows.cpp
  )

  # Make shure that the test libraries will be built before the test
  add_dependencies(peFileReaderTest_Windows
    testSharedLibrary
    testStaticLibrary
    testExecutableDynamic
#     testExecutableStatic
  )

  target_compile_definitions(peFileReaderTest_Windows PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
  target_compile_definitions(peFileReaderTest_Windows PRIVATE QT5_CORE_FILE_PATH="$<TARGET_FILE:Qt5::Core>")
  target_compile_definitions(peFileReaderTest_Windows PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
  target_compile_definitions(peFileReaderTest_Windows PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

endif()

mdt_add_test(
  NAME PeFileReaderErrorTest
  TARGET peFileReaderErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/PeFileReaderErrorTest.cpp
)

mdt_add_test(
  NAME ExecutableFileReaderTest
  TARGET executableFileReaderTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ExecutableFileReaderTest.cpp
)
target_compile_definitions(executableFileReaderTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
target_compile_definitions(executableFileReaderTest PRIVATE QT5_CORE_FILE_PATH="$<TARGET_FILE:Qt5::Core>")
target_compile_definitions(executableFileReaderTest PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
target_compile_definitions(executableFileReaderTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
if(MSVC)
  target_compile_definitions(executableFileReaderTest PRIVATE COMPILER_IS_MSVC=1)
endif()

mdt_add_test(
  NAME ExecutableFileReaderErrorTest
  TARGET executableFileReaderErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ExecutableFileReaderErrorTest.cpp
)
target_compile_definitions(executableFileReaderErrorTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
target_compile_definitions(executableFileReaderErrorTest PRIVATE QT5_CORE_FILE_PATH="$<TARGET_FILE:Qt5::Core>")
target_compile_definitions(executableFileReaderErrorTest PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
target_compile_definitions(executableFileReaderErrorTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

if(UNIX)
  mdt_add_test(
    NAME ExecutableFileReaderTest_Unix
    TARGET executableFileReaderTest_Unix
    DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/ExecutableFileReaderTest_Unix.cpp
  )
  target_compile_definitions(executableFileReaderTest_Unix PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
  target_compile_definitions(executableFileReaderTest_Unix PRIVATE QT5_CORE_FILE_PATH="$<TARGET_FILE:Qt5::Core>")
  target_compile_definitions(executableFileReaderTest_Unix PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
  target_compile_definitions(executableFileReaderTest_Unix PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
endif()

mdt_add_test(
  NAME MsvcVersionTest
  TARGET msvcVersionTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/MsvcVersionTest.cpp
)

# TODO enable if
# if(MSVC)
  mdt_add_test(
    NAME CompilerFinderTest_Msvc
    TARGET compilerFinderTest_Msvc
    DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/CompilerFinderTest_Msvc.cpp
  )
  target_compile_definitions(compilerFinderTest_Msvc PRIVATE CXX_COMPILER_PATH="${CMAKE_CXX_COMPILER}")
# endif()

mdt_add_test(
  NAME BinaryDependenciesImplTest
  TARGET binaryDependenciesImplTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesImplTest.cpp
)

mdt_add_test(
  NAME BinaryDependenciesTest
  TARGET binaryDependenciesTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesTest.cpp
)
target_compile_definitions(binaryDependenciesTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
target_compile_definitions(binaryDependenciesTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
target_compile_definitions(binaryDependenciesTest PRIVATE PREFIX_PATH="${TEST_PREFIX_PATH}")

mdt_add_test(
  NAME BinaryDependenciesErrorTest
  TARGET binaryDependenciesErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/BinaryDependenciesErrorTest.cpp
)

mdt_add_test(
  NAME FileCopierTest
  TARGET fileCopierTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileCopierTest.cpp
)

mdt_add_test(
  NAME FileCopierErrorTest
  TARGET fileCopierErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileCopierErrorTest.cpp
)

mdt_add_test(
  NAME CopySharedLibrariesTargetDependsOnTest
  TARGET copySharedLibrariesTargetDependsOnTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopySharedLibrariesTargetDependsOnTest.cpp
)
target_compile_definitions(copySharedLibrariesTargetDependsOnTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
target_compile_definitions(copySharedLibrariesTargetDependsOnTest PRIVATE PREFIX_PATH="${TEST_PREFIX_PATH}")

# TODO enable if
# if(MSVC)
  mdt_add_test(
    NAME CopySharedLibrariesTargetDependsOnTest_Msvc
    TARGET copySharedLibrariesTargetDependsOnTest_Msvc
    DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/CopySharedLibrariesTargetDependsOnTest_Msvc.cpp
  )
  target_compile_definitions(copySharedLibrariesTargetDependsOnTest_Msvc PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
  target_compile_definitions(copySharedLibrariesTargetDependsOnTest_Msvc PRIVATE PREFIX_PATH="${TEST_PREFIX_PATH}")
  target_compile_definitions(copySharedLibrariesTargetDependsOnTest_Msvc PRIVATE CXX_COMPILER_PATH="${CMAKE_CXX_COMPILER}")
# endif()

mdt_add_test(
  NAME CopySharedLibrariesTargetDependsOnErrorTest
  TARGET copySharedLibrariesTargetDependsOnErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopySharedLibrariesTargetDependsOnErrorTest.cpp
)
