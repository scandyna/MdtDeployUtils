/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2022 Philippe Steinmann.
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
#include "Mdt/DeployUtils/SharedLibraryFinderWindows.h"

using namespace Mdt::DeployUtils;

TEST_CASE("buildSearchPathListWindows")
{
  PathList searchFirstPathPrefixList;
  PathList searchPathList;
  std::shared_ptr<CompilerFinder> compilerFinder;

  QFileInfo binaryFilePath( QLatin1String("/some/path/to/app.exe") );

  SECTION("x86 (compiler finder is null)")
  {
    searchPathList = SharedLibraryFinderWindows::buildSearchPathList(binaryFilePath, searchFirstPathPrefixList, ProcessorISA::X86_32, compilerFinder);
    REQUIRE( !searchPathList.isEmpty() );
  }

  SECTION("x86_64 (compiler finder is null)")
  {
    searchPathList = SharedLibraryFinderWindows::buildSearchPathList(binaryFilePath, searchFirstPathPrefixList, ProcessorISA::X86_64, compilerFinder);
    REQUIRE( !searchPathList.isEmpty() );
  }
}
