/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#include "Mdt/DeployUtils/QtModule.h"
#include <QLatin1String>
#include <QString>

using namespace Mdt::DeployUtils;

TEST_CASE("qtModuleFromLibraryName")
{
  QString libraryName;

  SECTION("libm.so")
  {
    libraryName = QLatin1String("libm.so");

    REQUIRE_THROWS_AS( qtModuleFromLibraryName(libraryName), UnknownQtModuleError );
  }

  SECTION("libQt.so")
  {
    libraryName = QLatin1String("libQt.so");

    REQUIRE_THROWS_AS( qtModuleFromLibraryName(libraryName), UnknownQtModuleError );
  }

  SECTION("libQtUnknown.so")
  {
    libraryName = QLatin1String("libQtUnknown.so");

    REQUIRE_THROWS_AS( qtModuleFromLibraryName(libraryName), UnknownQtModuleError );
  }
}
