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

TEST_CASE("MainCommand")
{
  CommandLineParser parser;
  QStringList arguments;

  SECTION("No command provided")
  {
    arguments = qStringListFromUtf8Strings({"mdtdeployutils"});
    REQUIRE_THROWS_AS( parser.process(arguments), CommandLineParseError );
  }
}

TEST_CASE("CopySharedLibrariesTargetDependsOn")
{
  CommandLineParser parser;
  QStringList arguments = qStringListFromUtf8Strings({"mdtdeployutils","copy-shared-libraries-target-depends-on"});
  const QStringList positionalArguments = qStringListFromUtf8Strings({"/tmp/lib.so","/tmp"});
  CopySharedLibrariesTargetDependsOnRequest request;

  SECTION("Check setup")
  {
    arguments << positionalArguments;
    parser.process(arguments);
    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( !request.targetFilePath.isEmpty() );
  }

  SECTION("Specify 2x search-prefix-path-list")
  {
    arguments << qStringListFromUtf8Strings({"--search-prefix-path-list","/opt","--search-prefix-path-list","/tmp"});
    arguments << positionalArguments;
    REQUIRE_THROWS_AS( parser.process(arguments), CommandLineParseError );
  }

  SECTION("1 positional argument missing")
  {
    arguments << QLatin1String("/tmp");
    REQUIRE_THROWS_AS( parser.process(arguments), CommandLineParseError );
  }

  SECTION("More than 2 positional arguments given")
  {
    arguments << positionalArguments << QLatin1String("/opt");
    REQUIRE_THROWS_AS( parser.process(arguments), CommandLineParseError );
  }
}
