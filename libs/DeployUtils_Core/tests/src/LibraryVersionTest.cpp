/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#include "Mdt/DeployUtils/LibraryVersion.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("Construct")
{
  SECTION("default constructed")
  {
    LibraryVersion v;
    REQUIRE( v.versionMajor() == -1 );
    REQUIRE( v.versionMinor() == -1 );
    REQUIRE( v.versionPatch() == -1 );
    REQUIRE( v.isNull() );
    REQUIRE( v.toString().isEmpty() );
  }

  SECTION("version 1.?.?")
  {
    LibraryVersion v(1);
    REQUIRE( v.versionMajor() == 1 );
    REQUIRE( v.versionMinor() == -1 );
    REQUIRE( v.versionPatch() == -1 );
    REQUIRE( !v.isNull() );
    REQUIRE( v.toString() == QLatin1String("1") );
  }

  SECTION("version 1.2.?")
  {
    LibraryVersion v(1,2);
    REQUIRE( v.versionMajor() == 1 );
    REQUIRE( v.versionMinor() == 2 );
    REQUIRE( v.versionPatch() == -1 );
    REQUIRE( !v.isNull() );
    REQUIRE( v.toString() == QLatin1String("1.2") );
  }

  SECTION("version 1.2.3")
  {
    LibraryVersion v(1,2,3);
    REQUIRE( v.versionMajor() == 1 );
    REQUIRE( v.versionMinor() == 2 );
    REQUIRE( v.versionPatch() == 3 );
    REQUIRE( !v.isNull() );
    REQUIRE( v.toString() == QLatin1String("1.2.3") );
  }
}

TEST_CASE("Version from string")
{
  SECTION("empty")
  {
    LibraryVersion v( QLatin1String("") );
    REQUIRE( v.isNull() );
    REQUIRE( v.toString().isEmpty() );
  }

  SECTION("version 1")
  {
    LibraryVersion v( QLatin1String("1") );
    REQUIRE( !v.isNull() );
    REQUIRE( v.toString() == QLatin1String("1") );
  }

  SECTION("version 1.2")
  {
    LibraryVersion v( QLatin1String("1.2") );
    REQUIRE( !v.isNull() );
    REQUIRE( v.toString() == QLatin1String("1.2") );
  }

  SECTION("version 1.2.3")
  {
    LibraryVersion v( QLatin1String("1.2.3") );
    REQUIRE( !v.isNull() );
    REQUIRE( v.toString() == QLatin1String("1.2.3") );
  }

  SECTION("version 0.0.0")
  {
    LibraryVersion v( QLatin1String("0.0.0") );
    REQUIRE( !v.isNull() );
    REQUIRE( v.toString() == QLatin1String("0.0.0") );
  }

  SECTION("a")
  {
    LibraryVersion v( QLatin1String("a") );
    REQUIRE( v.isNull() );
    REQUIRE( v.toString().isEmpty() );
  }

  SECTION("a.b")
  {
    LibraryVersion v( QLatin1String("a.b") );
    REQUIRE( v.isNull() );
    REQUIRE( v.toString().isEmpty() );
  }

  SECTION("a.b.c")
  {
    LibraryVersion v( QLatin1String("a.b.c") );
    REQUIRE( v.isNull() );
    REQUIRE( v.toString().isEmpty() );
  }
}
