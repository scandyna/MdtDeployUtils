// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "BinaryDependenciesTestCommon.h"
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/BinaryDependenciesResult.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include <QLatin1String>
#include <QFileInfo>

using namespace Mdt::DeployUtils;

TEST_CASE("construct")
{
  const auto os = OperatingSystem::Linux;
  QFileInfo file( QLatin1String("/opt/project/app") );

  BinaryDependenciesResult result(file, os);

  REQUIRE( result.target().absoluteFilePath() == makeAbsolutePath("/opt/project/app") );
  REQUIRE( result.isSolved() );
}

TEST_CASE("addLibrary")
{
  const auto os = OperatingSystem::Linux;
  QFileInfo file( QLatin1String("/opt/project/app") );
  BinaryDependenciesResult result(file, os);

  RPath rpath;
  rpath.appendPath( QLatin1String("/opt") );

  SECTION("add a library with its full path (found)")
  {
    QFileInfo libA( QLatin1String("/opt/libA.so") );

    result.addFoundLibrary(libA, rpath);

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( result.libraryAt(0).isFound() );
    REQUIRE( !result.libraryAt(0).shouldNotBeRedistributed() );
    REQUIRE( result.libraryAt(0).rPath() == rpath );
    REQUIRE( result.containsLibraryName( QLatin1String("libA.so") ) );
    REQUIRE( result.isSolved() );
  }

  SECTION("add a library that was not found")
  {
    QFileInfo libA( QLatin1String("libA.so") );

    result.addNotFoundLibrary(libA);

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( !result.libraryAt(0).isFound() );
    REQUIRE( !result.libraryAt(0).shouldNotBeRedistributed() );
    REQUIRE( result.containsLibraryName( QLatin1String("libA.so") ) );
    REQUIRE( !result.isSolved() );
  }

  SECTION("add a library that should not be redistributed")
  {
    QFileInfo libA( QLatin1String("libA.so") );

    result.addLibraryToNotRedistribute(libA);

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( !result.libraryAt(0).isFound() );
    REQUIRE( result.libraryAt(0).shouldNotBeRedistributed() );
    REQUIRE( result.containsLibraryName( QLatin1String("libA.so") ) );
    REQUIRE( result.isSolved() );
  }

  SECTION("add a found library 2x")
  {
    QFileInfo libA( QLatin1String("/opt/libA.so") );

    result.addFoundLibrary(libA, rpath);
    result.addFoundLibrary(libA, rpath);

    REQUIRE( result.libraryCount() == 1 );
  }

  SECTION("add a NOT found library 2x")
  {
    QFileInfo libA( QLatin1String("libA.so") );

    result.addNotFoundLibrary(libA);
    result.addNotFoundLibrary(libA);

    REQUIRE( result.libraryCount() == 1 );
  }

  SECTION("add a library that should not be redistributed 2x")
  {
    QFileInfo libA( QLatin1String("libA.so") );

    result.addLibraryToNotRedistribute(libA);
    result.addLibraryToNotRedistribute(libA);

    REQUIRE( result.libraryCount() == 1 );
  }
}

TEST_CASE("addLibrary_WindowsSpecifics")
{
  const auto os = OperatingSystem::Windows;
  QFileInfo file( QLatin1String("/opt/project/app") );
  RPath rpath;
  BinaryDependenciesResult result(file, os);

  SECTION("add LIBA.DLL.DLL and libA.dll as found")
  {
    QFileInfo LIBA( QLatin1String("/opt/LIBA.DLL") );
    QFileInfo libA( QLatin1String("/opt/libA.dll") );

    result.addFoundLibrary(LIBA, rpath);
    result.addFoundLibrary(libA, rpath);

    REQUIRE( result.libraryCount() == 1 );
  }

  SECTION("add LIBA.DLL and libA.dll as NOT found")
  {
    QFileInfo LIBA( QLatin1String("LIBA.DLL") );
    QFileInfo libA( QLatin1String("libA.dll") );

    result.addNotFoundLibrary(LIBA);
    result.addNotFoundLibrary(libA);

    REQUIRE( result.libraryCount() == 1 );
  }

  SECTION("add KERNEL32.DLL and kernel32.dll as not to be redistributed")
  {
    QFileInfo KERNEL32( QLatin1String("KERNEL32.DLL") );
    QFileInfo kernel32( QLatin1String("kernel32.dll") );

    result.addLibraryToNotRedistribute(KERNEL32);
    result.addLibraryToNotRedistribute(kernel32);

    REQUIRE( result.libraryCount() == 1 );
  }
}

TEST_CASE("findLibraryByName")
{
  const auto os = OperatingSystem::Linux;
  QFileInfo file( QLatin1String("/opt/project/app") );
  RPath rpath;
  BinaryDependenciesResult result(file, os);

  QFileInfo libAfi( QLatin1String("/opt/libA.so") );
  result.addFoundLibrary(libAfi, rpath);

  const auto libA = result.findLibraryByName( QLatin1String("libA.so") );
  REQUIRE( libA.has_value() );
  REQUIRE( libA->libraryName() == QLatin1String("libA.so") );

  const auto notFound = result.findLibraryByName( QLatin1String("notFound.so") );
  REQUIRE( !notFound.has_value() );
}

TEST_CASE("getLibrariesToRedistribute")
{
  const auto os = OperatingSystem::Linux;
  QFileInfo app( QLatin1String("/opt/project/app") );
  RPath rpath;
  BinaryDependenciesResult result(app, os);
  std::vector<BinaryDependenciesResultLibrary> libraries;

  SECTION("result contains 1 library to redistribute")
  {
    QFileInfo libA( QLatin1String("/opt/libA.so") );
    result.addFoundLibrary(libA, rpath);

    libraries = getLibrariesToRedistribute(result);

    REQUIRE( libraries.size() == 1 );
    REQUIRE( libraryListContainsPath(libraries, "/opt/libA.so") );
  }

  SECTION("result contains 1 library to not redistribute")
  {
    QFileInfo libA( QLatin1String("libA.so") );
    result.addLibraryToNotRedistribute(libA);

    libraries = getLibrariesToRedistribute(result);

    REQUIRE( libraries.size() == 0 );
  }

  SECTION("result contains 1 library to not redistribute with its absolute path")
  {
    QFileInfo libc( QLatin1String("/lib/libc.so") );
    result.addLibraryToNotRedistribute(libc);

    libraries = getLibrariesToRedistribute(result);

    REQUIRE( libraries.size() == 0 );
  }
}

TEST_CASE("getLibrariesToRedistributeFilePathList")
{
  const auto os = OperatingSystem::Linux;
  QFileInfo app( QLatin1String("/opt/project/app") );
  RPath rpath;
  BinaryDependenciesResult result(app, os);
  QStringList libraries;

  SECTION("result contains 1 library to redistribute")
  {
    QFileInfo libA( QLatin1String("/opt/libA.so") );
    result.addFoundLibrary(libA, rpath);

    libraries = getLibrariesToRedistributeFilePathList(result);

    REQUIRE( libraries.size() == 1 );
    REQUIRE( libraries.contains( makeAbsolutePath("/opt/libA.so") ) );
  }

  SECTION("result contains 1 library to not redistribute")
  {
    QFileInfo libA( QLatin1String("libA.so") );
    result.addLibraryToNotRedistribute(libA);

    libraries = getLibrariesToRedistributeFilePathList(result);

    REQUIRE( libraries.size() == 0 );
  }

  SECTION("result contains 1 library to not redistribute with its absolute path")
  {
    QFileInfo libc( QLatin1String("/lib/libc.so") );
    result.addLibraryToNotRedistribute(libc);

    libraries = getLibrariesToRedistributeFilePathList(result);

    REQUIRE( libraries.size() == 0 );
  }
}
