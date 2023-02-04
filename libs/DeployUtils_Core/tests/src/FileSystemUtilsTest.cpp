// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "Mdt/DeployUtils/FileSystemUtils.h"
#include "TestFileUtils.h"
#include <QLatin1String>
#include <QString>

// using namespace Mdt::DeployUtils;

TEST_CASE("isAbsolutePath")
{
  using Mdt::DeployUtils::isAbsolutePath;

  SECTION("empty path")
  {
    REQUIRE( !isAbsolutePath( QString() ) );
  }

  SECTION("relative path")
  {
    REQUIRE( !isAbsolutePath( QLatin1String("opt/bin") ) );
  }

  SECTION("absolute path")
  {
    REQUIRE( isAbsolutePath( QLatin1String("/opt/bin") ) );
  }
}

TEST_CASE("makeAbsolutePath")
{
  const QString path = QLatin1String("/opt");

  REQUIRE( Mdt::DeployUtils::makeAbsolutePath(path) == makeAbsolutePath("/opt") );
}

TEST_CASE("absoluteDirectoryPathFromAbsoluteFilePath")
{
  using Mdt::DeployUtils::absoluteDirectoryPathFromAbsoluteFilePath;

  SECTION("/file.txt")
  {
    const QString path = QLatin1String("/file.txt");

    REQUIRE( absoluteDirectoryPathFromAbsoluteFilePath(path) == makeAbsolutePath("/") );
  }
}
