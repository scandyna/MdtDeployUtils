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
#include "Mdt/DeployUtils/DeployApplication.h"
#include "Mdt/DeployUtils/OperatingSystem.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("destinationDirectoryStructureFromRuntimeAndLibraryDestination")
{
  DeployApplicationRequest request;
  OperatingSystem os;
  DestinationDirectoryStructure structure;

  request.runtimeDestination = QLatin1String("mybin");
  request.libraryDestination = QLatin1String("mylibs");

  SECTION("Linux")
  {
    os = OperatingSystem::Linux;

    structure = DeployApplication::destinationDirectoryStructureFromRuntimeAndLibraryDestination(request, os);

    REQUIRE( structure.executablesDirectory() == QLatin1String("mybin") );
    REQUIRE( structure.sharedLibrariesDirectory() == QLatin1String("mylibs") );
    REQUIRE( !structure.qtPluginsRootDirectory().isEmpty() );
  }

  SECTION("Windows")
  {
    os = OperatingSystem::Windows;

    structure = DeployApplication::destinationDirectoryStructureFromRuntimeAndLibraryDestination(request, os);

    REQUIRE( structure.executablesDirectory() == QLatin1String("mybin") );
    REQUIRE( structure.sharedLibrariesDirectory() == QLatin1String("mybin") );
    REQUIRE( !structure.qtPluginsRootDirectory().isEmpty() );
  }
}
