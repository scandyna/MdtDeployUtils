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
#include "Mdt/DeployUtils/LibraryNameExtension.h"
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;

TEST_CASE("isSharedLibraryExtension")
{
  SECTION(".so")
  {
    REQUIRE( LibraryNameExtension::isSharedLibraryExtension( QLatin1String("so") ) );
  }

  SECTION(".dylib")
  {
    REQUIRE( LibraryNameExtension::isSharedLibraryExtension( QLatin1String("dylib") ) );
  }

  SECTION(".dll")
  {
    REQUIRE( LibraryNameExtension::isSharedLibraryExtension( QLatin1String("dll") ) );
  }

  SECTION(".DLL")
  {
    REQUIRE( LibraryNameExtension::isSharedLibraryExtension( QLatin1String("DLL") ) );
  }

  SECTION(".Dll")
  {
    REQUIRE( LibraryNameExtension::isSharedLibraryExtension( QLatin1String("Dll") ) );
  }

  // As long as we not support AIX
  SECTION(".a")
  {
    REQUIRE( !LibraryNameExtension::isSharedLibraryExtension( QLatin1String("a") ) );
  }

  SECTION(".lib")
  {
    REQUIRE( !LibraryNameExtension::isSharedLibraryExtension( QLatin1String("lib") ) );
  }

  SECTION(".txt")
  {
    REQUIRE( !LibraryNameExtension::isSharedLibraryExtension( QLatin1String("txt") ) );
  }
}

TEST_CASE("Construct")
{
  SECTION("default constructed")
  {
    LibraryNameExtension extension;
    REQUIRE( extension.isNull() );
    REQUIRE( extension.name().isEmpty() );
  }
}

TEST_CASE("fromSharedLibraryExtensionName")
{
  SECTION("so")
  {
    const auto extension = LibraryNameExtension::fromSharedLibraryExtensionName( QLatin1String("so") );
    REQUIRE( !extension.isNull() );
    REQUIRE( extension.name() == QLatin1String("so") );
    REQUIRE( extension.isSo() );
  }

  SECTION("dylib")
  {
    const auto extension = LibraryNameExtension::fromSharedLibraryExtensionName( QLatin1String("dylib") );
    REQUIRE( !extension.isNull() );
    REQUIRE( extension.name() == QLatin1String("dylib") );
    REQUIRE( extension.isDylib() );
  }

  SECTION("dll")
  {
    const auto extension = LibraryNameExtension::fromSharedLibraryExtensionName( QLatin1String("dll") );
    REQUIRE( !extension.isNull() );
    REQUIRE( extension.name() == QLatin1String("dll") );
    REQUIRE( extension.isDll() );
  }

  SECTION("DLL")
  {
    const auto extension = LibraryNameExtension::fromSharedLibraryExtensionName( QLatin1String("DLL") );
    REQUIRE( !extension.isNull() );
    REQUIRE( extension.name() == QLatin1String("DLL") );
    REQUIRE( extension.isDll() );
  }

  SECTION("Dll")
  {
    const auto extension = LibraryNameExtension::fromSharedLibraryExtensionName( QLatin1String("Dll") );
    REQUIRE( !extension.isNull() );
    REQUIRE( extension.name() == QLatin1String("Dll") );
    REQUIRE( extension.isDll() );
  }
}
