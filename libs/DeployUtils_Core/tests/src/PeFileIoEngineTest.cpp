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
#include "Mdt/DeployUtils/PeFileIoEngine.h"
#include <QString>
#include <QLatin1String>
#include <QTemporaryFile>

#include <QDebug>

using namespace Mdt::DeployUtils;

TEST_CASE("supportsPlatform")
{
    PeFileIoEngine engine;

  SECTION("PE")
  {
    const Platform platform(OperatingSystem::Windows, ExecutableFileFormat::Pe, Compiler::Gcc, ProcessorISA::X86_64);
    REQUIRE( engine.supportsPlatform(platform) );
  }

  SECTION("ELF")
  {
    const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    REQUIRE( !engine.supportsPlatform(platform) );
  }
}

TEST_CASE("open_close")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

    PeFileIoEngine engine;
  REQUIRE( !engine.isOpen() );

  SECTION("empty file")
  {
    engine.openFile( file.fileName() );
    REQUIRE( engine.isOpen() );
    engine.close();
    REQUIRE( !engine.isOpen() );
  }
}

TEST_CASE("isPeImageFile")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

    PeFileIoEngine engine;

  SECTION("empty file")
  {
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isPeImageFile() );
  }

  SECTION("text file - 3 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABC") ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isPeImageFile() );
  }

  SECTION("text file - 60 chars (0x3C)")
  {
    REQUIRE( writeTextFileUtf8( file, generateStringWithNChars(0x3c) ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isPeImageFile() );
  }

  SECTION("text file")
  {
    REQUIRE( writeTextFileUtf8( file, generateStringWithNChars(300) ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isPeImageFile() );
  }
}

TEST_CASE("isExecutableOrSharedLibrary")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

    PeFileIoEngine engine;

  SECTION("empty file")
  {
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isExecutableOrSharedLibrary() );
    engine.close();
  }

  SECTION("text file - 3 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABC") ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isExecutableOrSharedLibrary() );
    engine.close();
  }

  SECTION("text file - 4 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABCD") ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isExecutableOrSharedLibrary() );
    engine.close();
  }

  SECTION("text file - 64 chars")
  {
    REQUIRE( writeTextFileUtf8( file, generateStringWithNChars(64) ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isExecutableOrSharedLibrary() );
    engine.close();
  }
}
