/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
#include "Mdt/DeployUtils/CopySharedLibrariesTargetDependsOn.h"
#include <QTemporaryDir>

#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/CMakeStyleMessageLogger.h"

using namespace Mdt::DeployUtils;

TEST_CASE("sandbox")
{
  QTemporaryDir destinationDir;
  REQUIRE( destinationDir.isValid() );

  CopySharedLibrariesTargetDependsOnRequest request;

  request.targetFilePath = QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH);
  request.searchPrefixPathList = getTestPrefixPath(PREFIX_PATH);
  request.compilerLocationType = CompilerLocationType::CompilerPath;
  request.compilerLocationValue = QString::fromLocal8Bit(CXX_COMPILER_PATH);
  request.destinationDirectoryPath = destinationDir.path();
  request.overwriteBehavior = OverwriteBehavior::Overwrite;
  request.removeRpath = false;

  /// \todo just for sandbox
  MessageLogger messageLogger;
  MessageLogger::setBackend<CMakeStyleMessageLogger>();

  CopySharedLibrariesTargetDependsOn useCase;

  QObject::connect(&useCase, &CopySharedLibrariesTargetDependsOn::message, MessageLogger::info);
  QObject::connect(&useCase, &CopySharedLibrariesTargetDependsOn::verboseMessage, MessageLogger::info);

  useCase.execute(request);
}
