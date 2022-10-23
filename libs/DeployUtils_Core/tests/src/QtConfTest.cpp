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
#include "Mdt/DeployUtils/QtConf.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("PrefixPath")
{
  QtConf conf;

  SECTION("default")
  {
    REQUIRE( !conf.containsPrefixPath() );
  }

  SECTION("..")
  {
    conf.setPrefixPath( QLatin1String("..") );

    REQUIRE( conf.containsPrefixPath() );
    REQUIRE( conf.prefixPath() == QLatin1String("..") );
  }
}

TEST_CASE("prefixPathIsAbsolute")
{
  QtConf conf;

  SECTION("default")
  {
    REQUIRE( !conf.prefixPathIsAbsolute() );
  }

  SECTION("..")
  {
    conf.setPrefixPath( QLatin1String("..") );

    REQUIRE( !conf.prefixPathIsAbsolute() );
  }

  SECTION("/usr")
  {
    conf.setPrefixPath( QLatin1String("/usr") );

    REQUIRE( conf.prefixPathIsAbsolute() );
  }
}

TEST_CASE("Libraries path")
{
  QtConf conf;

  SECTION("default")
  {
    REQUIRE( !conf.containsLibrariesPath() );
  }

  SECTION("lib")
  {
    conf.setLibrariesPath( QLatin1String("lib") );

    REQUIRE( conf.containsLibrariesPath() );
    REQUIRE( conf.librariesPath() == QLatin1String("lib") );
  }
}

TEST_CASE("PluginsPath")
{
  QtConf conf;

  SECTION("default")
  {
    REQUIRE( !conf.containsPluginsPath() );
  }

  SECTION("plugins")
  {
    conf.setPluginsPath( QLatin1String("plugins") );

    REQUIRE( conf.containsPluginsPath() );
    REQUIRE( conf.pluginsPath() == QLatin1String("plugins") );
  }
}
