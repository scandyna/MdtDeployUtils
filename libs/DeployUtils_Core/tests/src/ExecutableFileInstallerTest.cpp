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
#include "Mdt/DeployUtils/ExecutableFileInstaller.h"
#include "Mdt/DeployUtils/CopiedExecutableFile.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/ConsoleMessageLogger.h"
#include <Mdt/ExecutableFile/ExecutableFileReader.h>
#include <QTemporaryDir>
#include <QString>
#include <QLatin1String>
#include <QLatin1Char>
#include <QFileInfo>
#include <cassert>

using namespace Mdt::DeployUtils;

const QFileInfo testExecutableFilePath( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );

#ifdef Q_OS_WIN
  constexpr bool checkRPath = false;
#else
  constexpr bool checkRPath = true;
#endif


QString testExecutableDestinationPath(const QTemporaryDir & dir)
{
  return dir.path() + QLatin1Char('/') + testExecutableFilePath.fileName();
}

bool dirContainsTestExecutable(const QTemporaryDir & dir)
{
  const QFileInfo file( testExecutableDestinationPath(dir) );

  return file.exists();
}

RPath getTestExecutableRunPath(const QTemporaryDir & dir)
{
  Mdt::ExecutableFile::ExecutableFileReader reader;
  reader.openFile( testExecutableDestinationPath(dir) );

  return reader.getRunPath();
}

void setupMessageLogger(ExecutableFileInstaller & installer)
{
  MessageLogger::setBackend<ConsoleMessageLogger>();
  QObject::connect(&installer, &ExecutableFileInstaller::statusMessage, MessageLogger::info);
  QObject::connect(&installer, &ExecutableFileInstaller::verboseMessage, MessageLogger::info);
  QObject::connect(&installer, &ExecutableFileInstaller::debugMessage, MessageLogger::info);
}


TEST_CASE("overwriteBehavior")
{
  ExecutableFileInstaller installer( Platform::nativePlatform() );

  SECTION("by default overwrite behavior is Fail")
  {
    REQUIRE( installer.overwriteBehavior() == OverwriteBehavior::Fail );
  }

  SECTION("set a other overwrite behavior")
  {
    installer.setOverwriteBehavior(OverwriteBehavior::Keep);

    REQUIRE( installer.overwriteBehavior() == OverwriteBehavior::Keep );
  }
}

TEST_CASE("hasToUpdateRpath")
{
  ExecutableFileInstaller installer( Platform::nativePlatform() );

  PathList systemWideLocations{makeAbsolutePath("/usr/lib")};

  RPath rpath;
  rpath.appendPath( QLatin1String(".") );

  SECTION("copied file already has correct rpath")
  {
    const CopiedExecutableFile copiedFile = makeCopiedExecutableFileFromSourceAndDestinationPathAndRPath("/build/mylib", "/opt/mylib", rpath);

    REQUIRE( !installer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
  }

  // https://gitlab.com/scandyna/mdtdeployutils/-/issues/3
  SECTION("take in to account the source location")
  {

    SECTION("file comes from /usr/lib")
    {
      const CopiedExecutableFile copiedFile =
        makeCopiedExecutableFileFromSourceAndDestinationPath("/usr/lib/someLib", "/tmp/app/lib/someLib");

      SECTION("copied file has no rpath (must be updated)")
      {
        REQUIRE( copiedFile.sourceFileRPath().isEmpty() );

        REQUIRE( installer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
      }
    }

    SECTION("file comes from /opt/qt/lib")
    {
      const std::string sourcePath("/opt/qt/lib/someLib");
      const std::string destinationPath("/tmp/app/lib/someLib");

      SECTION("copied file has no rpath (must NOT be updated)")
      {
        const CopiedExecutableFile copiedFile =
          makeCopiedExecutableFileFromSourceAndDestinationPath(sourcePath, destinationPath);

        REQUIRE( copiedFile.sourceFileRPath().isEmpty() );

        REQUIRE( !installer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
      }

      SECTION("copied file has the correct rpath (must NOT be updated)")
      {
        const CopiedExecutableFile copiedFile =
          makeCopiedExecutableFileFromSourceAndDestinationPathAndRPath(sourcePath, destinationPath, rpath);

        REQUIRE( !installer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
      }

      SECTION("copied file has wrong rpath (must be updated)")
      {
        const CopiedExecutableFile copiedFile =
          makeCopiedExecutableFileFromSourceAndDestinationPathAndRPath(sourcePath, destinationPath, {"/build/project"});

        REQUIRE( installer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
      }
    }
  }
}

TEST_CASE("install_executable")
{
  QTemporaryDir destinationDir;
  destinationDir.setAutoRemove(true);
  REQUIRE( destinationDir.isValid() );

  MessageLogger messageLogger;
  ExecutableFileInstaller installer( Platform::nativePlatform() );
  setupMessageLogger(installer);

  const auto executable = ExecutableFileToInstall::fromFilePath(testExecutableFilePath);
  RPath installRPath;
  installRPath.appendPath( QLatin1String("../lib") );

  installer.install(executable, destinationDir.path(), installRPath);

  REQUIRE( dirContainsTestExecutable(destinationDir) );
  if(checkRPath){
    REQUIRE( getTestExecutableRunPath(destinationDir).entryAt(0).path() == QLatin1String("../lib") );
  }
}
