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
#include "Mdt/DeployUtils/LogLevel.h"

using namespace Mdt::DeployUtils;

TEST_CASE("shouldOutputStatusMessages")
{
  SECTION("Error level")
  {
    REQUIRE( !shouldOutputStatusMessages(LogLevel::Error) );
  }

  SECTION("Status level")
  {
    REQUIRE( shouldOutputStatusMessages(LogLevel::Status) );
  }

  SECTION("Verbose level")
  {
    REQUIRE( shouldOutputStatusMessages(LogLevel::Verbose) );
  }

  SECTION("Debug level")
  {
    REQUIRE( shouldOutputStatusMessages(LogLevel::Debug) );
  }
}

TEST_CASE("shouldOutputVerboseMessages")
{
  SECTION("Error level")
  {
    REQUIRE( !shouldOutputVerboseMessages(LogLevel::Error) );
  }

  SECTION("Status level")
  {
    REQUIRE( !shouldOutputVerboseMessages(LogLevel::Status) );
  }

  SECTION("Verbose level")
  {
    REQUIRE( shouldOutputVerboseMessages(LogLevel::Verbose) );
  }

  SECTION("Debug level")
  {
    REQUIRE( shouldOutputVerboseMessages(LogLevel::Debug) );
  }
}

TEST_CASE("shouldOutputDebugMessages")
{
  SECTION("Error level")
  {
    REQUIRE( !shouldOutputDebugMessages(LogLevel::Error) );
  }

  SECTION("Status level")
  {
    REQUIRE( !shouldOutputDebugMessages(LogLevel::Status) );
  }

  SECTION("Verbose level")
  {
    REQUIRE( !shouldOutputDebugMessages(LogLevel::Verbose) );
  }

  SECTION("Debug level")
  {
    REQUIRE( shouldOutputDebugMessages(LogLevel::Debug) );
  }
}
