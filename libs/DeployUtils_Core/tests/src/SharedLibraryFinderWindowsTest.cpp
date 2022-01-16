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
#include <QtGlobal>

using namespace Mdt::DeployUtils;

TEST_CASE("ExcludeMsvcLibraries")
{
  TestIsExistingSharedLibrary isExistingShLibOp;
  SharedLibraryFinderWindows finder(isExistingShLibOp);

  SECTION("by default MSVC libraries are included but Windows API sets not")
  {
    REQUIRE( !finder.hasToExcludeMsvcLibraries() );
    REQUIRE( finder.hasToExcludeWindowsApiSets() );
  }

  SECTION("include MSVC libraries and Windows API sets")
  {
    finder.setExcludeWindowsApiSets(false);

    REQUIRE( !finder.hasToExcludeMsvcLibraries() );
    REQUIRE( !finder.hasToExcludeWindowsApiSets() );
  }

  SECTION("if MSVC libraries are excluded Windows API sets are allways excluded")
  {
    finder.setExcludeMsvcLibraries(true);

    SECTION("exclude Windows API sets")
    {
      finder.setExcludeWindowsApiSets(true);

      REQUIRE( finder.hasToExcludeWindowsApiSets() );
    }

    SECTION("include Windows API sets - must be ignored")
    {
      finder.setExcludeWindowsApiSets(false);

      REQUIRE( finder.hasToExcludeWindowsApiSets() );
    }
  }
}

TEST_CASE("isMsvcLibrary")
{
  SECTION("Qt5Core.dll")
  {
    REQUIRE( !SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("Qt5Core.dll") ) );
  }

  SECTION("concrt140.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("concrt140.dll") ) );
  }

  SECTION("concrt140d.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("concrt140d.dll") ) );
  }

  SECTION("CONCRT140.DLL")
  {
    REQUIRE( SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("CONCRT140.DLL") ) );
  }

  SECTION("msvcp140.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("msvcp140.dll") ) );
  }

  SECTION("vccorlib140.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("vccorlib140.dll") ) );
  }

  SECTION("vcruntime140.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("vcruntime140.dll") ) );
  }

  SECTION("vcamp140.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("vcamp140.dll") ) );
  }

  SECTION("vcomp140.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isMsvcLibrary( QLatin1String("vcomp140.dll") ) );
  }

}

TEST_CASE("isWindowsApiSet")
{
  SECTION("api-ms-win-core-ums-l1-1-0")
  {
    REQUIRE( SharedLibraryFinderWindows::isWindowsApiSet( QLatin1String("api-ms-win-core-ums-l1-1-0") ) );
  }

  SECTION("api-ms-win-core-ums-l1-1-0.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isWindowsApiSet( QLatin1String("api-ms-win-core-ums-l1-1-0.dll") ) );
  }

  SECTION("API-MS-WIN-CORE-UMS-L1-1-0")
  {
    REQUIRE( SharedLibraryFinderWindows::isWindowsApiSet( QLatin1String("API-MS-WIN-CORE-UMS-L1-1-0") ) );
  }

  SECTION("ext-ms-win-com-ole32-l1-1-5")
  {
    REQUIRE( SharedLibraryFinderWindows::isWindowsApiSet( QLatin1String("ext-ms-win-com-ole32-l1-1-5") ) );
  }

  SECTION("ext-ms-win-com-ole32-l1-1-5.dll")
  {
    REQUIRE( SharedLibraryFinderWindows::isWindowsApiSet( QLatin1String("ext-ms-win-com-ole32-l1-1-5.dll") ) );
  }

  SECTION("EXT-MS-WIN-COM-OLE32-L1-1-5")
  {
    REQUIRE( SharedLibraryFinderWindows::isWindowsApiSet( QLatin1String("EXT-MS-WIN-COM-OLE32-L1-1-5") ) );
  }

  SECTION("api.dll")
  {
    REQUIRE( !SharedLibraryFinderWindows::isWindowsApiSet( QLatin1String("api.dll") ) );
  }
}

TEST_CASE("libraryHasToBeExcluded")
{
  TestIsExistingSharedLibrary isExistingShLibOp;
  SharedLibraryFinderWindows finder(isExistingShLibOp);

  SECTION("exclude MSVC (and also Windows API sets)")
  {
    finder.setExcludeMsvcLibraries(true);
    finder.setExcludeWindowsApiSets(true);

    SECTION("Qt5Core.dll")
    {
      REQUIRE( !finder.libraryHasToBeExcluded( QLatin1String("Qt5Core.dll") ) );
    }

    SECTION("msvcp140.dll")
    {
      REQUIRE( finder.libraryHasToBeExcluded( QLatin1String("msvcp140.dll") ) );
    }

    SECTION("api-ms-win-core-ums-l1-1-0")
    {
      REQUIRE( finder.libraryHasToBeExcluded( QLatin1String("api-ms-win-core-ums-l1-1-0") ) );
    }
  }

  SECTION("exclude only Windows API sets")
  {
    finder.setExcludeMsvcLibraries(false);
    finder.setExcludeWindowsApiSets(true);

    SECTION("Qt5Core.dll")
    {
      REQUIRE( !finder.libraryHasToBeExcluded( QLatin1String("Qt5Core.dll") ) );
    }

    SECTION("msvcp140.dll")
    {
      REQUIRE( !finder.libraryHasToBeExcluded( QLatin1String("msvcp140.dll") ) );
    }

    SECTION("api-ms-win-core-ums-l1-1-0")
    {
      REQUIRE( finder.libraryHasToBeExcluded( QLatin1String("api-ms-win-core-ums-l1-1-0") ) );
    }
  }

  SECTION("do not exclude any MSVC library")
  {
    finder.setExcludeMsvcLibraries(false);
    finder.setExcludeWindowsApiSets(false);

    SECTION("Qt5Core.dll")
    {
      REQUIRE( !finder.libraryHasToBeExcluded( QLatin1String("Qt5Core.dll") ) );
    }

    SECTION("msvcp140.dll")
    {
      REQUIRE( !finder.libraryHasToBeExcluded( QLatin1String("msvcp140.dll") ) );
    }

    SECTION("api-ms-win-core-ums-l1-1-0")
    {
      REQUIRE( !finder.libraryHasToBeExcluded( QLatin1String("api-ms-win-core-ums-l1-1-0") ) );
    }
  }
}

TEST_CASE("buildSearchPathList")
{
/*
 * SharedLibraryFinderWindows::buildSearchPathList()
 * will remove non existing directories, which is good.
 * But, this test would probably fail on non Windows platform
 */
#ifdef Q_OS_WIN
  constexpr bool checkResult = true;
#else
  constexpr bool checkResult = false;
#endif // #ifdef Q_OS_WIN

  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderWindows finder(isExistingSharedLibraryOp);
  PathList searchFirstPathPrefixList;
  std::shared_ptr<CompilerFinder> compilerFinder;

  QFileInfo binaryFilePath( QLatin1String("/some/path/to/app.exe") );

  SECTION("x86 (compiler finder is null)")
  {
    finder.buildSearchPathList(binaryFilePath, searchFirstPathPrefixList, ProcessorISA::X86_32, compilerFinder);
    if(checkResult){
      REQUIRE( !finder.searchPathList().isEmpty() );
    }
  }

  SECTION("x86_64 (compiler finder is null)")
  {
    finder.buildSearchPathList(binaryFilePath, searchFirstPathPrefixList, ProcessorISA::X86_64, compilerFinder);
    if(checkResult){
      REQUIRE( !finder.searchPathList().isEmpty() );
    }
  }
}

TEST_CASE("findLibraryAbsolutePathByAlternateNames")
{
  BinaryDependenciesFile library;
  QFileInfo libraryFile;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderWindows finder(isExistingSharedLibraryOp);

  SECTION("A.dll exists as /tmp/A.dll")
  {
    libraryFile.setFile( QLatin1String("/tmp/A.dll") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.dll"});

    library = finder.findLibraryAbsolutePathByAlternateNames(libraryFile);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/A.dll") );
  }

  SECTION("A.dll exists as /tmp/a.dll")
  {
    libraryFile.setFile( QLatin1String("/tmp/A.dll") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/a.dll"});

    library = finder.findLibraryAbsolutePathByAlternateNames(libraryFile);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/a.dll") );
  }

  SECTION("A.DLL exists as /tmp/a.dll")
  {
    libraryFile.setFile( QLatin1String("/tmp/A.DLL") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/a.dll"});

    library = finder.findLibraryAbsolutePathByAlternateNames(libraryFile);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/a.dll") );
  }

  SECTION("a.dll exists as /tmp/A.DLL")
  {
    libraryFile.setFile( QLatin1String("/tmp/a.dll") );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.DLL"});

    library = finder.findLibraryAbsolutePathByAlternateNames(libraryFile);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/A.DLL") );
  }
}

TEST_CASE("findLibraryAbsolutePath")
{
  QString libraryName;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderWindows finder(isExistingSharedLibraryOp);

  SECTION("A.dll - pathList:/tmp - exists")
  {
    libraryName = QLatin1String("A.dll");
    finder.setSearchPathList( makePathListFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.dll"});

    auto library = finder.findLibraryAbsolutePath(libraryName);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/A.dll") );
  }

  /*
   * This is important if we search dependencies for a Windows binary
   * from a non Windows machine.
   */
  SECTION("A.DLL - pathList:/tmp - exists as /tmp/a.dll")
  {
    libraryName = QLatin1String("A.DLL");
    finder.setSearchPathList( makePathListFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/a.dll"});

    auto library = finder.findLibraryAbsolutePath(libraryName);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/a.dll") );
  }

  SECTION("A.dll - pathList:/tmp,/opt - exists in both path - must pick the first one")
  {
    libraryName = QLatin1String("A.dll");
    finder.setSearchPathList( makePathListFromUtf8Paths({"/tmp","/opt"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.dll","/opt/A.dll"});

    auto library = finder.findLibraryAbsolutePath(libraryName);

    REQUIRE( library.absoluteFilePath() == makeAbsolutePath("/tmp/A.dll") );
  }
}

TEST_CASE("findLibrariesAbsolutePath")
{
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  SharedLibraryFinderWindows finder(isExistingSharedLibraryOp);
  BinaryDependenciesFileList libraries;

  SECTION("A.dll,KERNEL32.DLL - pathList:/tmp - exists - KERNEL32.DLL must be excluded")
  {
    auto executable = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable.exe");
    executable.setDependenciesFileNames({QLatin1String("A.dll"),QLatin1String("KERNEL32.DLL")});
    finder.setSearchPathList( makePathListFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/A.dll","/tmp/KERNEL32.DLL"});

    libraries = finder.findLibrariesAbsolutePath(executable);

    REQUIRE( libraries.size() == 1 );
    REQUIRE( libraries[0].absoluteFilePath() == makeAbsolutePath("/tmp/A.dll") );
  }
}
