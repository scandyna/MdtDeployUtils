/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "TestUtils.h"
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/ExecutableFileReader.h"
#include <QString>
#include <QTemporaryFile>

using namespace Mdt::DeployUtils;

TEST_CASE("open_close")
{
  ExecutableFileReader reader;

  SECTION("default constructed")
  {
    REQUIRE( !reader.isOpen() );
  }

  SECTION("open a executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.isOpen() );
    reader.close();
    REQUIRE( !reader.isOpen() );
  }

  SECTION("open a native executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH), Platform::nativePlatform() );
    REQUIRE( reader.isOpen() );
    reader.close();
    REQUIRE( !reader.isOpen() );
  }
}

TEST_CASE("getFilePlatform")
{
  ExecutableFileReader reader;
  const auto platform = Platform::nativePlatform();

  SECTION("executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.getFilePlatform() == platform );
  }

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( reader.getFilePlatform() == platform );
  }
}

TEST_CASE("isExecutableOrSharedLibrary")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  ExecutableFileReader reader;

  SECTION("empty file")
  {
    file.close();
    reader.openFile( file.fileName() );
    REQUIRE( !reader.isExecutableOrSharedLibrary() );
    reader.close();
  }

  SECTION("text file")
  {
    REQUIRE( writeTextFileUtf8( file, generateStringWithNChars(100) ) );
    file.close();
    reader.openFile( file.fileName() );
    REQUIRE( !reader.isExecutableOrSharedLibrary() );
    reader.close();
  }

  SECTION("executable file")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.isExecutableOrSharedLibrary() );
    reader.close();
  }

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( reader.isExecutableOrSharedLibrary() );
    reader.close();
  }

  SECTION("static library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_STATIC_LIBRARY_FILE_PATH) );
    REQUIRE( !reader.isExecutableOrSharedLibrary() );
    reader.close();
  }
}

/*
 * MSVC allways adds debug symbols,
 * so this test will fail for Release build
 */
#ifndef COMPILER_IS_MSVC
TEST_CASE("containsDebugSymbols")
{
  ExecutableFileReader reader;
  const bool isDebug = currentBuildType() == BuildType::Debug;

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( reader.containsDebugSymbols() == isDebug );
    reader.close();
  }

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.containsDebugSymbols() == isDebug );
    reader.close();
  }
}
#endif // #ifndef COMPILER_IS_MSVC
