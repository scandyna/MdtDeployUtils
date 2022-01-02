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
#include "SharedLibraryFinderWindowsTestCommon.h"
#include "Mdt/DeployUtils/SharedLibraryFinderWindows.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("buildSearchPathList")
{
  PathList searchFirstPathPrefixList;
  PathList searchPathList;
  std::shared_ptr<CompilerFinder> compilerFinder;

  QFileInfo binaryFilePath( QLatin1String("/some/path/to/app.exe") );

  SECTION("x86 (compiler finder is null)")
  {
    searchPathList = SharedLibraryFinderWindows::buildSearchPathList(binaryFilePath, searchFirstPathPrefixList, ProcessorISA::X86_32, compilerFinder);
    REQUIRE( !searchPathList.isEmpty() );
  }

  SECTION("x86_64 (compiler finder is null)")
  {
    searchPathList = SharedLibraryFinderWindows::buildSearchPathList(binaryFilePath, searchFirstPathPrefixList, ProcessorISA::X86_64, compilerFinder);
    REQUIRE( !searchPathList.isEmpty() );
  }
}

TEST_CASE("findLibraryAbsolutePathByAlternateNames")
{
  BinaryDependenciesFile library;
  QFileInfo libraryFile;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;

  SECTION("A.dll exists as /tmp/A.dll")
  {
    libraryFile.setFile( QLatin1String("/tmp/A.dll") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.dll"});

    library = findLibraryAbsolutePathByAlternateNames(libraryFile, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/A.dll") );
  }

  SECTION("A.dll exists as /tmp/a.dll")
  {
    libraryFile.setFile( QLatin1String("/tmp/A.dll") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/a.dll"});

    library = findLibraryAbsolutePathByAlternateNames(libraryFile, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/a.dll") );
  }

  SECTION("A.DLL exists as /tmp/a.dll")
  {
    libraryFile.setFile( QLatin1String("/tmp/A.DLL") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/a.dll"});

    library = findLibraryAbsolutePathByAlternateNames(libraryFile, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/a.dll") );
  }

  SECTION("a.dll exists as /tmp/A.DLL")
  {
    libraryFile.setFile( QLatin1String("/tmp/a.dll") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.DLL"});

    library = findLibraryAbsolutePathByAlternateNames(libraryFile, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/A.DLL") );
  }
}

TEST_CASE("findLibraryAbsolutePath")
{
  QString libraryName;
  PathList pathList;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;

  SECTION("A.dll - pathList:/tmp - exists")
  {
    libraryName = QLatin1String("A.dll");
    pathList = makePathListFromUtf8Paths({"/tmp"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.dll"});

    auto library = findLibraryAbsolutePath(libraryName, pathList, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/A.dll") );
  }

  /*
   * This is important if we search dependencies for a Windows binary
   * from a non Windows machine.
   */
  SECTION("A.DLL - pathList:/tmp - exists as /tmp/a.dll")
  {
    libraryName = QLatin1String("A.DLL");
    pathList = makePathListFromUtf8Paths({"/tmp"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/a.dll"});

    auto library = findLibraryAbsolutePath(libraryName, pathList, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == QLatin1String("/tmp/a.dll") );
  }

  SECTION("A.dll - pathList:/tmp,/opt - exists in both path - must pick the first one")
  {
    libraryName = QLatin1String("A.dll");
    pathList = makePathListFromUtf8Paths({"/tmp","/opt"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.dll","/opt/A.dll"});

    auto library = findLibraryAbsolutePath(libraryName, pathList, isExistingSharedLibraryOp);

    REQUIRE( library.absoluteFilePath() == QLatin1String("/tmp/A.dll") );
  }
}

TEST_CASE("findLibrariesAbsolutePath")
{
  PathList pathList;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  BinaryDependenciesFileList libraries;

  SECTION("A.dll,KERNEL32.DLL - pathList:/tmp - exists - KERNEL32.DLL must be excluded")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable.exe");
    executable.setDependenciesFileNames({QLatin1String("A.dll"),QLatin1String("KERNEL32.DLL")});
    pathList = makePathListFromUtf8Paths({"/tmp"});
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.dll","/tmp/KERNEL32.DLL"});

    libraries = findLibrariesAbsolutePath(executable, pathList, isExistingSharedLibraryOp);

    REQUIRE( libraries.size() == 1 );
    REQUIRE( libraries[0].absoluteFilePath() == QLatin1String("/tmp/A.dll") );
  }
}
