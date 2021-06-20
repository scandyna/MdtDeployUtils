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
#include "Mdt/DeployUtils/PeFileReader.h"
#include <QString>
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("isPeImageFile")
{
  PeFileReader reader;

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( reader.isPeImageFile() );
  }

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.isPeImageFile() );
  }
}

TEST_CASE("isExecutableOrSharedLibrary")
{
  PeFileReader reader;

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

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.isExecutableOrSharedLibrary() );
    reader.close();
  }
}

TEST_CASE("getNeededSharedLibraries")
{
  PeFileReader reader;
  QStringList libraries;

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    libraries = reader.getNeededSharedLibraries();
    REQUIRE( !libraries.isEmpty() );
    REQUIRE( containsQt5Core(libraries) );
    reader.close();
  }

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    libraries = reader.getNeededSharedLibraries();
    REQUIRE( !libraries.isEmpty() );
    REQUIRE( containsTestSharedLibrary(libraries) );
    REQUIRE( containsQt5Core(libraries) );
    reader.close();
  }
}

TEST_CASE("open_2_consecutive_files_with_1_instance")
{
  PeFileReader reader;

  /*
   * Open a big library first
   */

  reader.openFile( QString::fromLocal8Bit(QT5_CORE_FILE_PATH) );
  REQUIRE( !reader.getNeededSharedLibraries().isEmpty() );
  reader.close();

  reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
  REQUIRE( !reader.getNeededSharedLibraries().isEmpty() );
  reader.close();
}

TEST_CASE("call_many_members_on_1_instance")
{
  PeFileReader reader;

  reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
  REQUIRE( reader.isExecutableOrSharedLibrary() );
  REQUIRE( !reader.getNeededSharedLibraries().isEmpty() );
  reader.close();
}
