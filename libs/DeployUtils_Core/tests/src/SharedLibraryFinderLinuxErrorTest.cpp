/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2023 Philippe Steinmann.
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
#include "SharedLibraryFinderLinuxTestCommon.h"
#include "Mdt/DeployUtils/QtDistributionDirectory.h"
#include "Mdt/DeployUtils/SharedLibraryFinderLinux.h"
#include "Mdt/DeployUtils/RPath.h"
#include <QLatin1String>
#include <QString>
#include <memory>

using namespace Mdt::DeployUtils;

TEST_CASE("findLibraryAbsolutePath")
{
  QString libraryName;
  auto isExistingSharedLibraryOp = std::make_shared<TestIsExistingSharedLibrary>();
  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  SharedLibraryFinderLinux finder(isExistingSharedLibraryOp, qtDistributionDirectory);

  SECTION("libA.so - pathList:/tmp - not exists in given pathList")
  {
    auto dependentFile = makeBinaryDependenciesFileFromUtf8Path("/tmp/executable");
    libraryName = QLatin1String("libA.so");
    finder.setSearchPathList( makePathListFromUtf8Paths({"/tmp"}) );
    isExistingSharedLibraryOp->setExistingSharedLibraries({"/tmp/libB.so","/opt/libA.so"});

    REQUIRE_THROWS_AS( finder.findLibraryAbsolutePath(libraryName, dependentFile), FindDependencyError );
  }
}
