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
#include "TestUtils.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies/DiscoveredDependenciesList.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;
using namespace Mdt::DeployUtils::Impl::BinaryDependencies;

TEST_CASE("containsDependentFile")
{
  DiscoveredDependenciesList list(OperatingSystem::Linux);
  GraphFile dependentFile;
  QStringList dependencies;

  SECTION("empty list does not contain libA.so")
  {
    dependentFile = GraphFile::fromLibraryName( QLatin1String("libA.so") );

    REQUIRE( !list.containsDependentFile(dependentFile) );
  }

  SECTION("contains libA.so")
  {
    dependentFile = GraphFile::fromLibraryName( QLatin1String("libA.so") );
    dependencies = qStringListFromUtf8Strings({"libB"});
    list.setDirectDependenciesFileNames(dependentFile, dependencies);

    REQUIRE( list.containsDependentFile(dependentFile) );
  }
}

TEST_CASE("setDirectDependenciesFileNames")
{
  DiscoveredDependenciesList list(OperatingSystem::Linux);
  GraphFile dependentFile;
  QStringList dependencies;

  SECTION("libA -> libB")
  {
    dependentFile = GraphFile::fromLibraryName( QLatin1String("libA") );
    dependencies = qStringListFromUtf8Strings({"libB"});
    list.setDirectDependenciesFileNames(dependentFile, dependencies);

    REQUIRE( list.size() == 1 );
    REQUIRE( list.at(0).dependentFileName() == QLatin1String("libA") );
    REQUIRE( list.at(0).dependenciesFileNames() == dependencies );

    /*
     * Setting dependencies for a existing file
     * does not change anything
     */
    list.setDirectDependenciesFileNames(dependentFile, dependencies);

    REQUIRE( list.size() == 1 );
    REQUIRE( list.at(0).dependentFileName() == QLatin1String("libA") );
    REQUIRE( list.at(0).dependenciesFileNames() == dependencies );
  }

  SECTION("add a file without dependencies does nothing")
  {
    dependentFile = GraphFile::fromLibraryName( QLatin1String("libA") );
    list.setDirectDependenciesFileNames(dependentFile, dependencies);

    REQUIRE( list.isEmpty() );
  }
}
