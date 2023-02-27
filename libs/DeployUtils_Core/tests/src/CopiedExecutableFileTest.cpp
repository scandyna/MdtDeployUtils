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
#include "TestFileUtils.h"
#include "CopiedExecutableFileUtils.h"
#include "Mdt/DeployUtils/CopiedExecutableFile.h"
#include "Mdt/DeployUtils/FileCopierFile.h"
#include "Mdt/DeployUtils/ExecutableFileToInstall.h"
#include "Mdt/DeployUtils/RPath.h"
#include <QFileInfo>
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;

TEST_CASE("fromCopierFileAndFileToInstall")
{
  QString sourceFilePath( QLatin1String("/build/myapp") );
  QString destinationFilePath( QLatin1String("/opt/myapp") );

  RPath rpath;
  rpath.appendPath( QLatin1String("../lib") );

  FileCopierFile copierFile;
  copierFile.setSourceFileInfo(sourceFilePath);
  copierFile.setDestinationFileInfo(destinationFilePath);
  copierFile.setAsBeenCopied();

  const auto fileToInstall = ExecutableFileToInstall::fromFilePathAndRPath(sourceFilePath, rpath);

  const auto copiedFile = CopiedExecutableFile::fromCopierFileAndFileToInstall(copierFile, fileToInstall);

  REQUIRE( copiedFile.sourceFileInfo().absoluteFilePath() == makeAbsolutePath("/build/myapp") );
  REQUIRE( copiedFile.destinationFileInfo().absoluteFilePath() == makeAbsolutePath("/opt/myapp") );
  REQUIRE( copiedFile.sourceFileRPath() == rpath );
}

// Test helper
TEST_CASE("makeCopiedExecutableFileFromDestinationPathAndRPath")
{
  const CopiedExecutableFile copiedFile = makeCopiedExecutableFileFromDestinationPathAndRPath("/opt/myapp",{"../lib"});

  REQUIRE( copiedFile.destinationFileInfo().absoluteFilePath() == makeAbsolutePath("/opt/myapp") );
  REQUIRE( copiedFile.sourceFileRPath().entryAt(0).path() == QLatin1String("../lib") );
}

// Test helper
TEST_CASE("makeCopiedExecutableFileFromSourceAndDestinationPathAndRPath")
{
  const CopiedExecutableFile copiedFile = makeCopiedExecutableFileFromSourceAndDestinationPathAndRPath("/lib/libA.so","/opt/libA.so",{"."});

  REQUIRE( copiedFile.sourceFileInfo().absoluteFilePath() == makeAbsolutePath("/lib/libA.so") );
  REQUIRE( copiedFile.destinationFileInfo().absoluteFilePath() == makeAbsolutePath("/opt/libA.so") );
  REQUIRE( copiedFile.sourceFileRPath().entryAt(0).path() == QLatin1String(".") );
}
