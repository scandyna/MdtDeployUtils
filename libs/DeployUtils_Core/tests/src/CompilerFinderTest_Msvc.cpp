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
#include "Mdt/DeployUtils/CompilerFinder.h"
#include "Mdt/DeployUtils/Platform.h"
#include "TestFileUtils.h"
#include "TestUtils.h"
#include <QString>

#include <QDebug>

using namespace Mdt::DeployUtils;

TEST_CASE("hasInstallDir")
{
  CompilerFinder finder;

  SECTION("default constructed")
  {
    REQUIRE( !finder.hasInstallDir() );
  }
}

TEST_CASE("findFromCxxCompilerPath")
{
  qDebug() << "CXX_COMPILER_PATH: " << CXX_COMPILER_PATH;
  
  CompilerFinder finder;
  finder.findFromCxxCompilerPath( QString::fromLocal8Bit(CXX_COMPILER_PATH) );
  REQUIRE( finder.hasInstallDir() );
  REQUIRE( !finder.installDir().isEmpty() );
}

TEST_CASE("setInstallDir")
{
  CompilerFinder finder;
  finder.findFromCxxCompilerPath( QString::fromLocal8Bit(CXX_COMPILER_PATH) );
  REQUIRE( finder.hasInstallDir() );

  CompilerFinder finderToTest;
  finderToTest.setInstallDir( finder.installDir(), Compiler::Msvc );
  REQUIRE( finderToTest.hasInstallDir() );
  REQUIRE( !finderToTest.installDir().isEmpty() );
}

TEST_CASE("findRedistDirectory")
{
  const auto platform = Platform::nativePlatform();
  CompilerFinder finder;
  finder.findFromCxxCompilerPath( QString::fromLocal8Bit(CXX_COMPILER_PATH) );
  REQUIRE( finder.hasInstallDir() );

  const QString redistPath = finder.findRedistDirectory( platform.processorISA(), currentBuildType() );
  REQUIRE( isExistingDirectory(redistPath) );
}
