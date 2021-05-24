##############################################################
#  Copyright Philippe Steinmann 2020 - 2020.
#  Distributed under the Boost Software License, Version 1.0.
#  (See accompanying file LICENSE.txt or copy at
#  https://www.boost.org/LICENSE_1_0.txt)
##############################################################
include(MdtAddTest)

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
  NAME ElfFileReaderTest
  TARGET elfFileReaderTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfFileReaderTest.cpp
)

# Create a shared library to make some tests
# Force this library to be a shared library
if( UNIX AND (NOT APPLE) )

  add_library(
    testSharedLibrary SHARED
    src/TestLibrary.cpp
  )
  target_link_libraries(testSharedLibrary
    PRIVATE
      Qt5::Core
  )

  # Make the test depend on the test shared library,
  # so it will be builded first
  mdt_add_test(
    NAME ElfFileReaderTest_Unix
    TARGET elfFileReaderTest_Unix
    DEPENDENCIES Mdt::DeployUtils testSharedLibrary Mdt::Catch2Main Mdt::Catch2Qt
    SOURCE_FILES
      src/ElfFileReaderTest_Unix.cpp
  )

  target_compile_definitions(elfFileReaderTest_Unix PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")

endif()



mdt_add_test(
  NAME ElfFileReaderErrorTest
  TARGET elfFileReaderErrorTest
  DEPENDENCIES Mdt::DeployUtils Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfFileReaderErrorTest.cpp
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
