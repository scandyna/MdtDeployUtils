/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2022 Philippe Steinmann.
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
#include "CopySharedLibrariesTargetDependsOn.h"
#include "SharedLibrariesDeployer.h"
#include "PathList.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

void CopySharedLibrariesTargetDependsOn::execute(const CopySharedLibrariesTargetDependsOnRequest & request)
{
  assert( !request.targetFilePath.trimmed().isEmpty() );
  assert( !request.destinationDirectoryPath.trimmed().isEmpty() );

  SharedLibrariesDeployer shLibDeployer;
  connect(&shLibDeployer, &SharedLibrariesDeployer::statusMessage, this, &CopySharedLibrariesTargetDependsOn::statusMessage);
  connect(&shLibDeployer, &SharedLibrariesDeployer::verboseMessage, this, &CopySharedLibrariesTargetDependsOn::verboseMessage);
  connect(&shLibDeployer, &SharedLibrariesDeployer::debugMessage, this, &CopySharedLibrariesTargetDependsOn::debugMessage);

  shLibDeployer.setSearchPrefixPathList( PathList::fromStringList(request.searchPrefixPathList) );
  shLibDeployer.setOverwriteBehavior(request.overwriteBehavior);
  shLibDeployer.setRemoveRpath(request.removeRpath);

  if( !request.compilerLocation.isNull() ){
    shLibDeployer.setCompilerLocation(request.compilerLocation);
  }

  shLibDeployer.copySharedLibrariesTargetDependsOn(request.targetFilePath, request.destinationDirectoryPath);
}

}} // namespace Mdt{ namespace DeployUtils{
