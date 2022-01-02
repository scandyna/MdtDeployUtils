/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#include "BinaryDependenciesTestCommon.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include <QLatin1String>

using namespace Mdt::DeployUtils;

TEST_CASE("default constructed")
{
  BinaryDependenciesFile file;

  REQUIRE( file.isNull() );
}

TEST_CASE("fromQFileInfo")
{
  QFileInfo fi( QLatin1String("/path/to/some/arbitraryFile") );

  auto file = BinaryDependenciesFile::fromQFileInfo(fi);

  REQUIRE( file.fileName() == QLatin1String("arbitraryFile") );
  REQUIRE( file.absoluteDirectoryPath() == makeAbsolutePath("/path/to/some") );
  REQUIRE( !file.isNull() );
}

TEST_CASE("removeDependenciesFileNames")
{
  QFileInfo fi( QLatin1String("/path/to/some/arbitraryFile") );
  auto file = BinaryDependenciesFile::fromQFileInfo(fi);

  const auto pred = [](const QString & libraryName){
    return libraryName == QLatin1String("libR.so");
  };

  SECTION("no dependencies")
  {
    file.removeDependenciesFileNames(pred);

    REQUIRE( file.dependenciesFileNames().isEmpty() );
  }

  SECTION("libA.so libR.so")
  {
    file.setDependenciesFileNames( qStringListFromUtf8Strings({"libA.so","libR.so"}) );

    file.removeDependenciesFileNames(pred);

    REQUIRE( file.dependenciesFileNames().size() == 1 );
    REQUIRE( file.dependenciesFileNames().at(0) == QLatin1String("libA.so") );
  }
}
