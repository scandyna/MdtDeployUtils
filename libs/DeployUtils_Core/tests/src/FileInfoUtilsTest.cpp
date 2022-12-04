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
#include "Mdt/DeployUtils/FileInfoUtils.h"
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;

TEST_CASE("fileInfoHasFileName")
{
  SECTION("empty file info")
  {
    QFileInfo fi;

    REQUIRE( !fileInfoHasFileName(fi) );
  }

  SECTION("file info with a file name")
  {
    QFileInfo fi( QString::fromLatin1("file.txt") );

    REQUIRE( fileInfoHasFileName(fi) );
  }
}

TEST_CASE("fileInfoIsAbsolutePath")
{
  SECTION("empty file info")
  {
    QFileInfo fi;

    REQUIRE( !fileInfoIsAbsolutePath(fi) );
  }

  SECTION("file name only")
  {
    QFileInfo fi( QString::fromLatin1("file.txt") );

    REQUIRE( !fileInfoIsAbsolutePath(fi) );
  }

  SECTION("relative path")
  {
    QFileInfo fi( QString::fromLatin1("opt/file.txt") );

    REQUIRE( !fileInfoIsAbsolutePath(fi) );
  }

  SECTION("absolute path")
  {
    QFileInfo fi( QString::fromLatin1("/some/path/to/file.txt") );

    REQUIRE( fileInfoIsAbsolutePath(fi) );
  }
}
