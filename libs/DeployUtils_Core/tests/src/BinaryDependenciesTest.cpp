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
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/DeployUtils/PathList.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/CompilerFinder.h"
// #include "Mdt/DeployUtils/Platform.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <QFileInfo>
#include <string>
#include <vector>
#include <memory>

#include <iostream>
// #include <QDebug>

using namespace Mdt::DeployUtils;


TEST_CASE("findDependencies")
{
  BinaryDependencies solver;

  MessageLogger messageLogger;
  QObject::connect(&solver, &BinaryDependencies::message, MessageLogger::info);
  QObject::connect(&solver, &BinaryDependencies::verboseMessage, MessageLogger::info);

#ifdef COMPILER_IS_MSVC
  auto compilerFinder = std::make_shared<CompilerFinder>();
  solver.setCompilerFinder(compilerFinder);
#endif // #ifdef COMPILER_IS_MSVC

  PathList searchFirstPathPrefixList = PathList::fromStringList( getTestPrefixPath(PREFIX_PATH) );
  QStringList dependencies;

  SECTION("Executable")
  {
    const QFileInfo target( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    dependencies = solver.findDependencies(target, searchFirstPathPrefixList);

    std::cout << "deps:\n" << dependencies.join( QLatin1Char('\n') ).toStdString() << std::endl;

    REQUIRE( containsTestSharedLibrary(dependencies) );
    REQUIRE( containsQt5Core(dependencies) );
  }
}
