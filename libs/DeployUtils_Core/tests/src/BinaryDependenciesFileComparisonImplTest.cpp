// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies/FileComparison.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;
using namespace Mdt::DeployUtils::Impl::BinaryDependencies;

TEST_CASE("fileNamesAreEqual_Linux")
{
  const auto os = OperatingSystem::Linux;

  SECTION("app == app")
  {
    REQUIRE( fileNamesAreEqual( QLatin1String("app"), QLatin1String("app"), os ) );
  }

  SECTION("libA.so != libB.so")
  {
    REQUIRE( !fileNamesAreEqual( QLatin1String("libA.so"), QLatin1String("libB.so"), os ) );
  }
}

TEST_CASE("fileNamesAreEqual_Windows")
{
  const auto os = OperatingSystem::Windows;

  SECTION("app.exe == app.exe")
  {
    REQUIRE( fileNamesAreEqual( QLatin1String("app.exe"), QLatin1String("app.exe"), os ) );
  }

  SECTION("A.dll != B.dll")
  {
    REQUIRE( !fileNamesAreEqual( QLatin1String("A.dll"), QLatin1String("B.dll"), os ) );
  }

  SECTION("KERNEL32.DLL == kernel32.dll")
  {
    REQUIRE( fileNamesAreEqual( QLatin1String("KERNEL32.DLL"), QLatin1String("kernel32.dll"), os ) );
  }
}
