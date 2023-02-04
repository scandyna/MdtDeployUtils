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
#include "Mdt/DeployUtils/QtDistributionDirectory.h"
#include "TestFileUtils.h"

using namespace Mdt::DeployUtils;

TEST_CASE("guessRootAbsolutePathFromQtSharedLibrary")
{
  SECTION("/usr/lib/x86_64-linux-gnu/libQt5Core.so")
  {
    const QFileInfo qtLibrary( QLatin1String("/usr/lib/x86_64-linux-gnu/libQt5Core.so") );

    REQUIRE( QtDistributionDirectory::guessRootAbsolutePathFromQtSharedLibrary(qtLibrary) == makeAbsolutePath("/usr") );
  }
}

TEST_CASE("guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary")
{
  SECTION("debian multi-arch")
  {
    const QFileInfo qtLibrary( QLatin1String("/usr/lib/x86_64-linux-gnu/libQt5Core.so") );

    REQUIRE(
      QtDistributionDirectory::guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary(qtLibrary)
      ==
      QLatin1String("lib/x86_64-linux-gnu")
    );
  }
}
