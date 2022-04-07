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
#include "Mdt/DeployUtils/DestinationDirectoryQtConf.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("setQtConfPathEntries")
{
  QtConf conf;
  DestinationDirectoryStructure destination;

  destination.setSharedLibrariesDirectory( QLatin1String("lib") );

  SECTION("exec in bin (no Qt plugins)")
  {
    destination.setExecutablesDirectory( QLatin1String("bin") );

    setQtConfPathEntries(conf, destination);

    REQUIRE( conf.prefixPath() == QLatin1String("..") );
    REQUIRE( !conf.containsPluginsPath() );
  }

  SECTION("exec in bin with Qt plugins")
  {
    destination.setExecutablesDirectory( QLatin1String("bin") );
    destination.setQtPluginsRootDirectory( QLatin1String("plugins") );

    setQtConfPathEntries(conf, destination);

    REQUIRE( conf.prefixPath() == QLatin1String("..") );
    REQUIRE( conf.pluginsPath() == QLatin1String("plugins") );
  }
}
