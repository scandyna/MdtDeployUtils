##############################################################
#  Copyright Philippe Steinmann 2020 - 2020.
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt)
##############################################################
include(MdtAddTest)
include(GenerateExportHeader)

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

# TODO also build test libs with delay load DLL (probably only supported by MSVC ??)
# see https://gitlab.kitware.com/cmake/cmake/-/issues/12372

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
)

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

# TODO re-enable if(WIN32)
# if(WIN32)

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

# endif()

mdt_add_test(
  NAME PeFileReaderErrorTest
  TARGET peFileReaderErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/PeFileReaderErrorTest.cpp
)

# mdt_add_test(
#   NAME BinaryDependenciesTest
#   TARGET binaryDependenciesTest
#   DEPENDENCIES Mdt::DeployUtils Qt5::Test
#   SOURCE_FILES
#     src/BinaryDependenciesTest.cpp
# )
# target_compile_definitions(binaryDependenciesTest PRIVATE PREFIX_PATH="${CMAKE_PREFIX_PATH}")

mdt_add_test(
  NAME CopySharedLibrariesTargetDependsOnTest
  TARGET copySharedLibrariesTargetDependsOnTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopySharedLibrariesTargetDependsOnTest.cpp
)

mdt_add_test(
  NAME CopySharedLibrariesTargetDependsOnErrorTest
  TARGET copySharedLibrariesTargetDependsOnErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/CopySharedLibrariesTargetDependsOnErrorTest.cpp
)
