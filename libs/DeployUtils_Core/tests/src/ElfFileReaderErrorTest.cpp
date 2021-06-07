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
#include "ElfFileReaderTestUtils.h"
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/ElfFileReader.h"
#include "Mdt/DeployUtils/Impl/Elf/FileReader.h"
#include <QTemporaryDir>
#include <QDir>
#include <QTemporaryFile>
#include <QString>
#include <QLatin1String>
#include <cassert>

using namespace Mdt::DeployUtils;

QString makeFilePath(const QTemporaryDir & dir, const QString & fileName)
{
  assert( dir.isValid() );

  return QDir::cleanPath( dir.path() + QLatin1Char('/') + fileName );
}


TEST_CASE("stringFromUnsignedCharArray")
{
  using Impl::Elf::stringFromUnsignedCharArray;
  using Impl::ByteArraySpan;


  ByteArraySpan span;

  SECTION("A (no end of string)")
  {
    const unsigned char array[1] = {'A'};
    span.data = array;
    span.size = 1;
    REQUIRE_THROWS_AS( stringFromUnsignedCharArray(span), ExecutableFileReadError );
  }
}

TEST_CASE("qStringFromUft8UnsignedCharArray")
{
  using Impl::Elf::qStringFromUft8UnsignedCharArray;
  using Impl::ByteArraySpan;

  ByteArraySpan span;

  SECTION("A (no end of string)")
  {
    const unsigned char array[1] = {'A'};
    span.data = array;
    span.size = 1;
    REQUIRE_THROWS_AS( qStringFromUft8UnsignedCharArray(span), ExecutableFileReadError );
  }
}

TEST_CASE("open_close")
{
  QString filePath;

  QTemporaryDir dir;
  REQUIRE( dir.isValid() );

  ElfFileReader reader;
  REQUIRE( !reader.isOpen() );

  SECTION("non existing file")
  {
    filePath = makeFilePath( dir, QLatin1String("nonExisting.so") );
    REQUIRE_THROWS_AS( reader.openFile(filePath), FileOpenError );
    REQUIRE( !reader.isOpen() );
  }
}

/*
 * See preconditions on ElfFileReader::getNeededSharedLibraries()
 *
TEST_CASE("getNeededSharedLibraries")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  ElfFileReader reader;

  SECTION("empty file")
  {
    file.close();
    reader.openFile( file.fileName() );
    REQUIRE_THROWS_AS( reader.getNeededSharedLibraries(), ExecutableFileReadError );
    reader.close();
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
    reader.openFile( file.fileName() );
    REQUIRE_THROWS_AS( reader.getNeededSharedLibraries(), ExecutableFileReadError );
    reader.close();
  }

  SECTION("other cases")
  {
    REQUIRE(false);
  }
}
*/
