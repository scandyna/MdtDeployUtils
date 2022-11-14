/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QString>
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("createDirectory")
{
  FileCopier fc;
  QTemporaryDir root;
  QString path;

  REQUIRE( root.isValid() );

  SECTION("Create a directory by giving a full path")
  {
    path = makePath(root, "dirA");
    fc.createDirectory(path);
    QDir dirA(path);
    REQUIRE( dirA.absolutePath() == path );
    REQUIRE( dirA.exists() );
  }
}

TEST_CASE("overwriteBehavior")
{
  FileCopier copier;

  SECTION("Default is fail")
  {
    REQUIRE( copier.overwriteBehavior() == OverwriteBehavior::Fail );
  }

  SECTION("set / get")
  {
    copier.setOverwriteBehavior(OverwriteBehavior::Overwrite);
    REQUIRE( copier.overwriteBehavior() == OverwriteBehavior::Overwrite );
  }
}

TEST_CASE("getDestinationFilePath")
{
  const QString destinationDirectoryPath = QLatin1String("/tmp");
  QString sourceFilePath;

  SECTION("libA.so -> /tmp/libA.so")
  {
    sourceFilePath = QLatin1String("libA.so");
    REQUIRE( FileCopier::getDestinationFilePath(sourceFilePath, destinationDirectoryPath) == QLatin1String("/tmp/libA.so") );
  }

  SECTION("/opt/libA.so -> /tmp/libA.so")
  {
    sourceFilePath = QLatin1String("/opt/libA.so");
    REQUIRE( FileCopier::getDestinationFilePath(sourceFilePath, destinationDirectoryPath) == QLatin1String("/tmp/libA.so") );
  }
}

TEST_CASE("isExistingDirectory")
{
  QTemporaryDir root;
  REQUIRE( root.isValid() );

  SECTION("existing directory")
  {
    REQUIRE( FileCopier::isExistingDirectory( root.path() ) );
  }

  SECTION("non existing path")
  {
    const QString path = makePath(root, "someDir");
    REQUIRE( !FileCopier::isExistingDirectory(path) );
  }

  SECTION("existing file")
  {
    QTemporaryFile file;
    REQUIRE( file.open() );
    REQUIRE( !FileCopier::isExistingDirectory( file.fileName() ) );
  }
}

TEST_CASE("copyFile")
{
  FileCopier fc;
  FileCopierFile copierFile;
  QTemporaryDir sourceRoot;
  QTemporaryDir destinationRoot;

  REQUIRE( sourceRoot.isValid() );
  REQUIRE( destinationRoot.isValid() );

  const QString libASourceFilePath = makePath(sourceRoot, "libA.so");

  REQUIRE( createTextFileUtf8( libASourceFilePath, QLatin1String("A") ) );

  const QString destinationDirectoryPath = makePath(destinationRoot, "usr/lib");
  const QString libADestinationFilePath = fc.getDestinationFilePath(libASourceFilePath, destinationDirectoryPath);

  fc.createDirectory(destinationDirectoryPath);

  SECTION("copy libA to a empty directory")
  {
    copierFile = fc.copyFile(libASourceFilePath, destinationDirectoryPath);

    REQUIRE( fileExists(libADestinationFilePath) );
    REQUIRE( readTextFileUtf8(libADestinationFilePath) == QLatin1String("A") );
    REQUIRE( copierFile.sourceFileInfo().absoluteFilePath() == libASourceFilePath );
    REQUIRE( copierFile.destinationFileInfo().absoluteFilePath() == libADestinationFilePath );
    REQUIRE( copierFile.hasBeenCopied() );
    REQUIRE( fc.copiedFilesDestinationPathList() == QStringList({libADestinationFilePath}) );
  }

  SECTION("copy libA to itself (destination file path == source file path)")
  {
    copierFile = fc.copyFile( libASourceFilePath, sourceRoot.path() );

    REQUIRE( readTextFileUtf8(libASourceFilePath) == QLatin1String("A") );
    REQUIRE( !copierFile.hasBeenCopied() );
    REQUIRE( fc.copiedFilesDestinationPathList().isEmpty() );
  }

  SECTION("copy libA to a existing file")
  {
    REQUIRE( createTextFileUtf8( libADestinationFilePath, QLatin1String("other A") ) );

    SECTION("overwrite behavior: Keep")
    {
      fc.setOverwriteBehavior(OverwriteBehavior::Keep);
      copierFile = fc.copyFile(libASourceFilePath, destinationDirectoryPath);

      REQUIRE( readTextFileUtf8(libADestinationFilePath) == QLatin1String("other A") );
      REQUIRE( !copierFile.hasBeenCopied() );
      REQUIRE( fc.copiedFilesDestinationPathList().isEmpty() );
    }

    SECTION("overwrite behavior: Overwrite")
    {
      fc.setOverwriteBehavior(OverwriteBehavior::Overwrite);
      copierFile = fc.copyFile(libASourceFilePath, destinationDirectoryPath);

      REQUIRE( readTextFileUtf8(libADestinationFilePath) == QLatin1String("A") );
      REQUIRE( copierFile.hasBeenCopied() );
      REQUIRE( fc.copiedFilesDestinationPathList() == QStringList({libADestinationFilePath}) );
    }
  }
}
