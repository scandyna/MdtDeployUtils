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

// #include "TestFileUtils.h"

#include "Mdt/DeployUtils/BinaryDependenciesResultList.h"
#include <QLatin1String>
#include <QFileInfo>

using namespace Mdt::DeployUtils;


TEST_CASE("addResult")
{
  const auto os = OperatingSystem::Linux;
  BinaryDependenciesResultList resultList(os);

  SECTION("empty list")
  {
    REQUIRE( resultList.resultCount() == 0 );
    REQUIRE( resultList.isEmpty() );
  }

  SECTION("add 1 empty result")
  {
    QFileInfo app( QLatin1String("/opt/app") );
    BinaryDependenciesResult result(app, os);

    resultList.addResult(result);

    REQUIRE( resultList.resultCount() == 1 );
    REQUIRE( !resultList.isEmpty() );
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
