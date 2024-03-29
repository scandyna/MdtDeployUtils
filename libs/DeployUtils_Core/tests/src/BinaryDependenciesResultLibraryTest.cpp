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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/BinaryDependenciesResultLibrary.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include <QLatin1String>
#include <QFileInfo>

using namespace Mdt::DeployUtils;

TEST_CASE("fromQFileInfo")
{
  SECTION("file with its full path (is found)")
  {
    const QFileInfo fi( QLatin1String("/opt/lib/arbitraryFile.so") );

    const auto resultLibrary = BinaryDependenciesResultLibrary::fromQFileInfo(fi);

    REQUIRE( resultLibrary.libraryName() == QLatin1String("arbitraryFile.so") );
    REQUIRE( resultLibrary.isFound() );
    REQUIRE( !resultLibrary.isMissing() );
    REQUIRE( resultLibrary.absoluteFilePath() == makeAbsolutePath("/opt/lib/arbitraryFile.so") );
    REQUIRE( resultLibrary.isToRedistribute() );
  }

  SECTION("file with library name only (is NOT found)")
  {
    const QFileInfo fi( QLatin1String("arbitraryFile.so") );

    const auto resultLibrary = BinaryDependenciesResultLibrary::fromQFileInfo(fi);

    REQUIRE( resultLibrary.libraryName() == QLatin1String("arbitraryFile.so") );
    REQUIRE( !resultLibrary.isFound() );
    REQUIRE( resultLibrary.isMissing() );
    REQUIRE( !resultLibrary.isToRedistribute() );
  }
}

TEST_CASE("fromFoundLibrary")
{
  RPath rpath;
  rpath.appendPath( QLatin1String("/opt/lib") );

  const QFileInfo fi( QLatin1String("/opt/lib/arbitraryFile.so") );

  const auto resultLibrary = BinaryDependenciesResultLibrary::fromFoundLibrary(fi, rpath);

  REQUIRE( resultLibrary.libraryName() == QLatin1String("arbitraryFile.so") );
  REQUIRE( resultLibrary.isFound() );
  REQUIRE( !resultLibrary.isMissing() );
  REQUIRE( resultLibrary.absoluteFilePath() == makeAbsolutePath("/opt/lib/arbitraryFile.so") );
  REQUIRE( resultLibrary.rPath() == rpath );
  REQUIRE( resultLibrary.isToRedistribute() );
}

TEST_CASE("libraryToNotRedistrbuteFromFileInfo")
{
  const QFileInfo fi( QLatin1String("arbitraryFile.so") );

  const auto resultLibrary = BinaryDependenciesResultLibrary::libraryToNotRedistrbuteFromFileInfo(fi);

  REQUIRE( resultLibrary.libraryName() == QLatin1String("arbitraryFile.so") );
  REQUIRE( resultLibrary.shouldNotBeRedistributed() );
  REQUIRE( !resultLibrary.isToRedistribute() );
  REQUIRE( !resultLibrary.isMissing() );
}
