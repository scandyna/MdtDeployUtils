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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/ElfFileIoEngine.h"
#include <QString>
#include <QLatin1String>
#include <QTemporaryFile>
#include <cassert>

using namespace Mdt::DeployUtils;

TEST_CASE("supportsPlatform")
{
  ElfFileIoEngine engine;

  SECTION("ELF")
  {
    const Platform platform(OperatingSystem::Linux, ExecutableFileFormat::Elf, Compiler::Gcc, ProcessorISA::X86_64);
    REQUIRE( engine.supportsPlatform(platform) );
  }

  SECTION("PE")
  {
    const Platform platform(OperatingSystem::Windows, ExecutableFileFormat::Pe, Compiler::Gcc, ProcessorISA::X86_64);
    REQUIRE( !engine.supportsPlatform(platform) );
  }
}

TEST_CASE("open_close")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  ElfFileIoEngine engine;
  REQUIRE( !engine.isOpen() );

  SECTION("empty file")
  {
    engine.openFile( file.fileName() );
    REQUIRE( engine.isOpen() );
    engine.close();
    REQUIRE( !engine.isOpen() );
  }
}

TEST_CASE("isElfFile")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  ElfFileIoEngine engine;

  SECTION("empty file")
  {
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isElfFile() );
  }

  SECTION("text file - 3 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABC") ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isElfFile() );
  }

  SECTION("text file - 4 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABCD") ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isElfFile() );
  }

  SECTION("text file")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABCDEFGHIJKLMNOPQRSTUWXYZ") ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isElfFile() );
  }
}

TEST_CASE("isExecutableOrSharedLibrary")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  ElfFileIoEngine engine;

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
    const QString text = QLatin1String(
      "0123456789"
      "0123456789"
      "0123456789"
      "0123456789"
      "0123456789"
      "0123456789"
      "1234"
    );
    REQUIRE( writeTextFileUtf8( file, text ) );
    file.close();
    engine.openFile( file.fileName() );
    REQUIRE( !engine.isExecutableOrSharedLibrary() );
    engine.close();
  }
}
