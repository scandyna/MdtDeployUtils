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
#include "Mdt/DeployUtils/MsvcFinder.h"
#include <QLatin1String>
#include <QFileInfo>

#include <QDebug>

using namespace Mdt::DeployUtils;

TEST_CASE("sandbox")
{
  qDebug() << "MSVC version: " << MSVC_VERSION;
}

TEST_CASE("findMsvcRoot")
{
  MsvcFinder finder;

  const auto version = MsvcVersion::fromMscVer( QString( QLatin1String(MSVC_VERSION) ).toInt() );
  QString msvcRoot = finder.findMsvcRoot(version);
  REQUIRE( QFileInfo::exists(msvcRoot) );
}
