/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/FileCopierFile.h"
#include <QString>
#include <QLatin1String>
#include <QFileInfo>

using namespace Mdt::DeployUtils;

TEST_CASE("set_SourceAndDestimation")
{
  FileCopierFile copierFile;
  QFileInfo source;
  QFileInfo destination;

  source.setFile( QLatin1String("/tmp/source.txt") );
  destination.setFile( QLatin1String("/tmp/destination.txt") );

  copierFile.setSourceFileInfo(source);
  copierFile.setDestinationFileInfo(destination);

  REQUIRE( copierFile.sourceFileInfo().fileName() == QLatin1String("source.txt") );
  REQUIRE( copierFile.sourceAbsoluteFilePath() == makeAbsolutePath("/tmp/source.txt") );
  REQUIRE( copierFile.destinationFileInfo().fileName() == QLatin1String("destination.txt") );
  REQUIRE( copierFile.destinationAbsoluteFilePath() == makeAbsolutePath("/tmp/destination.txt") );
}

TEST_CASE("hasBeenCopied")
{
  FileCopierFile copierFile;

  SECTION("default")
  {
    REQUIRE( !copierFile.hasBeenCopied() );
  }

  SECTION("set as beend copied")
  {
    copierFile.setAsBeenCopied();

    REQUIRE( copierFile.hasBeenCopied() );
  }
}
