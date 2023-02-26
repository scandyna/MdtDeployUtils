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
#include "Mdt/DeployUtils/RPathElf.h"
#include <QLatin1String>

using Mdt::DeployUtils::RPathEntry;
using Mdt::DeployUtils::RPath;
using Mdt::DeployUtils::RPathElf;


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

TEST_CASE("ELF_rPathEntryFromString")
{
  SECTION("/lib")
  {
    const RPathEntry entry = RPathElf::rPathEntryFromString( QLatin1String("/lib") );
    REQUIRE( entry.path() == QLatin1String("/lib") );
  }

  SECTION("lib (relative path)")
  {
    const RPathEntry entry = RPathElf::rPathEntryFromString( QLatin1String("lib") );
    REQUIRE( entry.path() == QLatin1String("lib") );
  }

  SECTION("$ORIGIN")
  {
    const RPathEntry entry = RPathElf::rPathEntryFromString( QLatin1String("$ORIGIN") );
    REQUIRE( entry.path() == QLatin1String(".") );
  }

  SECTION("${ORIGIN}")
  {
    const RPathEntry entry = RPathElf::rPathEntryFromString( QLatin1String("${ORIGIN}") );
    REQUIRE( entry.path() == QLatin1String(".") );
  }

  SECTION("ORIGIN")
  {
    const RPathEntry entry = RPathElf::rPathEntryFromString( QLatin1String("ORIGIN") );
    REQUIRE( entry.path() == QLatin1String("ORIGIN") );
  }

  SECTION("$ORIGIN/lib")
  {
    const RPathEntry entry = RPathElf::rPathEntryFromString( QLatin1String("$ORIGIN/lib") );
    REQUIRE( entry.path() == QLatin1String("lib") );
  }

  SECTION("${ORIGIN}/lib")
  {
    const RPathEntry entry = RPathElf::rPathEntryFromString( QLatin1String("${ORIGIN}/lib") );
    REQUIRE( entry.path() == QLatin1String("lib") );
  }

  SECTION("$ORIGIN/../lib")
  {
    const RPathEntry entry = RPathElf::rPathEntryFromString( QLatin1String("$ORIGIN/../lib") );
    REQUIRE( entry.path() == QLatin1String("../lib") );
  }
}

TEST_CASE("ELF_rPathFromString")
{
  RPath rpath;

  SECTION("empty")
  {
    rpath = RPathElf::rPathFromString( QString() );
    REQUIRE( rpath.isEmpty() );
  }

  SECTION("/lib")
  {
    rpath = RPathElf::rPathFromString( QLatin1String("/lib") );
    REQUIRE( rpath.entryAt(0).path() == QLatin1String("/lib") );
  }

  SECTION("lib (relative path)")
  {
    rpath = RPathElf::rPathFromString( QLatin1String("lib") );
    REQUIRE( rpath.entryAt(0).path() == QLatin1String("lib") );
  }

  SECTION("$ORIGIN")
  {
    rpath = RPathElf::rPathFromString( QLatin1String("$ORIGIN") );
    REQUIRE( rpath.entryAt(0).path() == QLatin1String(".") );
  }

  SECTION("$ORIGIN:opt/lib")
  {
    rpath = RPathElf::rPathFromString( QLatin1String("$ORIGIN:opt/lib") );
    REQUIRE( rpath.entryAt(0).path() == QLatin1String(".") );
    REQUIRE( rpath.entryAt(1).path() == QLatin1String("opt/lib") );
  }
}

TEST_CASE("ELF_rPathEntryToString")
{
  SECTION("/lib")
  {
    RPathEntry rpathEntry( QLatin1String("/lib") );

    REQUIRE( RPathElf::rPathEntryToString(rpathEntry) == QLatin1String("/lib") );
  }

  SECTION(".")
  {
    RPathEntry rpathEntry( QLatin1String(".") );

    REQUIRE( RPathElf::rPathEntryToString(rpathEntry) == QLatin1String("$ORIGIN") );
  }

  SECTION("lib")
  {
    RPathEntry rpathEntry( QLatin1String("lib") );

    REQUIRE( RPathElf::rPathEntryToString(rpathEntry) == QLatin1String("$ORIGIN/lib") );
  }

  SECTION("./lib")
  {
    RPathEntry rpathEntry( QLatin1String("./lib") );

    REQUIRE( RPathElf::rPathEntryToString(rpathEntry) == QLatin1String("$ORIGIN/lib") );
  }

  SECTION("../lib")
  {
    RPathEntry rpathEntry( QLatin1String("../lib") );

    REQUIRE( RPathElf::rPathEntryToString(rpathEntry) == QLatin1String("$ORIGIN/../lib") );
  }
}

TEST_CASE("ELF_rPathToString")
{
  RPath rpath;

  SECTION("empty")
  {
    REQUIRE( RPathElf::rPathToString(rpath).isEmpty() );
  }

  SECTION("/lib")
  {
    rpath.appendPath( QLatin1String("/lib") );

    REQUIRE( RPathElf::rPathToString(rpath) == QLatin1String("/lib") );
  }

  SECTION("/lib:opt/lib")
  {
    rpath.appendPath( QLatin1String("/lib") );
    rpath.appendPath( QLatin1String("opt/lib") );

    REQUIRE( RPathElf::rPathToString(rpath) == QLatin1String("/lib:$ORIGIN/opt/lib") );
  }

  SECTION("/lib:opt/lib:lib")
  {
    rpath.appendPath( QLatin1String("/lib") );
    rpath.appendPath( QLatin1String("opt/lib") );
    rpath.appendPath( QLatin1String("lib") );

    REQUIRE( RPathElf::rPathToString(rpath) == QLatin1String("/lib:$ORIGIN/opt/lib:$ORIGIN/lib") );
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
