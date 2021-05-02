/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ***********************************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "TestUtils.h"
#include "CommandLineParser.h"
#include "Mdt/DeployUtils/CopySharedLibrariesTargetDependsOnRequest.h"
#include <QStringList>

using namespace Mdt::DeployUtils;

TEST_CASE("CopySharedLibrariesTargetDependsOn")
{
  CommandLineParser parser;
  QStringList arguments = qStringListFromUtf8Strings({"mdtdeployutils","copy-shared-libraries-target-depends-on"});
  CopySharedLibrariesTargetDependsOnRequest request;

  SECTION("Default options")
  {
    arguments << qStringListFromUtf8Strings({"/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( request.overwriteBehavior == OverwriteBehavior::Fail );
    REQUIRE( !request.removeRpath );
    REQUIRE( request.searchPrefixPathList.isEmpty() );
  }

  SECTION("Specify overwrite-behavior")
  {
    arguments << qStringListFromUtf8Strings({"--overwrite-behavior","overwrite","/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( request.overwriteBehavior == OverwriteBehavior::Overwrite );
  }

  SECTION("Specify to remove RPATH")
  {
    arguments << qStringListFromUtf8Strings({"--remove-rpath","/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( request.removeRpath );
  }

  SECTION("Specify search-prefix-path-list")
  {
    arguments << qStringListFromUtf8Strings({"--search-prefix-path-list","/opt;/tmp","/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    const QStringList expectedPaths = qStringListFromUtf8Strings({"/opt","/tmp"});
    REQUIRE( request.searchPrefixPathList == expectedPaths );
  }

  SECTION("Positional arguments")
  {
    arguments << qStringListFromUtf8Strings({"/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( request.targetFilePath == QLatin1String("/tmp/lib.so") );
    REQUIRE( request.destinationDirectoryPath == QLatin1String("/tmp") );
  }
}
