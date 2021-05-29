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
#include "Mdt/DeployUtils/ElfFileReader.h"
#include <QString>
#include <QLatin1String>
#include <QTemporaryFile>
#include <cassert>

#include <QDebug>

using namespace Mdt::DeployUtils;


TEST_CASE("Sandbox")
{
  const QString filePath = QLatin1String("/home/philippe/dev/build/MdtDeployUtils/gccInstrumented/libs/DeployUtils_Core/src/libMdt0DeployUtils.so");
  qDebug() << "Is ELF " << filePath << ": " << ElfFileReader::isElfFile(filePath);

}

TEST_CASE("isElfFile")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  SECTION("empty file")
  {
    REQUIRE( !ElfFileReader::isElfFile( file.fileName() ) );
  }

  SECTION("text file - 3 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABC") ) );
    file.close();
    REQUIRE( !ElfFileReader::isElfFile( file.fileName() ) );
  }

  SECTION("text file - 4 chars")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABCD") ) );
    file.close();
    REQUIRE( !ElfFileReader::isElfFile( file.fileName() ) );
  }

  SECTION("text file")
  {
    REQUIRE( writeTextFileUtf8( file, QLatin1String("ABCDEFGHIJKLMNOPQRSTUWXYZ") ) );
    file.close();
    REQUIRE( !ElfFileReader::isElfFile( file.fileName() ) );
  }

}
