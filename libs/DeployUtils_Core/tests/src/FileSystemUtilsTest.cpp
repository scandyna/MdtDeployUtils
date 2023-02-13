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

TEST_CASE("relativePathToBase")
{
  using Mdt::DeployUtils::relativePathToBase;

  QString path;
  QString base;

  SECTION("/usr/lib , /usr -> lib")
  {
    path = QLatin1String("/usr/lib");
    base = QLatin1String("/usr");
    REQUIRE( relativePathToBase(path, base) == QLatin1String("lib") );
  }

  SECTION("/usr/lib , /usr/ -> lib")
  {
    path = QLatin1String("/usr/lib");
    base = QLatin1String("/usr/");
    REQUIRE( relativePathToBase(path, base) == QLatin1String("lib") );
  }

  SECTION("/usr/lib/ , /usr -> lib/")
  {
    path = QLatin1String("/usr/lib/");
    base = QLatin1String("/usr");
    REQUIRE( relativePathToBase(path, base) == QLatin1String("lib/") );
  }

  SECTION("/usr/lib/ , /usr/ -> lib/")
  {
    path = QLatin1String("/usr/lib/");
    base = QLatin1String("/usr/");
    REQUIRE( relativePathToBase(path, base) == QLatin1String("lib/") );
  }

  SECTION("/usr/lib/x86_64-linux-gnu , /usr -> lib")
  {
    path = QLatin1String("/usr/lib/x86_64-linux-gnu");
    base = QLatin1String("/usr");
    REQUIRE( relativePathToBase(path, base) == QLatin1String("lib/x86_64-linux-gnu") );
  }
}

TEST_CASE("pathIsInBase")
{
  using Mdt::DeployUtils::pathIsInBase;

  QString path;
  QString base;

  SECTION("/usr/lib is in /usr")
  {
    path = QLatin1String("/usr/lib");
    base = QLatin1String("/usr");

    REQUIRE( pathIsInBase(path, base) );
  }

  SECTION("/usr/lib is NOT in /lib")
  {
    path = QLatin1String("/usr/lib");
    base = QLatin1String("/lib");

    REQUIRE( !pathIsInBase(path, base) );
  }
}
