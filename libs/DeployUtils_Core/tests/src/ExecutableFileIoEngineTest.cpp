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
#include "Mdt/DeployUtils/ExecutableFileIoEngine.h"

using namespace Mdt::DeployUtils;

TEST_CASE("open_close")
{
  ExecutableFileIoEngine engine;

  SECTION("default constructed")
  {
    REQUIRE( !engine.isOpen() );
  }

  SECTION("call close on a default constructed engine does nothing (and does not crach)")
  {
    engine.close();
    REQUIRE( !engine.isOpen() );
  }

  SECTION("open a executable - read only")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    REQUIRE( engine.isOpen() );
    engine.close();
    REQUIRE( !engine.isOpen() );
  }

  SECTION("open a native executable - read only")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH), ExecutableFileOpenMode::ReadOnly, Platform::nativePlatform() );
    REQUIRE( engine.isOpen() );
    engine.close();
    REQUIRE( !engine.isOpen() );
  }

}

TEST_CASE("getFilePlatform")
{
  ExecutableFileIoEngine engine;
  const auto platform = Platform::nativePlatform();

  SECTION("executable")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    REQUIRE( engine.getFilePlatform() == platform );
  }

  SECTION("shared library")
  {
    engine.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH), ExecutableFileOpenMode::ReadOnly );
    REQUIRE( engine.getFilePlatform() == platform );
  }
}
