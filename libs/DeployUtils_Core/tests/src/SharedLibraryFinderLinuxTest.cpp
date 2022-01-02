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
#include "SharedLibraryFinderLinuxTestCommon.h"
#include "Mdt/DeployUtils/SharedLibraryFinderLinux.h"
#include "Mdt/DeployUtils/RPath.h"
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;

TEST_CASE("buildSearchPathList")
{
  PathList searchFirstPathPrefixList;
  PathList searchPathList;

  SECTION("x86")
  {
    searchPathList = SharedLibraryFinderLinux::buildSearchPathList(searchFirstPathPrefixList, ProcessorISA::X86_32);
    REQUIRE( !searchPathList.isEmpty() );
  }

  SECTION("x86_64")
  {
    searchPathList = SharedLibraryFinderLinux::buildSearchPathList(searchFirstPathPrefixList, ProcessorISA::X86_64);
    REQUIRE( !searchPathList.isEmpty() );
  }
}

TEST_CASE("makeDirectoryFromRpathEntry")
{
  const auto originExecutable = makeBinaryDependenciesFileFromUtf8Path("/opt/myapp");

  SECTION("rpath entry:/tmp")
  {
    REQUIRE( makeDirectoryFromRpathEntry( originExecutable, RPathEntry( QLatin1String("/tmp") ) ) == makeAbsolutePath("/tmp") );
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
//   RPath rpath;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  BinaryDependenciesFile library;

  auto originExecutable = makeBinaryDependenciesFileFromUtf8Path("/opt/myapp");

  SECTION("libA.so - rpath:/tmp - exists")
  {
    originExecutable.setRPath( makeRPathFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so"});

    library = findLibraryAbsolutePathByRPath(originExecutable, QLatin1String("libA.so"), isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }

  SECTION("libA.so - rpath:. - exists")
  {
    originExecutable.setRPath( makeRPathFromUtf8Paths({"."}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/opt/libA.so"});

    library = findLibraryAbsolutePathByRPath(originExecutable, QLatin1String("libA.so"), isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/opt/libA.so") );
  }

  SECTION("libA.so - rpath:/tmp - not exists")
  {
    originExecutable.setRPath( makeRPathFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/lib/libA.so"});

    library = findLibraryAbsolutePathByRPath(originExecutable, QLatin1String("libA.so"), isExistingSharedLibraryOp);

    REQUIRE( library.isNull() );
  }
}

TEST_CASE("findLibraryAbsolutePath")
{
  QString libraryName;
  PathList pathList;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;

  SECTION("libA.so - pathList:/tmp - exists")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    libraryName = QLatin1String("libA.so");
    pathList = makePathListFromUtf8Paths({"/tmp"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so"});

    auto library = findLibraryAbsolutePath(executable, libraryName, pathList, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }

  SECTION("libA.so - rpath:/tmp - exists")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    executable.setRPath( makeRPathFromUtf8Paths({"/tmp"}) );
    libraryName = QLatin1String("libA.so");
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so"});

    auto library = findLibraryAbsolutePath(executable, libraryName, pathList, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }

  SECTION("libA.so - pathList:/tmp,/opt - exists in both path - must pick the first one")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    libraryName = QLatin1String("libA.so");
    pathList = makePathListFromUtf8Paths({"/tmp","/opt"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so","/opt/libA.so"});

    auto library = findLibraryAbsolutePath(executable, libraryName, pathList, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }
}

TEST_CASE("findLibrariesAbsolutePath")
{
  PathList pathList;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  BinaryDependenciesFileList libraries;

  SECTION("libA.so,libm.so.6 - pathList:/tmp - exists - libm.so.6 must be excluded")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    executable.setDependenciesFileNames({QLatin1String("libA.so"),QLatin1String("libm.so.6")});
    pathList = makePathListFromUtf8Paths({"/tmp"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libA.so","/tmp/libm.so.6"});

    libraries = findLibrariesAbsolutePath(executable, pathList, isExistingSharedLibraryOp);

    REQUIRE( libraries.size() == 1 );
    REQUIRE( libraries[0].absoluteFilePath() == makeAbsolutePath("/tmp/libA.so") );
  }
}
