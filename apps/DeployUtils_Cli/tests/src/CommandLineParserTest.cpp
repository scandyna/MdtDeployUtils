/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ***********************************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "TestUtils.h"
#include "CommandLineParser.h"
#include "Mdt/DeployUtils/CopySharedLibrariesTargetDependsOnRequest.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include <QStringList>

using namespace Mdt::DeployUtils;

TEST_CASE("Logger backend")
{
  CommandLineParser parser;
  QStringList arguments = qStringListFromUtf8Strings({"mdtdeployutils"});
  const QStringList subCommandArguments = qStringListFromUtf8Strings({"copy-shared-libraries-target-depends-on","/tmp/lib.so","/tmp"});

  SECTION("default backend")
  {
    arguments << subCommandArguments;
    parser.process(arguments);
    REQUIRE( parser.messageLoggerBackend() == MessageLoggerBackend::Console );
  }

  SECTION("CMake backend")
  {
    arguments << qStringListFromUtf8Strings({"--logger-backend","cmake"});
    arguments << subCommandArguments;
    parser.process(arguments);
    REQUIRE( parser.messageLoggerBackend() == MessageLoggerBackend::CMake );
  }
}

TEST_CASE("CopySharedLibrariesTargetDependsOn")
{
  MessageLogger messageLogger;
  CommandLineParser parser;
  QStringList arguments = qStringListFromUtf8Strings({"mdtdeployutils","copy-shared-libraries-target-depends-on"});
  CopySharedLibrariesTargetDependsOnRequest request;

  SECTION("Default options")
  {
    arguments << qStringListFromUtf8Strings({"/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( request.overwriteBehavior == OverwriteBehavior::Fail );
    REQUIRE( !request.removeRpath );
    REQUIRE( request.searchPrefixPathList.isEmpty() );
  }

  SECTION("Specify overwrite-behavior")
  {
    arguments << qStringListFromUtf8Strings({"--overwrite-behavior","overwrite","/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( request.overwriteBehavior == OverwriteBehavior::Overwrite );
  }

  SECTION("Specify to remove RPATH")
  {
    arguments << qStringListFromUtf8Strings({"--remove-rpath","/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( request.removeRpath );
  }

  SECTION("Specify compiler location")
  {
    SECTION("from ENV")
    {
      arguments << qStringListFromUtf8Strings({"--compiler-location","from-env=VcInstallDir","/tmp/lib.so","/tmp"});
      parser.process(arguments);

      request = parser.copySharedLibrariesTargetDependsOnRequest();
      REQUIRE( request.compilerLocationType == CompilerLocationType::FromEnv );
      REQUIRE( request.compilerLocationValue == QLatin1String("VcInstallDir") );
    }

    SECTION("vc-install-dir")
    {
      const std::string vcInstallDir = "vc-install-dir=\"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC\"";
      const QString expectedVcInstallDirValue = QLatin1String("C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\BuildTools\\VC");
      arguments << qStringListFromUtf8Strings({"--compiler-location",vcInstallDir,"/tmp/lib.so","/tmp"});
      parser.process(arguments);

      request = parser.copySharedLibrariesTargetDependsOnRequest();
      REQUIRE( request.compilerLocationType == CompilerLocationType::VcInstallDir );
      REQUIRE( request.compilerLocationValue == expectedVcInstallDirValue );
    }

    SECTION("compiler-path")
    {
      const std::string compilerPath = "compiler-path=\"/opt/qt/Tools/mingw730_64/bin/g++\"";
      const QString expectedCompilerPath = QLatin1String("/opt/qt/Tools/mingw730_64/bin/g++");
      arguments << qStringListFromUtf8Strings({"--compiler-location",compilerPath,"/tmp/lib.so","/tmp"});
      parser.process(arguments);

      request = parser.copySharedLibrariesTargetDependsOnRequest();
      REQUIRE( request.compilerLocationType == CompilerLocationType::CompilerPath );
      REQUIRE( request.compilerLocationValue == expectedCompilerPath );
    }
  }

  SECTION("Specify search-prefix-path-list")
  {
    arguments << qStringListFromUtf8Strings({"--search-prefix-path-list","/opt;/tmp","/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    const QStringList expectedPaths = qStringListFromUtf8Strings({"/opt","/tmp"});
    REQUIRE( request.searchPrefixPathList == expectedPaths );
  }

  SECTION("Positional arguments")
  {
    arguments << qStringListFromUtf8Strings({"/tmp/lib.so","/tmp"});
    parser.process(arguments);

    request = parser.copySharedLibrariesTargetDependsOnRequest();
    REQUIRE( request.targetFilePath == QLatin1String("/tmp/lib.so") );
    REQUIRE( request.destinationDirectoryPath == QLatin1String("/tmp") );
  }
}
