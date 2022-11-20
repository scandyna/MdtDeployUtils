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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/BinaryDependenciesResult.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include <QLatin1String>
#include <QFileInfo>

using namespace Mdt::DeployUtils;

TEST_CASE("construct")
{
  QFileInfo file( QLatin1String("/opt/project/app") );

  BinaryDependenciesResult result(file);

  REQUIRE( result.target().absoluteFilePath() == makeAbsolutePath("/opt/project/app") );
  REQUIRE( result.isSolved() );
}

TEST_CASE("addLibrary")
{
  QFileInfo file( QLatin1String("/opt/project/app") );
  BinaryDependenciesResult result(file);

  SECTION("add a library with its full path (found)")
  {
    QFileInfo libA( QLatin1String("/opt/libA.so") );
    result.addLibrary( BinaryDependenciesFile::fromQFileInfo(libA) );

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( result.containsLibraryName( QLatin1String("libA.so") ) );
    REQUIRE( result.isSolved() );
  }

  SECTION("add a library that was not found")
  {
    QFileInfo libA( QLatin1String("libA.so") );
    result.addLibrary( BinaryDependenciesFile::fromLibraryName(libA) );

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( result.containsLibraryName( QLatin1String("libA.so") ) );
    REQUIRE( !result.isSolved() );
  }

  SECTION("add a library 2x")
  {
    QFileInfo libA( QLatin1String("libA.so") );
    result.addLibrary( BinaryDependenciesFile::fromLibraryName(libA) );
    result.addLibrary( BinaryDependenciesFile::fromLibraryName(libA) );

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( result.containsLibraryName( QLatin1String("libA.so") ) );
    REQUIRE( !result.isSolved() );
  }

  SECTION("add a library 2x but with different case")
  {
    QFileInfo libA( QLatin1String("libA.DLL") );
    QFileInfo liba( QLatin1String("liba.dll") );

    result.addLibrary( BinaryDependenciesFile::fromLibraryName(libA) );
    result.addLibrary( BinaryDependenciesFile::fromLibraryName(liba) );

    REQUIRE( result.libraryCount() == 1 );
    REQUIRE( result.containsLibraryName( QLatin1String("liba.dll") ) );
    REQUIRE( result.containsLibraryName( QLatin1String("libA.dll") ) );
    REQUIRE( result.containsLibraryName( QLatin1String("libA.DLL") ) );
    REQUIRE( !result.isSolved() );
  }
}

TEST_CASE("getLibrariesAbsoluteFilePathList")
{
  BinaryDependenciesResultList resultList;
  QStringList pathList;

  SECTION("empty list of results")
  {
    pathList = getLibrariesAbsoluteFilePathList(resultList);

    REQUIRE( pathList.isEmpty() );
  }

  SECTION("1 result")
  {
    QFileInfo app( QLatin1String("/opt/app") );
    BinaryDependenciesResult result(app);

    SECTION("1 library")
    {
      QFileInfo libA( QLatin1String("/opt/libA.so") );
      result.addLibrary( BinaryDependenciesFile::fromQFileInfo(libA) );
      resultList.push_back(result);

      pathList = getLibrariesAbsoluteFilePathList(resultList);

      REQUIRE( pathList.size() == 1 );
      REQUIRE( pathList.contains( QLatin1String("/opt/libA.so") ) );
    }
  }

  SECTION("2 results")
  {
    QFileInfo app1( QLatin1String("/opt/app1") );
    BinaryDependenciesResult result1(app1);
    QFileInfo app2( QLatin1String("/opt/app2") );
    BinaryDependenciesResult result2(app2);

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
      resultList.push_back(result1);
      resultList.push_back(result2);

      pathList = getLibrariesAbsoluteFilePathList(resultList);

      REQUIRE( pathList.size() == 1 );
      REQUIRE( pathList.contains( QLatin1String("/opt/libA.so") ) );
    }
  }
}
