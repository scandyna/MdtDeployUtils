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

mdt_add_test(
  NAME ByteArraySpanTest
  TARGET byteArraySpanTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ByteArraySpanTest.cpp
)

mdt_add_test(
  NAME FileMapperTest
  TARGET fileMapperTest
  DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/FileMapperTest.cpp
)

# mdt_add_test(
#   NAME ExecutableFileReaderUtilsImplTest
#   TARGET executableFileReaderUtilsImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ExecutableFileReaderUtilsImplTest.cpp
# )

mdt_add_test(
  NAME ExecutableFileReaderUtilsImplErrorTest
  TARGET executableFileReaderUtilsImplErrorTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ExecutableFileReaderUtilsImplErrorTest.cpp
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

add_executable(
  testSimpleExecutableDynamic
  src/TestSimpleExecutableDynamic.cpp
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

# mdt_add_test(
#   NAME ElfAlgorithmImplTest
#   TARGET elfAlgorithmImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfAlgorithmImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfFileHeaderWriterImplTest
#   TARGET elfFileHeaderWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfFileHeaderWriterImplTest.cpp
# )

mdt_add_test(
  NAME ElfProgramHeaderImplTest
  TARGET elfProgramHeaderImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfProgramHeaderImplTest.cpp
)

# mdt_add_test(
#   NAME ElfProgramHeaderTableImplTest
#   TARGET elfProgramHeaderTableImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfProgramHeaderTableImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfProgramHeaderReaderImplTest
#   TARGET elfProgramHeaderReaderImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfProgramHeaderReaderImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfProgramHeaderReaderImplErrorTest
#   TARGET elfProgramHeaderReaderImplErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfProgramHeaderReaderImplErrorTest.cpp
# )

# mdt_add_test(
#   NAME ElfProgramHeaderWriterImplTest
#   TARGET elfProgramHeaderWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfProgramHeaderWriterImplTest.cpp
# )

mdt_add_test(
  NAME ElfSectionHeaderImplTest
  TARGET elfSectionHeaderImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfSectionHeaderImplTest.cpp
)

mdt_add_test(
  NAME ElfSectionHeaderTableImplTest
  TARGET elfSectionHeaderTableImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfSectionHeaderTableImplTest.cpp
)

# mdt_add_test(
#   NAME ElfSectionHeaderWriterImplTest
#   TARGET elfSectionHeaderWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfSectionHeaderWriterImplTest.cpp
# )

mdt_add_test(
  NAME ElfOffsetRangeImplTest
  TARGET elfOffsetRangeImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfOffsetRangeImplTest.cpp
)

mdt_add_test(
  NAME ElfSectionSegmentUtilsImplTest
  TARGET elfSectionSegmentUtilsImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfSectionSegmentUtilsImplTest.cpp
)

# mdt_add_test(
#   NAME ElfFileAllHeadersImplTestUtilsTest
#   TARGET elfFileAllHeadersImplTestUtilsTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfFileAllHeadersImplTestUtilsTest.cpp
# )

# mdt_add_test(
#   NAME ElfFileAllHeadersImplTest
#   TARGET elfFileAllHeadersImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfFileAllHeadersImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfFileWriterFileImplTest
#   TARGET elfFileWriterFileImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfFileWriterFileImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfStringTableImplTest
#   TARGET elfStringTableImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfStringTableImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfStringTableImplErrorTest
#   TARGET elfStringTableImplErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfStringTableImplErrorTest.cpp
# )

# mdt_add_test(
#   NAME ElfStringTableWriterImplTest
#   TARGET elfStringTableWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfStringTableWriterImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfDynamicSectionImplTest
#   TARGET elfDynamicSectionImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfDynamicSectionImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfDynamicSectionImplErrorTest
#   TARGET elfDynamicSectionImplErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfDynamicSectionImplErrorTest.cpp
# )

# mdt_add_test(
#   NAME ElfDynamicSectionWriterImplTest
#   TARGET elfDynamicSectionWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfDynamicSectionWriterImplTest.cpp
# )

mdt_add_test(
  NAME ElfSymbolTableImplTest
  TARGET elfSymbolTableImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfSymbolTableImplTest.cpp
)

# mdt_add_test(
#   NAME ElfSymbolTableReaderWriterImplTest
#   TARGET elfSymbolTableReaderWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfSymbolTableReaderWriterImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfHashTableImplTest
#   TARGET elfHashTableImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfHashTableImplTest.cpp
# )

mdt_add_test(
  NAME ElfHashTableReaderWriterImplTest
  TARGET elfHashTableReaderWriterImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfHashTableReaderWriterImplTest.cpp
)

mdt_add_test(
  NAME ElfGnuHashTableImplTest
  TARGET elfGnuHashTableImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfGnuHashTableImplTest.cpp
)

# mdt_add_test(
#   NAME ElfGnuHashTableReaderWriterImplTest
#   TARGET elfGnuHashTableReaderWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfGnuHashTableReaderWriterImplTest.cpp
# )

mdt_add_test(
  NAME ElfGlobalOffsetTableImplTest
  TARGET elfGlobalOffsetTableImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfGlobalOffsetTableImplTest.cpp
)

# mdt_add_test(
#   NAME ElfGlobalOffsetTableReaderWriterImplTest
#   TARGET elfGlobalOffsetTableReaderWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfGlobalOffsetTableReaderWriterImplTest.cpp
# )

mdt_add_test(
  NAME ElfProgramInterpreterSectionImplTest
  TARGET elfProgramInterpreterSectionImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfProgramInterpreterSectionImplTest.cpp
)

# mdt_add_test(
#   NAME ElfProgramInterpreterSectionReaderWriterImplTest
#   TARGET elfProgramInterpreterSectionReaderWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfProgramInterpreterSectionReaderWriterImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfNoteSectionImplTest
#   TARGET elfNoteSectionImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfNoteSectionImplTest.cpp
# )

mdt_add_test(
  NAME ElfNoteSectionTableImplTest
  TARGET elfNoteSectionTableImplTest
  DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
  SOURCE_FILES
    src/ElfNoteSectionTableImplTest.cpp
)

# mdt_add_test(
#   NAME ElfNoteSectionReaderWriterImplTest
#   TARGET elfNoteSectionReaderWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfNoteSectionReaderWriterImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfNoteSectionReaderWriterImplErrorTest
#   TARGET elfNoteSectionReaderWriterImplErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfNoteSectionReaderWriterImplErrorTest.cpp
# )

# mdt_add_test(
#   NAME ElfFileReaderImplTest
#   TARGET elfFileReaderImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfFileReaderImplTest.cpp
# )

# mdt_add_test(
#   NAME ElfFileWriterImplTest
#   TARGET elfFileWriterImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfFileWriterImplTest.cpp
# )

if( UNIX AND (NOT APPLE) )

  # TODO: either force to be dynamicly linked, or disable for static builds

  add_executable(
    testSimpleExecutableDynamic_withRunPath
    src/TestSimpleExecutableDynamic.cpp
  )
  set_target_properties(testSimpleExecutableDynamic_withRunPath PROPERTIES BUILD_RPATH "XXXXX")

  add_executable(
    testSimpleExecutableDynamic_noRunPath
    src/TestSimpleExecutableDynamic.cpp
  )
  set_target_properties(testSimpleExecutableDynamic_noRunPath PROPERTIES SKIP_BUILD_RPATH TRUE)

  add_library(
    testSharedLibrary_noRunPath SHARED
    src/TestSharedLibrary.cpp
  )
  generate_export_header(testSharedLibrary_noRunPath BASE_NAME testSharedLibrary)
  target_include_directories(testSharedLibrary_noRunPath
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  )
  target_link_libraries(testSharedLibrary_noRunPath
    PRIVATE
      Qt5::Core
  )
  set_target_properties(testSharedLibrary_noRunPath PROPERTIES SKIP_BUILD_RPATH TRUE)

#   mdt_add_test(
#     NAME ElfFileWriterImplTest_Unix
#     TARGET elfFileWriterImplTest_Unix
#     DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#     SOURCE_FILES
#       src/ElfFileWriterImplTest_Unix.cpp
#   )

#   # Make shure that the test libraries will be built before the test
#   add_dependencies(elfFileWriterImplTest_Unix
#     testSimpleExecutableDynamic
#     testSimpleExecutableDynamic_withRunPath
#     testSimpleExecutableDynamic_noRunPath
#     testSharedLibrary
#     testSharedLibrary_noRunPath
#   )

#   target_compile_definitions(elfFileWriterImplTest_Unix
#     PRIVATE
#       TEST_SIMPLE_EXECUTABLE_DYNAMIC_FILE_PATH="$<TARGET_FILE:testSimpleExecutableDynamic>"
#       TEST_SIMPLE_EXECUTABLE_DYNAMIC_WITH_RUNPATH_FILE_PATH="$<TARGET_FILE:testSimpleExecutableDynamic_withRunPath>"
#       TEST_SIMPLE_EXECUTABLE_DYNAMIC_NO_RUNPATH_FILE_PATH="$<TARGET_FILE:testSimpleExecutableDynamic_noRunPath>"
#       TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>"
#       TEST_SHARED_LIBRARY_NO_RUNPATH_FILE_PATH="$<TARGET_FILE:testSharedLibrary_noRunPath>"
#   )

#   target_compile_definitions(elfFileWriterImplTest_Unix
#     PRIVATE
#       TEST_SIMPLE_EXECUTABLE_DYNAMIC_WITH_RUNPATH_FILE_PATH="$<TARGET_FILE:testSimpleExecutableDynamic_withRunPath>"
#   )
# 
#   target_compile_definitions(elfFileWriterImplTest_Unix
#     PRIVATE
#       TEST_SIMPLE_EXECUTABLE_DYNAMIC_NO_RUNPATH_FILE_PATH="$<TARGET_FILE:testSimpleExecutableDynamic_noRunPath>"
#   )

endif()

# mdt_add_test(
#   NAME ElfFileIoEngineTest
#   TARGET elfFileIoEngineTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfFileIoEngineTest.cpp
# )

if( UNIX AND (NOT APPLE) )

#   mdt_add_test(
#     NAME ElfFileIoEngineTest_Unix
#     TARGET elfFileIoEngineTest_Unix
#     DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#     SOURCE_FILES
#       src/ElfFileIoEngineTest_Unix.cpp
#   )

#   # Make shure that the test libraries will be built before the test
#   add_dependencies(elfFileIoEngineTest_Unix
#     testSharedLibrary
#     testStaticLibrary
#     testExecutableDynamic
# #     testExecutableStatic
#   )

#   target_compile_definitions(elfFileIoEngineTest_Unix PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
#   target_compile_definitions(elfFileIoEngineTest_Unix PRIVATE QT5_CORE_FILE_PATH="${Qt5_CORE_SHARED_LIB_PATH}")
#   target_compile_definitions(elfFileIoEngineTest_Unix PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
#   target_compile_definitions(elfFileIoEngineTest_Unix PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

endif()

# mdt_add_test(
#   NAME ElfFileIoEngineErrorTest
#   TARGET elfFileIoEngineErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ElfFileIoEngineErrorTest.cpp
# )


# mdt_add_test(
#   NAME PeFileReaderImplTest
#   TARGET peFileReaderImplTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/PeFileReaderImplTest.cpp
# )

# mdt_add_test(
#   NAME PeFileIoEngineTest
#   TARGET peFileIoEngineTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/PeFileIoEngineTest.cpp
# )

if(WIN32)

#   mdt_add_test(
#     NAME PeFileIoEngineTest_Windows
#     TARGET peFileIoEngineTest_Windows
#     DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#     SOURCE_FILES
#       src/PeFileIoEngineTest_Windows.cpp
#   )

#   # Make shure that the test libraries will be built before the test
#   add_dependencies(peFileIoEngineTest_Windows
#     testSharedLibrary
#     testStaticLibrary
#     testExecutableDynamic
# #     testExecutableStatic
#   )

#   target_compile_definitions(peFileIoEngineTest_Windows PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
#   target_compile_definitions(peFileIoEngineTest_Windows PRIVATE QT5_CORE_FILE_PATH="${Qt5_CORE_SHARED_LIB_PATH}")
#   target_compile_definitions(peFileIoEngineTest_Windows PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
#   target_compile_definitions(peFileIoEngineTest_Windows PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

endif()

# mdt_add_test(
#   NAME PeFileIoEngineErrorTest
#   TARGET peFileIoEngineErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/PeFileIoEngineErrorTest.cpp
# )

# mdt_add_test(
#   NAME ExecutableFileIoEngineTest
#   TARGET executableFileIoEngineTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ExecutableFileIoEngineTest.cpp
# )
# target_compile_definitions(executableFileIoEngineTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
# target_compile_definitions(executableFileIoEngineTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

# mdt_add_test(
#   NAME ExecutableFileIoEngineErrorTest
#   TARGET executableFileIoEngineErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ExecutableFileIoEngineErrorTest.cpp
# )
# target_compile_definitions(executableFileIoEngineErrorTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
# target_compile_definitions(executableFileIoEngineErrorTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

# mdt_add_test(
#   NAME ExecutableFileReaderTest
#   TARGET executableFileReaderTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ExecutableFileReaderTest.cpp
# )
# target_compile_definitions(executableFileReaderTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
# target_compile_definitions(executableFileReaderTest PRIVATE QT5_CORE_FILE_PATH="${Qt5_CORE_SHARED_LIB_PATH}")
# target_compile_definitions(executableFileReaderTest PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
# target_compile_definitions(executableFileReaderTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
# if(MSVC)
#   target_compile_definitions(executableFileReaderTest PRIVATE COMPILER_IS_MSVC=1)
# endif()

# mdt_add_test(
#   NAME ExecutableFileReaderErrorTest
#   TARGET executableFileReaderErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ExecutableFileReaderErrorTest.cpp
# )
# target_compile_definitions(executableFileReaderErrorTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
# target_compile_definitions(executableFileReaderErrorTest PRIVATE QT5_CORE_FILE_PATH="${Qt5_CORE_SHARED_LIB_PATH}")
# target_compile_definitions(executableFileReaderErrorTest PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
# target_compile_definitions(executableFileReaderErrorTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

# if(UNIX)
#   mdt_add_test(
#     NAME ExecutableFileReaderTest_Unix
#     TARGET executableFileReaderTest_Unix
#     DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#     SOURCE_FILES
#       src/ExecutableFileReaderTest_Unix.cpp
#   )
#   target_compile_definitions(executableFileReaderTest_Unix PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
#   target_compile_definitions(executableFileReaderTest_Unix PRIVATE QT5_CORE_FILE_PATH="${Qt5_CORE_SHARED_LIB_PATH}")
#   target_compile_definitions(executableFileReaderTest_Unix PRIVATE TEST_STATIC_LIBRARY_FILE_PATH="$<TARGET_FILE:testStaticLibrary>")
#   target_compile_definitions(executableFileReaderTest_Unix PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")
# endif()

# mdt_add_test(
#   NAME ExecutableFileWriterTest
#   TARGET executableFileWriterTest
#   DEPENDENCIES Mdt::DeployUtilsCore TestLib Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ExecutableFileWriterTest.cpp
# )
# target_compile_definitions(executableFileWriterTest PRIVATE TEST_SHARED_LIBRARY_FILE_PATH="$<TARGET_FILE:testSharedLibrary>")
# target_compile_definitions(executableFileWriterTest PRIVATE TEST_DYNAMIC_EXECUTABLE_FILE_PATH="$<TARGET_FILE:testExecutableDynamic>")

# mdt_add_test(
#   NAME ExecutableFileWriterErrorTest
#   TARGET executableFileWriterErrorTest
#   DEPENDENCIES Mdt::DeployUtilsCore Mdt::Catch2Main Mdt::Catch2Qt
#   SOURCE_FILES
#     src/ExecutableFileWriterErrorTest.cpp
# )

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
