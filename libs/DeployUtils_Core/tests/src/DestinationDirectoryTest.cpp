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
#include "Mdt/DeployUtils/DestinationDirectory.h"
#include "Mdt/DeployUtils/DestinationDirectoryStructure.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("fromPathAndStructure")
{
  const auto structure = DestinationDirectoryStructure::fromOperatingSystem( Platform::nativeOperatingSystem() );
  const auto destination = DestinationDirectory::fromPathAndStructure(QLatin1String("/tmp/app"), structure);

  REQUIRE( destination.path() == QLatin1String("/tmp/app") );
  REQUIRE( !destination.structure().isNull() );
}

TEST_CASE("fromPathAndOs")
{
  const auto destination = DestinationDirectory::fromPathAndOs( QLatin1String("/tmp/app"), OperatingSystem::Linux );

  REQUIRE( destination.path() == QLatin1String("/tmp/app") );
  REQUIRE( !destination.structure().isNull() );
  REQUIRE( destination.executablesDirectoryPath() == QLatin1String("/tmp/app/bin") );
  REQUIRE( destination.sharedLibrariesDirectoryPath() == QLatin1String("/tmp/app/lib") );
}
