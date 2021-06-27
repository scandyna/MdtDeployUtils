/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#include "BinaryDependenciesTestCommon.h"
#include "TestUtils.h"
#include "Mdt/DeployUtils/FindDependencyError.h"

TEST_CASE("findLibraryAbsolutePath")
{
  using Impl::findLibraryAbsolutePath;

  PathList pathList;
  TestIsExistingSharedLibrary isExistingSharedLibraryOp;
  const auto platform = Platform::nativePlatform();
//   ExecutableFileInfo library;

  SECTION("libA.so - pathList:/tmp - not exists in given pathList")
  {
    pathList.appendPathList( {QLatin1String("/tmp")} );
    isExistingSharedLibraryOp.setExistingSharedLibraries({"/tmp/libB.so","/opt/libA.so"});
//     library = findLibraryAbsolutePath( QLatin1String("libA.so"), pathList, isExistingSharedLibraryOp );
    REQUIRE_THROWS_AS( findLibraryAbsolutePath( QLatin1String("libA.so"), pathList, platform, isExistingSharedLibraryOp ), FindDependencyError );
  }
}
