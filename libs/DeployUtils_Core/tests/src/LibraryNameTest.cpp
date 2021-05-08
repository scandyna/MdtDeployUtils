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
#include "Mdt/DeployUtils/LibraryName.h"
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;

TEST_CASE("FromString")
{
  QString inputName;

  SECTION("empty")
  {
    LibraryName libraryName( QLatin1String("") );
    REQUIRE( libraryName.isNull() );
    REQUIRE( libraryName.fullName().isEmpty() );
    REQUIRE( libraryName.prefix().isEmpty() );
    REQUIRE( libraryName.name().isEmpty() );
    REQUIRE( libraryName.extension().isEmpty() );
    REQUIRE( libraryName.version().toString().isEmpty() );
  }

  SECTION("Qt5Core")
  {
    inputName = QLatin1String("Qt5Core");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix().isEmpty() );
    REQUIRE( libraryName.name() == QLatin1String("Qt5Core") );
    REQUIRE( libraryName.extension().isEmpty() );
    REQUIRE( libraryName.version().toString().isEmpty() );
  }

  SECTION("libQt5Core")
  {
    inputName = QLatin1String("libQt5Core");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix() == QLatin1String("lib") );
    REQUIRE( libraryName.name() == QLatin1String("Qt5Core") );
    REQUIRE( libraryName.extension().isEmpty() );
    REQUIRE( libraryName.version().toString().isEmpty() );
  }

  SECTION("libQt5Core.so")
  {
    inputName = QLatin1String("libQt5Core.so");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix() == QLatin1String("lib") );
    REQUIRE( libraryName.name() == QLatin1String("Qt5Core") );
    REQUIRE( libraryName.extension() == QLatin1String("so") );
    REQUIRE( libraryName.version().toString().isEmpty() );
  }

  SECTION("libQt5Core.so.5")
  {
    inputName = QLatin1String("libQt5Core.so.5");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix() == QLatin1String("lib") );
    REQUIRE( libraryName.name() == QLatin1String("Qt5Core") );
    REQUIRE( libraryName.extension() == QLatin1String("so") );
    REQUIRE( libraryName.version().toString() == QLatin1String("5") );
  }

  SECTION("libQt5Core.so.5.6")
  {
    inputName = QLatin1String("libQt5Core.so.5.6");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix() == QLatin1String("lib") );
    REQUIRE( libraryName.name() == QLatin1String("Qt5Core") );
    REQUIRE( libraryName.extension() == QLatin1String("so") );
    REQUIRE( libraryName.version().toString() == QLatin1String("5.6") );
  }

  SECTION("libQt5Core.so.5.6.2")
  {
    inputName = QLatin1String("libQt5Core.so.5.6.2");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix() == QLatin1String("lib") );
    REQUIRE( libraryName.name() == QLatin1String("Qt5Core") );
    REQUIRE( libraryName.extension() == QLatin1String("so") );
    REQUIRE( libraryName.version().toString() == QLatin1String("5.6.2") );
  }

  SECTION("Qt5Core.dll")
  {
    inputName = QLatin1String("Qt5Core.dll");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix().isEmpty() );
    REQUIRE( libraryName.name() == QLatin1String("Qt5Core") );
    REQUIRE( libraryName.extension() == QLatin1String("dll") );
    REQUIRE( libraryName.version().toString().isEmpty() );
  }

  SECTION("libpng12.so")
  {
    inputName = QLatin1String("libpng12.so");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix() == QLatin1String("lib") );
    REQUIRE( libraryName.name() == QLatin1String("png12") );
    REQUIRE( libraryName.extension() == QLatin1String("so") );
    REQUIRE( libraryName.version().toString().isEmpty() );
  }

  SECTION("libpng12.so.0")
  {
    inputName = QLatin1String("libpng12.so.0");

    LibraryName libraryName(inputName);
    REQUIRE( !libraryName.isNull() );
    REQUIRE( libraryName.fullName() == inputName );
    REQUIRE( libraryName.prefix() == QLatin1String("lib") );
    REQUIRE( libraryName.name() == QLatin1String("png12") );
    REQUIRE( libraryName.extension() == QLatin1String("so") );
    REQUIRE( libraryName.version().toString() == QLatin1String("0") );
  }

  SECTION("OSX")
  {
    SECTION("libQt5Core.dylib")
    {
      inputName = QLatin1String("libQt5Core.dylib");

      LibraryName libraryName(inputName);
      REQUIRE( !libraryName.isNull() );
      REQUIRE( libraryName.fullName() == inputName );
      REQUIRE( libraryName.prefix() == QLatin1String("lib") );
      REQUIRE( libraryName.name() == QLatin1String("Qt5Core") );
      REQUIRE( libraryName.extension() == QLatin1String("dylib") );
      REQUIRE( libraryName.version().toString().isEmpty() );
    }

  }
}

TEST_CASE("debugSuffix")
{
  SECTION("empty")
  {
    LibraryName libraryName( QLatin1String("") );
    REQUIRE( !libraryName.hasNameDebugSuffix() );
  }

  SECTION("Qt5Core.dll")
  {
    LibraryName libraryName( QLatin1String("Qt5Core.dll") );
    REQUIRE( !libraryName.hasNameDebugSuffix() );
  }

  SECTION("Qt5Cored.dll")
  {
    LibraryName libraryName( QLatin1String("Qt5Cored.dll") );
    REQUIRE( libraryName.hasNameDebugSuffix() );
  }

  SECTION("liba.dll")
  {
    LibraryName libraryName( QLatin1String("liba.dll") );
    REQUIRE( !libraryName.hasNameDebugSuffix() );
  }

  SECTION("libaD.dll")
  {
    LibraryName libraryName( QLatin1String("libaD.dll") );
    REQUIRE( libraryName.hasNameDebugSuffix() );
  }
}

TEST_CASE("toFullNameLinux")
{
  SECTION("No version")
  {
    LibraryName libraryName( QLatin1String("Qt5Core") );
    REQUIRE( libraryName.toFullNameLinux() == QLatin1String("libQt5Core.so") );
  }

  SECTION("libQt5Core.so.1.2.3")
  {
    LibraryName libraryName( QLatin1String("libQt5Core.so.1.2.3") );
    REQUIRE( libraryName.toFullNameLinux() == QLatin1String("libQt5Core.so.1.2.3") );
  }
}

TEST_CASE("operatingSystemFromLibraryName")
{
  SECTION("empty")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("") ) == OperatingSystem::Unknown );
  }

  SECTION("A")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("A") ) == OperatingSystem::Unknown );
  }

  SECTION("A.so")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("A.so") ) == OperatingSystem::Linux );
  }

  SECTION("A.so.1")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("A.so.1") ) == OperatingSystem::Linux );
  }

  SECTION("A.so.1.2")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("A.so.1.2") ) == OperatingSystem::Linux );
  }

  SECTION("A.so.1.2.3")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("A.so.1.2.3") ) == OperatingSystem::Linux );
  }

  SECTION("A.dll")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("A.dll") ) == OperatingSystem::Windows );
  }

  SECTION("A.DLL")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("A.DLL") ) == OperatingSystem::Windows );
  }

  SECTION("A.Dll")
  {
    REQUIRE( LibraryName::operatingSystem( QLatin1String("A.Dll") ) == OperatingSystem::Windows );
  }

  SECTION("OSX")
  {
    REQUIRE(false);
  }
}
