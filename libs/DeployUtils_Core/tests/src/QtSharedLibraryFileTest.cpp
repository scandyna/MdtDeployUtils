/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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
#include "Mdt/DeployUtils/QtSharedLibraryFile.h"
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;

TEST_CASE("isQtSharedLibrary")
{
  QFileInfo library;

  SECTION("empty")
  {
    REQUIRE( !QtSharedLibraryFile::isQtSharedLibrary(library) );
  }

  SECTION("libQt5Core.so")
  {
    library = QString::fromLatin1("libQt5Core.so");

    REQUIRE( QtSharedLibraryFile::isQtSharedLibrary(library) );
  }

  SECTION("Qt5Core.dll")
  {
    library = QString::fromLatin1("Qt5Core.dll");

    REQUIRE( QtSharedLibraryFile::isQtSharedLibrary(library) );
  }

  SECTION("qt5core.dll")
  {
    library = QString::fromLatin1("qt5core.dll");

    REQUIRE( QtSharedLibraryFile::isQtSharedLibrary(library) );
  }

  SECTION("QT5CORE.dll")
  {
    library = QString::fromLatin1("QT5CORE.dll");

    REQUIRE( QtSharedLibraryFile::isQtSharedLibrary(library) );
  }

  SECTION("/opt/libQt5Core.so")
  {
    library = QString::fromLatin1("/opt/libQt5Core.so");

    REQUIRE( QtSharedLibraryFile::isQtSharedLibrary(library) );
  }

  SECTION("/opt/libQt6Core.so")
  {
    library = QString::fromLatin1("/opt/libQt6Core.so");

    REQUIRE( QtSharedLibraryFile::isQtSharedLibrary(library) );
  }

  SECTION("/opt/Qt5Core.dll")
  {
    library = QString::fromLatin1("/opt/Qt5Core.dll");

    REQUIRE( QtSharedLibraryFile::isQtSharedLibrary(library) );
  }

  SECTION("/opt/Qt5Cored.dll")
  {
    library = QString::fromLatin1("/opt/Qt5Cored.dll");

    REQUIRE( QtSharedLibraryFile::isQtSharedLibrary(library) );
  }
}

TEST_CASE("fromQFileInfo")
{
  QFileInfo library;
  QtSharedLibraryFile qtLibrary;

  SECTION("/opt/libQt5Core.so")
  {
    library = QString::fromLatin1("/opt/libQt5Core.so");

    qtLibrary = QtSharedLibraryFile::fromQFileInfo(library);

    REQUIRE( qtLibrary.absoluteFilePath() == makeAbsolutePath("/opt/libQt5Core.so") );
    REQUIRE( qtLibrary.fileName() == QLatin1String("libQt5Core.so") );
  }
}

TEST_CASE("getMajorVersionStringFromFileName")
{
  QString fileName;
  QString version;

  SECTION("libQt5Core.so")
  {
    fileName = QLatin1String("libQt5Core.so");

    version = QtSharedLibraryFile::getMajorVersionStringFromFileName(fileName);

    REQUIRE( version == QLatin1String("5") );
  }

  SECTION("Qt5Core.dll")
  {
    fileName = QLatin1String("Qt5Core.dll");

    version = QtSharedLibraryFile::getMajorVersionStringFromFileName(fileName);

    REQUIRE( version == QLatin1String("5") );
  }

  SECTION("Qt5")
  {
    fileName = QLatin1String("Qt5");

    version = QtSharedLibraryFile::getMajorVersionStringFromFileName(fileName);

    REQUIRE( version == QLatin1String("5") );
  }

  SECTION("libQt12Core.so")
  {
    fileName = QLatin1String("libQt12Core.so");

    version = QtSharedLibraryFile::getMajorVersionStringFromFileName(fileName);

    REQUIRE( version == QLatin1String("12") );
  }
}
