/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2022 Philippe Steinmann.
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
#include "TestIsExistingSharedLibrary.h"
#include "SharedLibraryFinderLinuxTestCommon.h"
#include "Mdt/DeployUtils/QtDistributionDirectory.h"
#include "Mdt/DeployUtils/SharedLibraryFinderLinux.h"
#include "Mdt/DeployUtils/RPath.h"
#include <QLatin1String>
#include <QString>
#include <QtGlobal>
#include <memory>

using namespace Mdt::DeployUtils;

TEST_CASE("OperatingSystem")
{
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibraryFinderLinux finder(isExistingSharedLibraryOp, qtDistributionDirectory);

  REQUIRE( finder.operatingSystem() == OperatingSystem::Linux );
}

TEST_CASE("buildSearchPathList")
{
/*
 * SharedLibraryFinderLinux::buildSearchPathList()
 * will remove non existing directories, which is good.
 * But, this test would probably fail on non Linux platform
 */
#ifdef Q_OS_UNIX
  constexpr bool checkResult = true;
#else
  constexpr bool checkResult = false;
#endif // #ifdef Q_OS_UNIX

  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibraryFinderLinux finder(isExistingSharedLibraryOp, qtDistributionDirectory);
  PathList searchFirstPathPrefixList;

  SECTION("x86")
  {
    finder.buildSearchPathList(searchFirstPathPrefixList, ProcessorISA::X86_32);
    if(checkResult){
      REQUIRE( !finder.searchPathList().isEmpty() );
    }
  }

  SECTION("x86_64")
  {
    finder.buildSearchPathList(searchFirstPathPrefixList, ProcessorISA::X86_64);
    if(checkResult){
      REQUIRE( !finder.searchPathList().isEmpty() );
    }
  }
}

TEST_CASE("libraryShouldBeDistributed")
{
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibraryFinderLinux finder(isExistingSharedLibraryOp, qtDistributionDirectory);

  SECTION("libQt5Core.so should be distributed")
  {
    REQUIRE( finder.libraryShouldBeDistributed( QLatin1String("libQt5Core.so") ) );
  }

  SECTION("ld-linux.so.2 should not be distributed")
  {
    REQUIRE( !finder.libraryShouldBeDistributed( QLatin1String("ld-linux.so.2") ) );
  }
}

TEST_CASE("makeDirectoryFromRpathEntry")
{
  const auto originExecutable = makeBinaryDependenciesFileFromUtf8Path("/opt/myapp");

  SECTION("rpath entry:/tmp")
  {
    REQUIRE( makeDirectoryFromRpathEntry( originExecutable, RPathEntry( QLatin1String("/tmp") ) ) == QLatin1String("/tmp") );
  }

  SECTION("rpath entry:. (relative to binary, like ELF $ORIGIN)")
  {
    REQUIRE( makeDirectoryFromRpathEntry( originExecutable, RPathEntry( QLatin1String(".") ) ) == makeAbsolutePath("/opt") );
  }

  SECTION("rpath entry:../lib")
  {
    REQUIRE( makeDirectoryFromRpathEntry( originExecutable, RPathEntry( QLatin1String("../lib") ) ) == makeAbsolutePath("/lib") );
  }
}

TEST_CASE("findLibraryAbsolutePathByRPath")
{
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibraryFinderLinux finder(isExistingSharedLibraryOp, qtDistributionDirectory);
  BinaryDependenciesFile library;

  auto originExecutable = makeBinaryDependenciesFileFromUtf8Path("/opt/myapp");

  SECTION("libA.so - rpath:/tmp - exists")
  {
    originExecutable.setRPath( makeRPathFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so"});

    library = finder.findLibraryAbsolutePathByRPath( QLatin1String("libA.so"), originExecutable );

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }

  SECTION("libA.so - rpath:. - exists")
  {
    originExecutable.setRPath( makeRPathFromUtf8Paths({"."}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/opt/libA.so"});

    library = finder.findLibraryAbsolutePathByRPath( QLatin1String("libA.so"), originExecutable );

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/opt/libA.so") );
  }

  SECTION("libA.so - rpath:/tmp - not exists")
  {
    originExecutable.setRPath( makeRPathFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/lib/libA.so"});

    library = finder.findLibraryAbsolutePathByRPath( QLatin1String("libA.so"), originExecutable );

    REQUIRE( library.isNull() );
  }
}

TEST_CASE("findLibraryAbsolutePath_OLD")
{
  QString libraryName;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibraryFinderLinux finder(isExistingSharedLibraryOp, qtDistributionDirectory);

  SECTION("libA.so - pathList:/tmp - exists")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    libraryName = QLatin1String("libA.so");
    finder.setSearchPathList( makePathListFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so"});

    auto library = finder.findLibraryAbsolutePath(libraryName, executable);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }

  SECTION("libA.so - rpath:/tmp - exists")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    executable.setRPath( makeRPathFromUtf8Paths({"/tmp"}) );
    libraryName = QLatin1String("libA.so");
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so"});

    auto library = finder.findLibraryAbsolutePath(libraryName, executable);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }

  SECTION("libA.so - pathList:/tmp,/opt - exists in both path - must pick the first one")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    libraryName = QLatin1String("libA.so");
    finder.setSearchPathList( makePathListFromUtf8Paths({"/tmp","/opt"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so","/opt/libA.so"});

    auto library = finder.findLibraryAbsolutePath(libraryName, executable);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }
}

TEST_CASE("findLibraryAbsolutePath")
{
  REQUIRE(false);
}

TEST_CASE("findLibrariesAbsolutePath")
{
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibraryFinderLinux finder(isExistingSharedLibraryOp, qtDistributionDirectory);
  BinaryDependenciesFileList libraries;

  SECTION("libA.so,libm.so.6 - pathList:/tmp - exists - libm.so.6 must be excluded")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    executable.setDependenciesFileNames({QLatin1String("libA.so"),QLatin1String("libm.so.6")});
    finder.setSearchPathList( makePathListFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so","/tmp/libm.so.6"});

    libraries = finder.findLibrariesAbsolutePath(executable);

    REQUIRE( libraries.size() == 1 );
    REQUIRE( libraries[0].absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }
}
