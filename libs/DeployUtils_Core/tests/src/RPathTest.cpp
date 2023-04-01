/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2023 Philippe Steinmann.
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
#include "RPathUtils.h"
#include "Mdt/DeployUtils/RPath.h"
#include <QLatin1String>

using Mdt::DeployUtils::RPathEntry;
using Mdt::DeployUtils::RPath;


TEST_CASE("entry_isRelative")
{
  SECTION("opt")
  {
    RPathEntry entry( QLatin1String("opt") );

    REQUIRE( entry.isRelative() );
  }

  SECTION("./opt")
  {
    RPathEntry entry( QLatin1String("./opt") );

    REQUIRE( entry.isRelative() );
  }

  SECTION("../opt")
  {
    RPathEntry entry( QLatin1String("../opt") );

    REQUIRE( entry.isRelative() );
  }

  SECTION("/opt")
  {
    RPathEntry entry( QLatin1String("/opt") );

    REQUIRE( !entry.isRelative() );
  }
}

TEST_CASE("append_and_attributes")
{
  RPath rpath;

  SECTION("default constructed")
  {
    REQUIRE( rpath.entriesCount() == 0 );
    REQUIRE( rpath.isEmpty() );
  }

  SECTION("1 path")
  {
    rpath.appendPath( QLatin1String(".") );

    REQUIRE( rpath.entriesCount() == 1 );
    REQUIRE( !rpath.isEmpty() );
    REQUIRE( rpath.entryAt(0).path() == QLatin1String(".") );
  }
}

TEST_CASE("clear")
{
  RPath rpath;
  rpath.appendPath( QLatin1String(".") );
  REQUIRE( !rpath.isEmpty() );

  rpath.clear();

  REQUIRE( rpath.isEmpty() );
}

TEST_CASE("RPathEntry_compare_equal")
{
  SECTION("/tmp == /tmp")
  {
    RPathEntry a( QLatin1String("/tmp") );
    RPathEntry b( QLatin1String("/tmp") );

    REQUIRE( a == b );
  }

  SECTION("/tmp == /tmp/")
  {
    RPathEntry a( QLatin1String("/tmp") );
    RPathEntry b( QLatin1String("/tmp/") );

    REQUIRE( a == b );
  }

  SECTION("/tmp/ == /tmp")
  {
    RPathEntry a( QLatin1String("/tmp/") );
    RPathEntry b( QLatin1String("/tmp") );

    REQUIRE( a == b );
  }

  SECTION("/tmp != /lib")
  {
    RPathEntry a( QLatin1String("/tmp") );
    RPathEntry b( QLatin1String("/lib") );

    REQUIRE( !(a == b) );
  }
}

TEST_CASE("RPath_compare_equal")
{
  RPath a, b;

  SECTION("empty")
  {
    REQUIRE( a == b );
  }

  SECTION("/tmp == /tmp")
  {
    a.appendPath( QLatin1String("/tmp") );
    b.appendPath( QLatin1String("/tmp") );

    REQUIRE( a == b );
  }

  SECTION("/tmp != /lib")
  {
    a.appendPath( QLatin1String("/tmp") );
    b.appendPath( QLatin1String("/lib") );

    REQUIRE( !(a == b) );
  }

  SECTION("empty != /lib")
  {
    b.appendPath( QLatin1String("/lib") );

    REQUIRE( !(a == b) );
  }
}

TEST_CASE("RPath_compare_notEqual")
{
  RPath a, b;

  SECTION("/tmp == /tmp")
  {
    a.appendPath( QLatin1String("/tmp") );
    b.appendPath( QLatin1String("/tmp") );

    REQUIRE( !(a != b) );
  }

  SECTION("/tmp != /lib")
  {
    a.appendPath( QLatin1String("/tmp") );
    b.appendPath( QLatin1String("/lib") );

    REQUIRE( a != b );
  }
}

// Test utils
TEST_CASE("makeRPathFromPathList")
{
  const RPath rpath = makeRPathFromPathList({".","../lib"});

  REQUIRE( rpath.entriesCount() == 2 );
  REQUIRE( rpath.entryAt(0).path() == QLatin1String(".") );
  REQUIRE( rpath.entryAt(1).path() == QLatin1String("../lib") );
}
