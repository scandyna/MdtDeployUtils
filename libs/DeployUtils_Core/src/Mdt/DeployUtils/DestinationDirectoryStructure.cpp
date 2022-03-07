/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#include "DestinationDirectoryStructure.h"
#include <QLatin1String>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

void DestinationDirectoryStructure::setExecutablesDirectory(const QString & directory) noexcept
{
  assert( !directory.trimmed().isEmpty() );

  mExecutablesDirectory = directory;
}

void DestinationDirectoryStructure::setSharedLibrariesDirectory(const QString & directory) noexcept
{
  assert( !directory.trimmed().isEmpty() );

  mSharedLibrariesDirectory = directory;
}

void DestinationDirectoryStructure::setQtPluginsRootDirectory(const QString & path) noexcept
{
  assert( !path.trimmed().isEmpty() );

  mQtPluginsRootRelativePath = path;
}

QString DestinationDirectoryStructure::executablesDirectoryFromOs(OperatingSystem os) noexcept
{
  assert(os != OperatingSystem::Unknown);

  switch(os){
    case OperatingSystem::Linux:
    case OperatingSystem::Windows:
      return QLatin1String("bin");
    case OperatingSystem::Unknown:
      break;
  }

  return QString();
}

QString DestinationDirectoryStructure::sharedLibrariesDirectoryFromOs(OperatingSystem os) noexcept
{
  assert(os != OperatingSystem::Unknown);

  switch(os){
    case OperatingSystem::Linux:
      return QLatin1String("lib");
    case OperatingSystem::Windows:
      return QLatin1String("bin");
    case OperatingSystem::Unknown:
      break;
  }

  return QString();
}

QString DestinationDirectoryStructure::qtPluginsRootDirectoryFromOs(OperatingSystem os) noexcept
{
  assert(os != OperatingSystem::Unknown);

  switch(os){
    case OperatingSystem::Linux:
      return QLatin1String("plugins");
    case OperatingSystem::Windows:
      return QLatin1String("bin");
    case OperatingSystem::Unknown:
      break;
  }

  return QString();
}

DestinationDirectoryStructure DestinationDirectoryStructure::fromOperatingSystem(OperatingSystem os) noexcept
{
  assert(os != OperatingSystem::Unknown);

  DestinationDirectoryStructure structure;

  structure.setExecutablesDirectory( executablesDirectoryFromOs(os) );
  structure.setSharedLibrariesDirectory( sharedLibrariesDirectoryFromOs(os) );
  structure.setQtPluginsRootDirectory( qtPluginsRootDirectoryFromOs(os) );

  return structure;
}

}} // namespace Mdt{ namespace DeployUtils{
