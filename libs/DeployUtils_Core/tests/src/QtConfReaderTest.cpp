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
#include "Mdt/DeployUtils/QtConfReader.h"
#include "TestFileUtils.h"
#include <QTemporaryDir>
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;

TEST_CASE("readFile")
{
  QTemporaryDir qtRoot;
  REQUIRE( qtRoot.isValid() );
  qtRoot.setAutoRemove(true);

  const QString qtBinDir = makePath(qtRoot, "bin");
  REQUIRE( createDirectoryFromPath(qtBinDir) );

  const QString qtConfFilePath = makePath(qtRoot, "/bin/qt.conf");

  const QString qtConfFileContents = QLatin1String("[Paths]\nPrefix = ..\nLibraries=lib/x86_64-linux-gnu\nPlugins=lib/x86_64-linux-gnu/qt5/plugins");
  REQUIRE( createTextFileUtf8(qtConfFilePath, qtConfFileContents) );

  QtConfReader reader;

  const QtConf qtConf = reader.readFile(qtConfFilePath);

  REQUIRE( qtConf.prefixPath() == QLatin1String("..") );
  REQUIRE( qtConf.librariesPath() == QLatin1String("lib/x86_64-linux-gnu") );
  REQUIRE( qtConf.pluginsPath() == QLatin1String("lib/x86_64-linux-gnu/qt5/plugins") );
}
