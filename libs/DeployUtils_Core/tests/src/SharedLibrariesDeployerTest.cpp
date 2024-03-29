/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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
#include "BinaryDependenciesTestCommon.h"
#include "Mdt/DeployUtils/QtDistributionDirectory.h"
#include "Mdt/DeployUtils/SharedLibrariesDeployer.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/ConsoleMessageLogger.h"
#include "Mdt/DeployUtils/RPath.h"
#include <Mdt/ExecutableFile/ExecutableFileReader.h>
#include <QLatin1String>
#include <QString>
#include <QStringList>
#include <QTemporaryDir>
#include <memory>

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
  Mdt::ExecutableFile::ExecutableFileReader reader;
  reader.openFile( makePath(dir, "libtestSharedLibrary.so") );

  return reader.getRunPath();
}


TEST_CASE("searchPrefixPathList")
{
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibrariesDeployer deployer(qtDistributionDirectory);

  const QStringList pathList = qStringListFromUtf8Strings({"/tmp/qt"});
  deployer.setSearchPrefixPathList( PathList::fromStringList(pathList) );

  REQUIRE( deployer.searchPrefixPathList().containsPath( QLatin1String("/tmp/qt") ) );
}

TEST_CASE("overwriteBehavior")
{
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibrariesDeployer deployer(qtDistributionDirectory);

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
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibrariesDeployer deployer(qtDistributionDirectory);

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

TEST_CASE("hasToUpdateRpath")
{
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibrariesDeployer deployer(qtDistributionDirectory);

  CopiedSharedLibraryFile copiedFile;

  PathList systemWideLocations{makeAbsolutePath("/usr/lib")};

  RPath rpath;
  rpath.appendPath( QLatin1String(".") );

  SECTION("copied file already has correct rpath")
  {
    copiedFile.rpath = rpath;

    REQUIRE( !deployer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
  }

  // https://gitlab.com/scandyna/mdtdeployutils/-/issues/3
  SECTION("take in to account the source location")
  {
    QFileInfo sourceFile;
    QFileInfo destinationFile;

    SECTION("file comes from /usr/lib")
    {
      sourceFile.setFile( QLatin1String("/usr/lib") );
      destinationFile.setFile( QLatin1String("/tmp/app/lib") );
      copiedFile.file.setSourceFileInfo(sourceFile);
      copiedFile.file.setDestinationFileInfo(destinationFile);

      SECTION("copied file has no rpath (must be updated)")
      {
        REQUIRE( copiedFile.rpath.isEmpty() );

        REQUIRE( deployer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
      }
    }

    SECTION("file comes from /opt/qt/lib")
    {
      sourceFile.setFile( QLatin1String("/opt/qt/lib") );
      destinationFile.setFile( QLatin1String("/tmp/app/lib") );
      copiedFile.file.setSourceFileInfo(sourceFile);
      copiedFile.file.setDestinationFileInfo(destinationFile);

      SECTION("copied file has no rpath (must NOT be updated)")
      {
        REQUIRE( copiedFile.rpath.isEmpty() );

        REQUIRE( !deployer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
      }

      SECTION("copied file has the correct rpath (must NOT be updated)")
      {
        copiedFile.rpath = rpath;

        REQUIRE( !deployer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
      }

      SECTION("copied file has wrong rpath (must be updated)")
      {
        copiedFile.rpath.appendPath( QLatin1String("/build/project") );

        REQUIRE( deployer.hasToUpdateRpath(copiedFile, rpath, systemWideLocations) );
      }
    }
  }
}

void doDeployerCommonSetup(SharedLibrariesDeployer & deployer)
{
  MessageLogger::setBackend<ConsoleMessageLogger>();
  QObject::connect(&deployer, &SharedLibrariesDeployer::statusMessage, MessageLogger::info);
  QObject::connect(&deployer, &SharedLibrariesDeployer::verboseMessage, MessageLogger::info);
  QObject::connect(&deployer, &SharedLibrariesDeployer::debugMessage, MessageLogger::info);

  deployer.setSearchPrefixPathList(searchPrefixPathList);

#ifdef COMPILER_IS_MSVC
  CompilerLocationRequest compilerLocation;
  compilerLocation.setType(CompilerLocationType::CompilerPath);
  compilerLocation.setValue(  QString::fromLocal8Bit(CXX_COMPILER_PATH) );
  deployer.setCompilerLocation(compilerLocation);
#endif // #ifdef COMPILER_IS_MSVC

}

TEST_CASE("findSharedLibrariesTargetDependsOn")
{
  MessageLogger messageLogger;
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibrariesDeployer deployer(qtDistributionDirectory);

  doDeployerCommonSetup(deployer);

  const QFileInfo target(testExecutableFilePath);

  REQUIRE( deployer.currentPlatform().isNull() );

  const BinaryDependenciesResult libraries = deployer.findSharedLibrariesTargetDependsOn(target);

  REQUIRE( deployer.currentPlatform() == Platform::nativePlatform() );
  REQUIRE( libraries.isSolved() );
  REQUIRE( containsTestSharedLibrary(libraries) );
  REQUIRE( containsQt5Core(libraries) );
}

TEST_CASE("findSharedLibrariesTargetsDependsOn")
{
  MessageLogger messageLogger;
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibrariesDeployer deployer(qtDistributionDirectory);

  doDeployerCommonSetup(deployer);

  const QFileInfoList targets{testExecutableFilePath, qtCoreLibraryFilePath};

  REQUIRE( deployer.currentPlatform().isNull() );

  const BinaryDependenciesResultList resultList = deployer.findSharedLibrariesTargetsDependsOn(targets);

  REQUIRE( deployer.currentPlatform() == Platform::nativePlatform() );
  REQUIRE( resultList.resultCount() == 2 );

  const auto exeResult = resultList.findResultForTargetName( targets.at(0).fileName() );
  REQUIRE( exeResult.has_value() );
  REQUIRE( exeResult->isSolved() );
  REQUIRE( containsTestSharedLibrary(*exeResult) );
  REQUIRE( containsQt5Core(*exeResult) );

  const auto qtCoreResult = resultList.findResultForTargetName( targets.at(0).fileName() );
  REQUIRE( qtCoreResult.has_value() );
  REQUIRE( qtCoreResult->isSolved() );
}

TEST_CASE("copySharedLibrariesTargetDependsOn")
{
  QTemporaryDir destinationDir;
  destinationDir.setAutoRemove(true);
  REQUIRE( destinationDir.isValid() );

  MessageLogger messageLogger;

  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibrariesDeployer deployer(qtDistributionDirectory);
  doDeployerCommonSetup(deployer);
  deployer.setOverwriteBehavior(OverwriteBehavior::Fail);
  deployer.setRemoveRpath(false);

  REQUIRE( deployer.currentPlatform().isNull() );

  deployer.copySharedLibrariesTargetDependsOn( testExecutableFilePath, destinationDir.path() );

  REQUIRE( deployer.currentPlatform() == Platform::nativePlatform() );
  REQUIRE( dirContainsTestSharedLibrary(destinationDir) );
  if(checkRPath){
    REQUIRE( getTestSharedLibraryRunPath(destinationDir).entryAt(0).path() == QLatin1String(".") );
  }
}
