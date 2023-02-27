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
#include "Mdt/DeployUtils/ExecutableFileToInstall.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("fromFilePath")
{
  const QFileInfo file( QLatin1String("/some/path/myapp") );

  const auto fileToInstall = ExecutableFileToInstall::fromFilePath(file);

  REQUIRE( fileToInstall.fileInfo() == file );
  REQUIRE( fileToInstall.haveToReadRPathFromFile() );
}

TEST_CASE("fromFilePathAndRPath")
{
  const QFileInfo file( QLatin1String("/some/path/myapp") );
  RPath rpath;
  rpath.appendPath( QLatin1String("/opt") );

  const auto fileToInstall = ExecutableFileToInstall::fromFilePathAndRPath(file, rpath);

  REQUIRE( fileToInstall.fileInfo() == file );
  REQUIRE( !fileToInstall.haveToReadRPathFromFile() );
  REQUIRE( fileToInstall.rPath() == rpath );
}
