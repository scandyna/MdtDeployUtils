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
#include "Mdt/DeployUtils/Impl/LibraryNameData.h"
#include "Mdt/DeployUtils/Impl/LibraryNameImpl.h"
#include "Mdt/DeployUtils/LibraryName.h"
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;
using Mdt::DeployUtils::Impl::LibraryNameData;
using Mdt::DeployUtils::Impl::LibraryNameImpl;

TEST_CASE("extractPrefix")
{
  SECTION("empty")
  {
    REQUIRE( LibraryNameImpl::extractPrefix( QLatin1String("") ).isEmpty() );
  }

  SECTION("Qt5Core")
  {
    REQUIRE( LibraryNameImpl::extractPrefix( QLatin1String("Qt5Core") ).isEmpty() );
  }

  SECTION("libQt5Core")
  {
    REQUIRE( LibraryNameImpl::extractPrefix( QLatin1String("libQt5Core") ) == QLatin1String("lib") );
  }

  SECTION("m")
  {
    REQUIRE( LibraryNameImpl::extractPrefix( QLatin1String("m") ).isEmpty() );
  }

  SECTION("libm")
  {
    REQUIRE( LibraryNameImpl::extractPrefix( QLatin1String("libm") ) == QLatin1String("lib") );
  }
}

TEST_CASE("extractName")
{
  QString fullExtension;
  const QString libPrefix = QLatin1String("lib");

  SECTION("empty")
  {
    REQUIRE( LibraryNameImpl::extractName( QString(), QString(), &fullExtension ).isEmpty() );
  }

  SECTION("Qt5Core")
  {
    REQUIRE( LibraryNameImpl::extractName( QString(), QLatin1String("Qt5Core"), &fullExtension ) == QLatin1String("Qt5Core") );
  }

  SECTION("m")
  {
    REQUIRE( LibraryNameImpl::extractName( QString(), QLatin1String("m"), &fullExtension ) == QLatin1String("m") );
  }

  SECTION("libQt5Core")
  {
    REQUIRE( LibraryNameImpl::extractName( libPrefix, QLatin1String("libQt5Core"), &fullExtension ) == QLatin1String("Qt5Core") );
  }

  SECTION("libm")
  {
    REQUIRE( LibraryNameImpl::extractName( libPrefix, QLatin1String("libm"), &fullExtension ) == QLatin1String("m") );
  }

  SECTION("Qt5Core.dll")
  {
    fullExtension = QLatin1String(".dll");
    REQUIRE( LibraryNameImpl::extractName( QString(), QLatin1String("Qt5Core.dll"), &fullExtension ) == QLatin1String("Qt5Core") );
  }

  SECTION("m.dll")
  {
    fullExtension = QLatin1String(".dll");
    REQUIRE( LibraryNameImpl::extractName( QString(), QLatin1String("m.dll"), &fullExtension ) == QLatin1String("m") );
  }
}

TEST_CASE("extractSharedLibraryExtensionAndVersion")
{
  QString fullExtension;
  std::pair<LibraryNameExtension, LibraryVersion> extensionAndVersion;

  SECTION(".so")
  {
    fullExtension = QLatin1String(".so");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("so") );
    REQUIRE( extensionAndVersion.second.isNull() );
  }

  SECTION(".dylib")
  {
    fullExtension = QLatin1String(".dylib");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("dylib") );
    REQUIRE( extensionAndVersion.second.isNull() );
  }

  SECTION(".dll")
  {
    fullExtension = QLatin1String(".dll");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("dll") );
    REQUIRE( extensionAndVersion.second.isNull() );
  }

  SECTION(".DLL")
  {
    fullExtension = QLatin1String(".DLL");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("DLL") );
    REQUIRE( extensionAndVersion.second.isNull() );
  }

  SECTION(".Dll")
  {
    fullExtension = QLatin1String(".Dll");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("Dll") );
    REQUIRE( extensionAndVersion.second.isNull() );
  }

  SECTION(".so.5")
  {
    fullExtension = QLatin1String(".so.5");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("so") );
    REQUIRE( extensionAndVersion.second.toString() == QLatin1String("5") );
  }

  SECTION(".so.5.9")
  {
    fullExtension = QLatin1String(".so.5.9");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("so") );
    REQUIRE( extensionAndVersion.second.toString() == QLatin1String("5.9") );
  }

  SECTION(".so.5.9.1")
  {
    fullExtension = QLatin1String(".so.5.9.1");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("so") );
    REQUIRE( extensionAndVersion.second.toString() == QLatin1String("5.9.1") );
  }

  SECTION(".so.123.456.7890")
  {
    fullExtension = QLatin1String(".so.123.456.7890");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("so") );
    REQUIRE( extensionAndVersion.second.toString() == QLatin1String("123.456.7890") );
  }

  SECTION(".5.dylib")
  {
    fullExtension = QLatin1String(".5.dylib");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("dylib") );
    REQUIRE( extensionAndVersion.second.toString() == QLatin1String("5") );
  }

  SECTION(".5.9.dylib")
  {
    fullExtension = QLatin1String(".5.9.dylib");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("dylib") );
    REQUIRE( extensionAndVersion.second.toString() == QLatin1String("5.9") );
  }

  SECTION(".5.9.1.dylib")
  {
    fullExtension = QLatin1String(".5.9.1.dylib");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("dylib") );
    REQUIRE( extensionAndVersion.second.toString() == QLatin1String("5.9.1") );
  }

  SECTION(".123.456.7890.dylib")
  {
    fullExtension = QLatin1String(".123.456.7890.dylib");
    extensionAndVersion = LibraryNameImpl::extractSharedLibraryExtensionAndVersion(&fullExtension);
    REQUIRE( extensionAndVersion.first.name() == QLatin1String("dylib") );
    REQUIRE( extensionAndVersion.second.toString() == QLatin1String("123.456.7890") );
  }
}

TEST_CASE("fromFullName")
{
  LibraryNameData data;

  SECTION("empty")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("") );
    REQUIRE( data.fullName.isEmpty() );
    REQUIRE( data.prefix.isEmpty() );
    REQUIRE( data.name.isEmpty() );
    REQUIRE( data.extensionAndVersion.first.isNull() );
    REQUIRE( data.extensionAndVersion.second.isNull() );
  }

  SECTION("Qt5Core")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("Qt5Core") );
    REQUIRE( data.fullName == QLatin1String("Qt5Core") );
    REQUIRE( data.prefix.isEmpty() );
    REQUIRE( data.name == QLatin1String("Qt5Core") );
    REQUIRE( data.extensionAndVersion.first.isNull() );
    REQUIRE( data.extensionAndVersion.second.isNull() );
  }

  SECTION("m")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("m") );
    REQUIRE( data.fullName == QLatin1String("m") );
    REQUIRE( data.prefix.isEmpty() );
    REQUIRE( data.name == QLatin1String("m") );
    REQUIRE( data.extensionAndVersion.first.isNull() );
    REQUIRE( data.extensionAndVersion.second.isNull() );
  }

  SECTION("libQt5Core")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("libQt5Core") );
    REQUIRE( data.fullName == QLatin1String("libQt5Core") );
    REQUIRE( data.prefix == QLatin1String("lib") );
    REQUIRE( data.name == QLatin1String("Qt5Core") );
    REQUIRE( data.extensionAndVersion.first.isNull() );
    REQUIRE( data.extensionAndVersion.second.isNull() );
  }

  SECTION("libm")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("libm") );
    REQUIRE( data.fullName == QLatin1String("libm") );
    REQUIRE( data.prefix == QLatin1String("lib") );
    REQUIRE( data.name == QLatin1String("m") );
    REQUIRE( data.extensionAndVersion.first.isNull() );
    REQUIRE( data.extensionAndVersion.second.isNull() );
  }

  SECTION("Qt5Core.dll")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("Qt5Core.dll") );
    REQUIRE( data.fullName == QLatin1String("Qt5Core.dll") );
    REQUIRE( data.prefix.isEmpty() );
    REQUIRE( data.name == QLatin1String("Qt5Core") );
    REQUIRE( data.extensionAndVersion.first.name() == QLatin1String("dll") );
    REQUIRE( data.extensionAndVersion.second.isNull() );
  }

  SECTION("m.dll")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("m.dll") );
    REQUIRE( data.fullName == QLatin1String("m.dll") );
    REQUIRE( data.prefix.isEmpty() );
    REQUIRE( data.name == QLatin1String("m") );
    REQUIRE( data.extensionAndVersion.first.name() == QLatin1String("dll") );
    REQUIRE( data.extensionAndVersion.second.isNull() );
  }

  SECTION("libm.so.123.456.7890")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("libm.so.123.456.7890") );
    REQUIRE( data.fullName == QLatin1String("libm.so.123.456.7890") );
    REQUIRE( data.prefix == QLatin1String("lib") );
    REQUIRE( data.name == QLatin1String("m") );
    REQUIRE( data.extensionAndVersion.first.name() == QLatin1String("so") );
    REQUIRE( data.extensionAndVersion.second.toString() == QLatin1String("123.456.7890") );
  }

  SECTION("libm.123.456.7890.dylib")
  {
    data = LibraryNameImpl::fromFullName( QLatin1String("libm.123.456.7890.dylib") );
    REQUIRE( data.fullName == QLatin1String("libm.123.456.7890.dylib") );
    REQUIRE( data.prefix == QLatin1String("lib") );
    REQUIRE( data.name == QLatin1String("m") );
    REQUIRE( data.extensionAndVersion.first.name() == QLatin1String("dylib") );
    REQUIRE( data.extensionAndVersion.second.toString() == QLatin1String("123.456.7890") );
  }
}

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
    REQUIRE( libraryName.extension().isNull() );
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
    REQUIRE( libraryName.extension().isNull() );
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
    REQUIRE( libraryName.extension().isNull() );
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
    REQUIRE( libraryName.extension().isSo() );
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
    REQUIRE( libraryName.extension().isSo() );
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
    REQUIRE( libraryName.extension().isSo() );
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
    REQUIRE( libraryName.extension().isSo() );
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
    REQUIRE( libraryName.extension().isDll() );
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
    REQUIRE( libraryName.extension().isSo() );
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
    REQUIRE( libraryName.extension().isSo() );
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
      REQUIRE( libraryName.extension().isDylib() );
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

TEST_CASE("nameWithoutDebugSuffix")
{
  SECTION("empty")
  {
    LibraryName libraryName( QLatin1String("") );
    REQUIRE( libraryName.nameWithoutDebugSuffix().isEmpty() );
  }

  SECTION("Qt5Core.dll")
  {
    LibraryName libraryName( QLatin1String("Qt5Core.dll") );
    REQUIRE( libraryName.nameWithoutDebugSuffix() == QLatin1String("Qt5Core") );
  }

  SECTION("Qt5Cored.dll")
  {
    LibraryName libraryName( QLatin1String("Qt5Cored.dll") );
    REQUIRE( libraryName.nameWithoutDebugSuffix() == QLatin1String("Qt5Core") );
  }

  SECTION("m.dll")
  {
    LibraryName libraryName( QLatin1String("m.dll") );
    REQUIRE( libraryName.nameWithoutDebugSuffix() == QLatin1String("m") );
  }

  SECTION("md.dll")
  {
    LibraryName libraryName( QLatin1String("md.dll") );
    REQUIRE( libraryName.nameWithoutDebugSuffix() == QLatin1String("m") );
  }

  SECTION("mD.dll")
  {
    LibraryName libraryName( QLatin1String("mD.dll") );
    REQUIRE( libraryName.nameWithoutDebugSuffix() == QLatin1String("m") );
  }
}

