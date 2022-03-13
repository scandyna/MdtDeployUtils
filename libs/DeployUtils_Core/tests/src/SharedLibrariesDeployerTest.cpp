/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "TestUtils.h"
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/SharedLibrariesDeployer.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/ConsoleMessageLogger.h"
#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/DeployUtils/ExecutableFileReader.h"
#include <QLatin1String>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>

using namespace Mdt::DeployUtils;

const QString testExecutableFilePath = QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH);
const QString qtCoreLibraryFilePath = QString::fromLocal8Bit(TEST_QT_CORE_FILE_PATH);
const auto searchPrefixPathList = PathList::fromStringList( getTestPrefixPath(PREFIX_PATH) );

#ifdef Q_OS_WIN
  constexpr bool checkRPath = false;
#else
  constexpr bool checkRPath = true;
#endif

RPath getTestSharedLibraryRunPath(const QTemporaryDir & dir)
{
  ExecutableFileReader reader;
  reader.openFile( makePath(dir, "libtestSharedLibrary.so") );

  return reader.getRunPath();
}


TEST_CASE("searchPrefixPathList")
{
  SharedLibrariesDeployer deployer;

  const QStringList pathList = qStringListFromUtf8Strings({"/tmp/qt"});
  deployer.setSearchPrefixPathList( PathList::fromStringList(pathList) );

  REQUIRE( deployer.searchPrefixPathList().containsPath( QLatin1String("/tmp/qt") ) );
}

TEST_CASE("overwriteBehavior")
{
  SharedLibrariesDeployer deployer;

  SECTION("by default overwrite behavior is Fail")
  {
    REQUIRE( deployer.overwriteBehavior() == OverwriteBehavior::Fail );
  }

  SECTION("set a other overwrite behavior")
  {
    deployer.setOverwriteBehavior(OverwriteBehavior::Keep);

    REQUIRE( deployer.overwriteBehavior() == OverwriteBehavior::Keep );
  }
}

TEST_CASE("removeRpath")
{
  SharedLibrariesDeployer deployer;

  SECTION("by default RPath is not removed")
  {
    REQUIRE( !deployer.removeRpath() );
  }

  SECTION("tell to remove RPath")
  {
    deployer.setRemoveRpath(true);

    REQUIRE( deployer.removeRpath() );
  }
}

TEST_CASE("copySharedLibrariesTargetsDependsOn")
{
  QTemporaryDir destinationDir;
  destinationDir.setAutoRemove(true);
  REQUIRE( destinationDir.isValid() );

  MessageLogger messageLogger;
  MessageLogger::setBackend<ConsoleMessageLogger>();

  SharedLibrariesDeployer deployer;
  QObject::connect(&deployer, &SharedLibrariesDeployer::statusMessage, MessageLogger::info);
  QObject::connect(&deployer, &SharedLibrariesDeployer::verboseMessage, MessageLogger::info);
  QObject::connect(&deployer, &SharedLibrariesDeployer::debugMessage, MessageLogger::info);

  const QStringList targets = {testExecutableFilePath, qtCoreLibraryFilePath};

  const auto toQFileInfo = [](const QString & filePath){
    return filePath;
  };

  deployer.setSearchPrefixPathList(searchPrefixPathList);
  deployer.setOverwriteBehavior(OverwriteBehavior::Fail);
  deployer.setRemoveRpath(false);
#ifdef COMPILER_IS_MSVC
  CompilerLocationRequest compilerLocation;
  compilerLocation.setType(CompilerLocationType::CompilerPath);
  compilerLocation.setValue(  QString::fromLocal8Bit(CXX_COMPILER_PATH) );
  deployer.setCompilerLocation(compilerLocation);
#endif // #ifdef COMPILER_IS_MSVC

  deployer.copySharedLibrariesTargetsDependsOn( targets, toQFileInfo, destinationDir.path() );

  REQUIRE( !deployer.foundDependencies().isEmpty() );
  REQUIRE( containsTestSharedLibrary( deployer.foundDependencies() ) );
  REQUIRE( dirContainsTestSharedLibrary(destinationDir) );
  if(checkRPath){
    REQUIRE( getTestSharedLibraryRunPath(destinationDir).entryAt(0).path() == QLatin1String(".") );
  }
}
