/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2023 Philippe Steinmann.
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
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/DeployUtils/QtDistributionDirectory.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/CompilerFinder.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <QFileInfo>
#include <string>
#include <vector>
#include <memory>

using namespace Mdt::DeployUtils;


TEST_CASE("findDependencies")
{
  BinaryDependencies solver;

  MessageLogger messageLogger;
  QObject::connect(&solver, &BinaryDependencies::message, MessageLogger::info);
  QObject::connect(&solver, &BinaryDependencies::verboseMessage, MessageLogger::info);
//   QObject::connect(&solver, &BinaryDependencies::debugMessage, MessageLogger::info);

#ifdef COMPILER_IS_MSVC
  auto compilerFinder = std::make_shared<CompilerFinder>();
  compilerFinder->findFromCxxCompilerPath( QString::fromLocal8Bit(CXX_COMPILER_PATH) );
  solver.setCompilerFinder(compilerFinder);
#endif // #ifdef COMPILER_IS_MSVC

  auto qtDistributionDirectory = std::make_shared<QtDistributionDirectory>();
  PathList searchFirstPathPrefixList = PathList::fromStringList( getTestPrefixPath(PREFIX_PATH) );

  SECTION("Executable")
  {
    const QFileInfo target( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    const BinaryDependenciesResult dependencies = solver.findDependencies(target, searchFirstPathPrefixList, qtDistributionDirectory);

    REQUIRE( containsTestSharedLibrary(dependencies) );
    REQUIRE( containsQt5Core(dependencies) );
  }

  SECTION("solve 2 targets")
  {
    const QFileInfo library( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    const QFileInfo app( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );

    const BinaryDependenciesResultList dependenciesList = solver.findDependencies({library, app}, searchFirstPathPrefixList, qtDistributionDirectory);

    REQUIRE( dependenciesList.resultCount() == 2 );

    const auto libraryResult = dependenciesList.findResultForTargetName( library.fileName() );
    REQUIRE( libraryResult.has_value() );
    REQUIRE( libraryResult->target().fileName() == library.fileName() );
    REQUIRE( containsQt5Core(*libraryResult) );

    const auto appResult = dependenciesList.findResultForTargetName( app.fileName() );
    REQUIRE( appResult.has_value() );
    REQUIRE( appResult->target().fileName() == app.fileName() );
    REQUIRE( containsTestSharedLibrary(*appResult) );
    REQUIRE( containsQt5Core(*appResult) );
  }
}
