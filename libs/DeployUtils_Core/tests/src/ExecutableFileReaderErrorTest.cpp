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
#include "Mdt/DeployUtils/ExecutableFileReader.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QString>

using namespace Mdt::DeployUtils;

Platform getNonNativePlatform()
{
  Platform nativePlatform = Platform::nativePlatform();

  if( nativePlatform.executableFileFormat() == ExecutableFileFormat::Elf ){
    return Platform( OperatingSystem::Windows, ExecutableFileFormat::Pe, nativePlatform.compiler(), nativePlatform.processorISA() );
  }
  return Platform( OperatingSystem::Linux, ExecutableFileFormat::Elf, nativePlatform.compiler(), nativePlatform.processorISA() );
}


TEST_CASE("open_wrong_platform_file")
{
  ExecutableFileReader reader;
  const QString file = QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH);
  const Platform platform = getNonNativePlatform();
  REQUIRE_THROWS_AS(reader.openFile(file, platform), FileOpenError);
}
