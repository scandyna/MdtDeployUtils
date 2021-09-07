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
#include "Mdt/DeployUtils/ExecutableFileWriter.h"
#include "Mdt/DeployUtils/ExecutableFileReader.h"
#include <QString>
#include <QStringList>
#include <QTemporaryFile>
#include <QtGlobal>

// #include <QDebug>

using namespace Mdt::DeployUtils;

QStringList getFileRunPath(const QString & filePath)
{
  ExecutableFileReader reader;
  reader.openFile(filePath);

  return reader.getRunPath();
}

TEST_CASE("open_close")
{
  ExecutableFileWriter writer;

  SECTION("default constructed")
  {
    REQUIRE( !writer.isOpen() );
  }

  SECTION("open a executable")
  {
    writer.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( writer.isOpen() );
    writer.close();
    REQUIRE( !writer.isOpen() );
  }

}

TEST_CASE("isExecutableOrSharedLibrary")
{
  QTemporaryFile file;
  REQUIRE( file.open() );

  ExecutableFileWriter writer;

  SECTION("empty file")
  {
    file.close();
    writer.openFile( file.fileName() );
    REQUIRE( !writer.isExecutableOrSharedLibrary() );
    writer.close();
  }

  SECTION("text file")
  {
    REQUIRE( writeTextFileUtf8( file, generateStringWithNChars(100) ) );
    file.close();
    writer.openFile( file.fileName() );
    REQUIRE( !writer.isExecutableOrSharedLibrary() );
    writer.close();
  }

  SECTION("executable file")
  {
    writer.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( writer.isExecutableOrSharedLibrary() );
    writer.close();
  }
}

TEST_CASE("setRunPath")
{
  QTemporaryDir dir;
  REQUIRE( dir.isValid() );
  const QString targetFilePath = makePath(dir, "targetFile");
  ExecutableFileWriter writer;
  QStringList originalRPath;
  QStringList expectedRPath;

#ifdef Q_OS_WIN
  constexpr bool checkResult = false;
#else
  constexpr bool checkResult = true;
#endif

  SECTION("executable")
  {
    REQUIRE( copyFile(QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH), targetFilePath) );
    originalRPath = getFileRunPath(targetFilePath);
    expectedRPath << dir.path() << originalRPath;
    REQUIRE(originalRPath != expectedRPath);

    writer.openFile(targetFilePath);
    writer.setRunPath(expectedRPath);
    writer.close();
    if(checkResult){
      REQUIRE( getFileRunPath(targetFilePath) == expectedRPath );
    }
    
    /** \todo Should check that the executable works after modify (not corrupted) !!!
     * - Try run
     */
    REQUIRE(false);
  }

  SECTION("shared library")
  {
    REQUIRE( copyFile(QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH), targetFilePath) );
    originalRPath = getFileRunPath(targetFilePath);

    SECTION("add temp dir to the original RunPath")
    {
      expectedRPath << dir.path() << originalRPath;
      REQUIRE(originalRPath != expectedRPath);

      writer.openFile(targetFilePath);
      writer.setRunPath(expectedRPath);
      writer.close();
      if(checkResult){
        REQUIRE( getFileRunPath(targetFilePath) == expectedRPath );
      }
    }

    SECTION("set a empty RunPath")
    {
      REQUIRE( expectedRPath.isEmpty() );
      REQUIRE(originalRPath != expectedRPath);

      writer.openFile(targetFilePath);
      writer.setRunPath(expectedRPath);
      writer.close();
      if(checkResult){
        REQUIRE( getFileRunPath(targetFilePath) == expectedRPath );
      }
    }
  }
}