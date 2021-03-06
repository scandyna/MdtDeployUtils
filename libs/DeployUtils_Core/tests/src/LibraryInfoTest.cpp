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
#include "Mdt/DeployUtils/LibraryInfo.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("Construct")
{
  SECTION("default constructed")
  {
    LibraryInfo libraryInfo;
    REQUIRE( libraryInfo.isNull() );
  }
}

TEST_CASE("fromFile")
{
  LibraryInfo libraryInfo;

  SECTION("libQt5Core.so")
  {
    libraryInfo = LibraryInfo::fromFile( QLatin1String("libQt5Core.so") );
    
  }

  REQUIRE(false);
}

TEST_CASE("isSharedLibraryFile")
{
  REQUIRE(false);
}

// TEST_CASE("libraryNameFromFile")
// {
//   REQUIRE(false);
// }
