/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#include "Mdt/DeployUtils/FileCopier.h"
#include <QFile>
#include <QTemporaryDir>
#include <QByteArray>

using namespace Mdt::DeployUtils;

TEST_CASE("createDirectory")
{
  FileCopier fc;
  QTemporaryDir root;
  QString path;

  REQUIRE( root.isValid() );

  SECTION("Try to create a directory by giving a path to a existing file")
  {
    path = makePath(root, "fileB");

    QFile fileB(path);
    REQUIRE( fileB.open(QFile::WriteOnly) );
    REQUIRE( writeTextFileUtf8( fileB, QLatin1String("B") ) );
    fileB.close();

    REQUIRE_THROWS_AS( fc.createDirectory(path), FileCopyError );

    // Check that file was untouched
    REQUIRE( fileB.open(QFile::ReadOnly) );
    REQUIRE( fileB.readAll() == QByteArray("B") );
    fileB.close();
  }
}

TEST_CASE("copyFile")
{
  FileCopier fc;
  QTemporaryDir sourceRoot;
  QTemporaryDir destinationRoot;

  REQUIRE( sourceRoot.isValid() );
  REQUIRE( destinationRoot.isValid() );

  const QString libASourceFilePath = makePath(sourceRoot, "libA.so");

  REQUIRE( createTextFileUtf8( libASourceFilePath, QLatin1String("A") ) );

  const QString destinationDirectoryPath = makePath(destinationRoot, "usr/lib");
  const QString libADestinationFilePath = fc.getDestinationFilePath(libASourceFilePath, destinationDirectoryPath);

  fc.createDirectory(destinationDirectoryPath);

  SECTION("copy libA to a existing file")
  {
    REQUIRE( createTextFileUtf8( libADestinationFilePath, QLatin1String("other A") ) );

    SECTION("overwrite behavior: Fail")
    {
      fc.setOverwriteBehavior(OverwriteBehavior::Fail);
      REQUIRE_THROWS_AS( fc.copyFile(libASourceFilePath, destinationDirectoryPath), FileCopyError );
      REQUIRE( readTextFileUtf8(libADestinationFilePath) == QLatin1String("other A") );
    }
  }
}
