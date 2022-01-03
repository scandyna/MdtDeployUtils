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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/PathList.h"
#include <QTemporaryDir>
#include <QDir>
#include <vector>
#include <string>

using namespace Mdt::DeployUtils;

/*
 * NOTE: some tests for PathList are also in PathTest.cpp
 */

PathList makePathList(const std::vector<std::string> & strList)
{
  return PathList::fromStringList( qStringListFromUtf8Strings(strList) );
}


TEST_CASE("removeNonExistingDirectories")
{
  PathList pathList;
  QTemporaryDir root;
  REQUIRE( root.isValid() );

  SECTION("/existing")
  {
    const QString existing = makePath(root, "/existing");
    REQUIRE( createDirectoryFromPath(existing) );
    pathList.appendPath(existing);

    pathList.removeNonExistingDirectories();

    REQUIRE( pathList.toStringList().count() == 1 );
    REQUIRE( pathList.containsPath(existing) );
  }

  SECTION("/nonExisting")
  {
    const QString nonExisting = makePath(root, "/nonExisting");
    pathList.appendPath(nonExisting);

    pathList.removeNonExistingDirectories();

    REQUIRE( pathList.isEmpty() );
  }

  SECTION("/existing,/nonExisting")
  {
    const QString existing = makePath(root, "/existing");
    REQUIRE( createDirectoryFromPath(existing) );
    const QString nonExisting = makePath(root, "/nonExisting");
    pathList.appendPath(existing);
    pathList.appendPath(nonExisting);

    pathList.removeNonExistingDirectories();

    REQUIRE( pathList.toStringList().count() == 1 );
    REQUIRE( pathList.containsPath(existing) );
    REQUIRE( !pathList.containsPath(nonExisting) );
  }
}
