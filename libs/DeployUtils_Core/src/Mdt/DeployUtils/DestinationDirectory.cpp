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
#include "DestinationDirectory.h"
#include <QStringBuilder>
#include <QLatin1Char>
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

QString DestinationDirectory::executablesDirectoryPath() const noexcept
{
  return QDir::cleanPath( mPath % QLatin1Char('/') % mStructure.executablesDirectory() );
}

QString DestinationDirectory::sharedLibrariesDirectoryPath() const noexcept
{
  return QDir::cleanPath( mPath % QLatin1Char('/') % mStructure.sharedLibrariesDirectory() );
}

QString DestinationDirectory::qtPluginsRootDirectoryPath() const noexcept
{
  return QDir::cleanPath( mPath % QLatin1Char('/') % mStructure.qtPluginsRootDirectory() );
}

DestinationDirectory DestinationDirectory::fromPathAndStructure(const QString & path, const DestinationDirectoryStructure & structure) noexcept
{
  assert( !path.trimmed().isEmpty() );
  assert( !structure.isNull() );

  return DestinationDirectory(path, structure);
}

DestinationDirectory DestinationDirectory::fromPathAndOs(const QString & path, OperatingSystem os) noexcept
{
  assert( !path.trimmed().isEmpty() );
  assert(os != OperatingSystem::Unknown);

  return fromPathAndStructure( path, DestinationDirectoryStructure::fromOperatingSystem(os) );
}

}} // namespace Mdt{ namespace DeployUtils{
