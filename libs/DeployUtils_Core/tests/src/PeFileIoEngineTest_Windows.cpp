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
#include "Mdt/DeployUtils/PeFileIoEngine.h"
#include <QString>
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("isPeImageFile")
{
    PeFileIoEngine engine;

  SECTION("shared library")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    REQUIRE( engine.isPeImageFile() );
  }

  SECTION("dynamic linked executable")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    REQUIRE( engine.isPeImageFile() );
  }
}

TEST_CASE("isExecutableOrSharedLibrary")
{
    PeFileIoEngine engine;

  SECTION("shared library")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    REQUIRE( engine.isExecutableOrSharedLibrary() );
    engine.close();
  }

  SECTION("static library")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_STATIC_LIBRARY_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    REQUIRE( !engine.isExecutableOrSharedLibrary() );
    engine.close();
  }

  SECTION("dynamic linked executable")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    REQUIRE( engine.isExecutableOrSharedLibrary() );
    engine.close();
  }
}

TEST_CASE("getNeededSharedLibraries")
{
    PeFileIoEngine engine;
  QStringList libraries;

  SECTION("shared library")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    libraries = engine.getNeededSharedLibraries();
    REQUIRE( !libraries.isEmpty() );
    REQUIRE( containsQt5Core(libraries) );
    engine.close();
  }

  SECTION("dynamic linked executable")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    libraries = engine.getNeededSharedLibraries();
    REQUIRE( !libraries.isEmpty() );
    REQUIRE( containsTestSharedLibrary(libraries) );
    REQUIRE( containsQt5Core(libraries) );
    engine.close();
  }
}

TEST_CASE("open_2_consecutive_files_with_1_instance")
{
    PeFileIoEngine engine;

  /*
   * Open a big library first
   */

  engine.openFile( QString::fromLocal8Bit(QT5_CORE_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
  REQUIRE( !engine.getNeededSharedLibraries().isEmpty() );
  engine.close();

  engine.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
  REQUIRE( !engine.getNeededSharedLibraries().isEmpty() );
  engine.close();
}

TEST_CASE("call_many_members_on_1_instance")
{
    PeFileIoEngine engine;

  engine.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
  REQUIRE( engine.isExecutableOrSharedLibrary() );
  REQUIRE( !engine.getNeededSharedLibraries().isEmpty() );
  engine.close();
}
