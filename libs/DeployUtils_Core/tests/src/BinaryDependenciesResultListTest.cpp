// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "BinaryDependenciesTestCommon.h"

// #include "TestFileUtils.h"

#include "Mdt/DeployUtils/BinaryDependenciesResultList.h"
#include <QLatin1String>
#include <QFileInfo>
#include <algorithm>
#include <cassert>

using namespace Mdt::DeployUtils;


TEST_CASE("addResult")
{
  const auto os = OperatingSystem::Linux;
  RPath rpath;
  BinaryDependenciesResultList resultList(os);

  SECTION("empty list")
  {
    REQUIRE( resultList.resultCount() == 0 );
    REQUIRE( resultList.isEmpty() );
    REQUIRE( !resultList.isSolved() );
  }

  SECTION("add 1 empty result")
  {
    QFileInfo app( QLatin1String("/opt/app") );
    BinaryDependenciesResult result(app, os);

    resultList.addResult(result);

    REQUIRE( resultList.resultCount() == 1 );
    REQUIRE( !resultList.isEmpty() );
    REQUIRE( resultList.isSolved() );
  }

  SECTION("add 1 solved result")
  {
    QFileInfo app( QLatin1String("/opt/app") );
    BinaryDependenciesResult result(app, os);

    QFileInfo libA( QLatin1String("/opt/libA.so") );
    result.addFoundLibrary(libA, rpath);

    resultList.addResult(result);

    REQUIRE( resultList.resultCount() == 1 );
    REQUIRE( resultList.isSolved() );
  }

  SECTION("2 results")
  {
    QFileInfo app1( QLatin1String("/opt/app1") );
    BinaryDependenciesResult solvedResult(app1, os);
    QFileInfo MyLib( QLatin1String("/opt/libMyLib.so") );
    solvedResult.addFoundLibrary(MyLib, rpath);

    QFileInfo app2( QLatin1String("/opt/app2") );
    BinaryDependenciesResult unsolvedResult(app2, os);
    QFileInfo NotFoundLib( QLatin1String("libNotFoundLib.so") );
    unsolvedResult.addNotFoundLibrary(NotFoundLib);

    REQUIRE( solvedResult.isSolved() );
    REQUIRE( !unsolvedResult.isSolved() );

    SECTION("add a solved result then a unsolved one")
    {
      resultList.addResult(solvedResult);
      resultList.addResult(unsolvedResult);

      REQUIRE( resultList.resultCount() == 2 );
      REQUIRE( !resultList.isSolved() );
    }

    SECTION("add a unsolved result then a solved one")
    {
      resultList.addResult(unsolvedResult);
      resultList.addResult(solvedResult);

      REQUIRE( resultList.resultCount() == 2 );
      REQUIRE( !resultList.isSolved() );
    }
  }
}

TEST_CASE("findResultForTargetName")
{
  const auto os = OperatingSystem::Linux;

  BinaryDependenciesResultList resultList(os);
  QFileInfo app( QLatin1String("/opt/app") );
  BinaryDependenciesResult result(app, os);
  resultList.addResult(result);

  const auto appResult = resultList.findResultForTargetName( QLatin1String("app") );
  REQUIRE( appResult.has_value() );
  REQUIRE( appResult->target().fileName() == QLatin1String("app") );

  const auto noResult = resultList.findResultForTargetName( QLatin1String("noLib") );
  REQUIRE( !noResult.has_value() );
}


TEST_CASE("getLibrariesAbsoluteFilePathList")
{
  const auto os = OperatingSystem::Linux;
  BinaryDependenciesResultList resultList(os);
  QStringList pathList;

  SECTION("empty list of results")
  {
    pathList = getLibrariesAbsoluteFilePathList(resultList);

    REQUIRE( pathList.isEmpty() );
  }

  SECTION("1 result")
  {
    QFileInfo app( QLatin1String("/opt/app") );
    BinaryDependenciesResult result(app, os);

    SECTION("1 library")
    {
      QFileInfo libA( QLatin1String("/opt/libA.so") );
      result.addLibrary( BinaryDependenciesFile::fromQFileInfo(libA) );
      resultList.addResult(result);

      pathList = getLibrariesAbsoluteFilePathList(resultList);

      REQUIRE( pathList.size() == 1 );
      REQUIRE( pathList.contains( QLatin1String("/opt/libA.so") ) );
    }
  }

  SECTION("2 results")
  {
    QFileInfo app1( QLatin1String("/opt/app1") );
    BinaryDependenciesResult result1(app1, os);
    QFileInfo app2( QLatin1String("/opt/app2") );
    BinaryDependenciesResult result2(app2, os);

    /// \todo This is probably wrong
    /*
     * NOTE
     * Libraries in the results are solved.
     * This means that they have been found on the system.
     * As result, we should never encounter a situation with different cases.
     * Example:
     *  C:/opt/libA.DLL
     *  C:/opt/liba.dll
     * above should never happen.
     * The existing library will be returned by the shared library finder correctly
     * (each result will then contain the unique variant that exists on the system)
     */
    SECTION("each result has same library")
    {
      QFileInfo libA( QLatin1String("/opt/libA.so") );

      result1.addLibrary( BinaryDependenciesFile::fromQFileInfo(libA) );
      result2.addLibrary( BinaryDependenciesFile::fromQFileInfo(libA) );
      resultList.addResult(result1);
      resultList.addResult(result2);

      pathList = getLibrariesAbsoluteFilePathList(resultList);

      REQUIRE( pathList.size() == 1 );
      REQUIRE( pathList.contains( QLatin1String("/opt/libA.so") ) );
    }
  }

  SECTION("2 results having KERNEL32.DLL and kernel32.dll")
  {
    REQUIRE( false );
  }
}

TEST_CASE("getLibrariesToRedistribute")
{
  const auto os = OperatingSystem::Linux;
  RPath rpath;
  BinaryDependenciesResultList resultList(os);
  std::vector<BinaryDependenciesResultLibrary> libraries;

  SECTION("1 result")
  {
    QFileInfo app( QLatin1String("/opt/app") );
    BinaryDependenciesResult result(app, os);

    SECTION("1 library to redistribute")
    {
      QFileInfo libA( QLatin1String("/opt/libA.so") );
      result.addFoundLibrary(libA, rpath);
      resultList.addResult(result);

      libraries = getLibrariesToRedistribute(resultList);

      REQUIRE( libraries.size() == 1 );
      REQUIRE( libraryListContainsPath(libraries, "/opt/libA.so") );
    }

    SECTION("1 library to redistribute and 1 not")
    {
      QFileInfo MyLib( QLatin1String("/opt/MyLib.so") );
      result.addFoundLibrary(MyLib, rpath);
      QFileInfo libc( QLatin1String("/lib/libc.so") );
      result.addLibraryToNotRedistribute(libc);
      resultList.addResult(result);

      libraries = getLibrariesToRedistribute(resultList);

      REQUIRE( libraries.size() == 1 );
      REQUIRE( libraryListContainsPath(libraries, "/opt/MyLib.so") );
    }
  }

  SECTION("2 results")
  {
    QFileInfo app1( QLatin1String("/opt/app1") );
    BinaryDependenciesResult result1(app1, os);
    QFileInfo app2( QLatin1String("/opt/app2") );
    BinaryDependenciesResult result2(app2, os);

    SECTION("both results have different libraries do redistribute")
    {
      QFileInfo MyLib1( QLatin1String("/opt/MyLib1.so") );
      result1.addFoundLibrary(MyLib1, rpath);
      resultList.addResult(result1);
      QFileInfo MyLib2( QLatin1String("/opt/MyLib2.so") );
      result2.addFoundLibrary(MyLib2, rpath);
      resultList.addResult(result2);
      REQUIRE( resultList.resultCount() == 2 );

      libraries = getLibrariesToRedistribute(resultList);

      REQUIRE( libraries.size() == 2 );
      REQUIRE( libraryListContainsPath(libraries, "/opt/MyLib1.so") );
      REQUIRE( libraryListContainsPath(libraries, "/opt/MyLib2.so") );
    }

    SECTION("both results have the same library to redistribute")
    {
      QFileInfo MyLib( QLatin1String("/opt/MyLib.so") );
      result1.addFoundLibrary(MyLib, rpath);
      resultList.addResult(result1);
      result2.addFoundLibrary(MyLib, rpath);
      resultList.addResult(result2);
      REQUIRE( resultList.resultCount() == 2 );

      libraries = getLibrariesToRedistribute(resultList);

      REQUIRE( libraries.size() == 1 );
      REQUIRE( libraryListContainsPath(libraries, "/opt/MyLib.so") );
    }
  }
}

TEST_CASE("getLibrariesToRedistribute_WindowsSpecifics")
{
  const auto os = OperatingSystem::Windows;
  RPath rpath;
  BinaryDependenciesResultList resultList(os);
  std::vector<BinaryDependenciesResultLibrary> libraries;

  QFileInfo app1( QLatin1String("/opt/app1") );
  BinaryDependenciesResult result1(app1, os);
  QFileInfo app2( QLatin1String("/opt/app2") );
  BinaryDependenciesResult result2(app2, os);

  SECTION("both result have the same library to redistribute with different cases")
  {
    QFileInfo LIBA( QLatin1String("/opt/LIBA.DLL") );
    result1.addFoundLibrary(LIBA, rpath);
    QFileInfo liba( QLatin1String("/opt/liba.dll") );
    result2.addFoundLibrary(liba, rpath);
    resultList.addResult(result1);
    resultList.addResult(result2);
    REQUIRE( resultList.resultCount() == 2 );

    libraries = getLibrariesToRedistribute(resultList);

    REQUIRE( libraries.size() == 1 );
  }
}
