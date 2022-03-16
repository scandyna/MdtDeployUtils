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
#include "Mdt/DeployUtils/DestinationDirectoryStructure.h"
#include "Mdt/DeployUtils/OperatingSystem.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("executablesDirectory")
{
  DestinationDirectoryStructure structure;

  SECTION("bin")
  {
    structure.setExecutablesDirectory( QLatin1String("bin") );

    REQUIRE( structure.executablesDirectory() == QLatin1String("bin") );
  }

  SECTION("bin/")
  {
    structure.setExecutablesDirectory( QLatin1String("bin/") );

    REQUIRE( structure.executablesDirectory() == QLatin1String("bin") );
  }
}

TEST_CASE("sharedLibrariesDirectory")
{
  DestinationDirectoryStructure structure;

  SECTION("lib")
  {
    structure.setSharedLibrariesDirectory( QLatin1String("lib") );

    REQUIRE( structure.sharedLibrariesDirectory() == QLatin1String("lib") );
  }

  SECTION("lib/")
  {
    structure.setSharedLibrariesDirectory( QLatin1String("lib/") );

    REQUIRE( structure.sharedLibrariesDirectory() == QLatin1String("lib") );
  }
}

TEST_CASE("qtPluginsRootRelativePath")
{
  DestinationDirectoryStructure structure;

  SECTION("plugins")
  {
    structure.setQtPluginsRootDirectory( QLatin1String("plugins") );

    REQUIRE( structure.qtPluginsRootDirectory() == QLatin1String("plugins") );
  }

  SECTION("plugins/")
  {
    structure.setQtPluginsRootDirectory( QLatin1String("plugins/") );

    REQUIRE( structure.qtPluginsRootDirectory() == QLatin1String("plugins") );
  }
}

TEST_CASE("qtPluginsToSharedLibrariesRelativePath")
{
  DestinationDirectoryStructure structure;

  SECTION("lib , plugins")
  {
    structure.setSharedLibrariesDirectory( QLatin1String("lib") );
    structure.setQtPluginsRootDirectory( QLatin1String("plugins") );

    REQUIRE( structure.qtPluginsToSharedLibrariesRelativePath() == QLatin1String("../../lib") );
  }

  SECTION("lib/ , plugins")
  {
    structure.setSharedLibrariesDirectory( QLatin1String("lib/") );
    structure.setQtPluginsRootDirectory( QLatin1String("plugins") );

    REQUIRE( structure.qtPluginsToSharedLibrariesRelativePath() == QLatin1String("../../lib") );
  }

  SECTION("lib , fw/plugins")
  {
    structure.setSharedLibrariesDirectory( QLatin1String("lib") );
    structure.setQtPluginsRootDirectory( QLatin1String("fw/plugins") );

    REQUIRE( structure.qtPluginsToSharedLibrariesRelativePath() == QLatin1String("../../../lib") );
  }
}

TEST_CASE("isNull")
{
  DestinationDirectoryStructure structure;

  SECTION("default constructed")
  {
    REQUIRE( structure.isNull() );
  }

  SECTION("only executables dir defined")
  {
    structure.setExecutablesDirectory( QLatin1String("bin") );

    REQUIRE( structure.isNull() );
  }

  SECTION("only shared libraries dir defined")
  {
    structure.setSharedLibrariesDirectory( QLatin1String("lib") );

    REQUIRE( structure.isNull() );
  }

  SECTION("executables and shared libraries dirs defiend")
  {
    structure.setExecutablesDirectory( QLatin1String("bin") );
    structure.setSharedLibrariesDirectory( QLatin1String("lib") );

    REQUIRE( !structure.isNull() );
  }
}

TEST_CASE("fromOperatingSystem")
{
  SECTION("Linux")
  {
    const auto structure = DestinationDirectoryStructure::fromOperatingSystem(OperatingSystem::Linux);

    REQUIRE( structure.executablesDirectory() == QLatin1String("bin") );
    REQUIRE( structure.sharedLibrariesDirectory() == QLatin1String("lib") );
    REQUIRE( structure.qtPluginsRootDirectory() == QLatin1String("plugins") );
    REQUIRE( !structure.isNull() );
  }

  SECTION("Windows")
  {
    const auto structure = DestinationDirectoryStructure::fromOperatingSystem(OperatingSystem::Windows);

    REQUIRE( structure.executablesDirectory() == QLatin1String("bin") );
    REQUIRE( structure.sharedLibrariesDirectory() == QLatin1String("bin") );
    REQUIRE( structure.qtPluginsRootDirectory() == QLatin1String("bin") );
    REQUIRE( !structure.isNull() );
  }
}
