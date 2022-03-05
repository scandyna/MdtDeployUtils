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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/QtModulePlugins.h"
#include <QLatin1String>
#include <QString>
#include <QTemporaryDir>

using namespace Mdt::DeployUtils;

TEST_CASE("findPluginsRootFromQtLibraryPath_pluginsDirDoesNotExist")
{
  QString qtLibraryPath;

  QTemporaryDir qtRoot;
  REQUIRE( qtRoot.isValid() );

  const QString qtLibDir = makePath(qtRoot, "lib");
  const QString qtBinDir = makePath(qtRoot, "bin");

  REQUIRE( createDirectoryFromPath(qtLibDir) );
  REQUIRE( createDirectoryFromPath(qtBinDir) );

  SECTION("QTDIR/lib/libQt5Core.so")
  {
    qtLibraryPath = qtLibDir + QLatin1String("/libQt5Core.so");

    REQUIRE_THROWS_AS( QtModulePlugins::findPluginsRootFromQtLibraryPath(qtLibraryPath), FindQtPluginError );
  }
}
