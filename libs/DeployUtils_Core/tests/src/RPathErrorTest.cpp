/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/DeployUtils/RPathElf.h"
#include <QLatin1String>

using Mdt::DeployUtils::RPathEntry;
using Mdt::DeployUtils::RPath;
using Mdt::DeployUtils::RPathElf;
using Mdt::DeployUtils::RPathFormatError;

TEST_CASE("ELF_rPathFromString")
{
  RPath rpath;

  SECTION("$ORIGINa")
  {
    REQUIRE_THROWS_AS( rpath = RPathElf::rPathFromString( QLatin1String("$ORIGINa") ), RPathFormatError);
  }

  SECTION("${ORIGIN}a")
  {
    REQUIRE_THROWS_AS( rpath = RPathElf::rPathFromString( QLatin1String("${ORIGIN}a") ), RPathFormatError);
  }
}
