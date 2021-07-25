/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#include "Mdt/DeployUtils/MsvcVersion.h"

using namespace Mdt::DeployUtils;

TEST_CASE("contruct")
{
  SECTION("default constructed")
  {
    MsvcVersion v;
    REQUIRE( v.isNull() );
  }

  SECTION("from_MSC_VER")
  {
    const auto v = MsvcVersion::fromMscVer(100);
    REQUIRE( v.mscVer() == 100 );
    REQUIRE( !v.isNull() );
  }
}

TEST_CASE("toVsProductYearNumber")
{
  SECTION("1910 (Visual Studio 2017 15.0)")
  {
    const auto v = MsvcVersion::fromMscVer(1910);
    REQUIRE( v.toVsProductYearNumber() == 2017 );
  }

  SECTION("1916 (Visual Studio 2017 version 15.9)")
  {
    const auto v = MsvcVersion::fromMscVer(1916);
    REQUIRE( v.toVsProductYearNumber() == 2017 );
  }

  SECTION("1920 (Visual Studio 2019 Version 16.0)")
  {
    const auto v = MsvcVersion::fromMscVer(1920);
    REQUIRE( v.toVsProductYearNumber() == 2019 );
  }
}
