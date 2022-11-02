/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#include "Mdt/DeployUtils/QtSharedLibrary.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <QtGlobal>
#include <QDir>

using namespace Mdt::DeployUtils;

void setLibraryAbsoluteFilePath(QFileInfo & library, const QString & dirPath, const char * const fileName)
{
  const QString filePath = QDir::cleanPath( dirPath + QLatin1Char('/') + QLatin1String(fileName) );

  library.setFile(filePath);
}


TEST_CASE("getQtSharedLibraries_OnlyReturnsQtLibraries")
{
  QStringList allLibraries;
  QtSharedLibraryFileList qtLibraries;

  SECTION("empty list")
  {
    qtLibraries = QtSharedLibrary::getQtSharedLibraries(allLibraries);

    REQUIRE( qtLibraries.empty() );
  }

  SECTION("/opt/libQt5Core.so")
  {
    allLibraries = qStringListFromUtf8Strings({"/opt/libQt5Core.so"});

    qtLibraries = QtSharedLibrary::getQtSharedLibraries(allLibraries);

    REQUIRE( qtLibraries.size() == 1 );
    REQUIRE( qtLibraries[0].absoluteFilePath() == makeAbsolutePath("/opt/libQt5Core.so") );
  }

  SECTION("/opt/libm.so,/opt/libQt5Core.so")
  {
    allLibraries = qStringListFromUtf8Strings({"/opt/libm.so","/opt/libQt5Core.so"});

    qtLibraries = QtSharedLibrary::getQtSharedLibraries(allLibraries);

    REQUIRE( qtLibraries.size() == 1 );
    REQUIRE( qtLibraries[0].absoluteFilePath() == makeAbsolutePath("/opt/libQt5Core.so") );
  }
}
