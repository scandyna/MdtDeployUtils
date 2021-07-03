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
#include "TestUtils.h"
#include "Mdt/DeployUtils/BinaryDependencies.h"
#include "Mdt/DeployUtils/PathList.h"
// #include "Mdt/DeployUtils/Platform.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <QFileInfo>
#include <string>
#include <vector>

#include <iostream>
// #include <QDebug>

using namespace Mdt::DeployUtils;

TEST_CASE("buildSearchPathListLinux")
{
  BinaryDependencies binaryDependencies;
  PathList searchFirstPathPrefixList;
  PathList searchPathList;

  SECTION("x86")
  {
    searchPathList = binaryDependencies.buildSearchPathListLinux(searchFirstPathPrefixList, ProcessorISA::X86_32);
    REQUIRE( !searchPathList.isEmpty() );
  }

  SECTION("x86_64")
  {
    searchPathList = binaryDependencies.buildSearchPathListLinux(searchFirstPathPrefixList, ProcessorISA::X86_64);
    REQUIRE( !searchPathList.isEmpty() );
  }
}

TEST_CASE("buildSearchPathListWindows")
{
  BinaryDependencies binaryDependencies;
  PathList searchFirstPathPrefixList;
  PathList searchPathList;

  QFileInfo binaryFilePath( QLatin1String("/some/path/to/app.exe") );

  SECTION("x86")
  {
    searchPathList = binaryDependencies.buildSearchPathListWindows(binaryFilePath, searchFirstPathPrefixList, ProcessorISA::X86_32);
    REQUIRE( !searchPathList.isEmpty() );
  }

  SECTION("x86_64")
  {
    searchPathList = binaryDependencies.buildSearchPathListWindows(binaryFilePath, searchFirstPathPrefixList, ProcessorISA::X86_64);
    REQUIRE( !searchPathList.isEmpty() );
  }
}

TEST_CASE("findDependencies")
{
  BinaryDependencies solver;

  PathList searchFirstPathPrefixList = PathList::fromStringList( getTestPrefixPath(PREFIX_PATH) );
  /// pathList.appendPathList( PathList::getSystemLibraryPathList() );

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

TEST_CASE("Windows_sandbox")
{
  BinaryDependencies solver;

  PathList searchFirstPathPrefixList;
  searchFirstPathPrefixList.appendPath( QLatin1String("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/Tools/mingw492_32/") );
  searchFirstPathPrefixList.appendPath( QLatin1String("/home/philippe/.wine/drive_c/windows/syswow64/") );
  searchFirstPathPrefixList.appendPath( QLatin1String("/home/philippe/.wine/drive_c/windows/system32/") );

  QStringList dependencies;

  SECTION("Qt5")
  {
    const QFileInfo target( QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Widgets.dll") );
    dependencies = solver.findDependencies(target, searchFirstPathPrefixList);

    std::cout << "deps:\n" << dependencies.join( QLatin1Char('\n') ).toStdString() << std::endl;

//     REQUIRE( containsTestSharedLibrary(dependencies) );
    REQUIRE( containsQt5Core(dependencies) );
  }
}
