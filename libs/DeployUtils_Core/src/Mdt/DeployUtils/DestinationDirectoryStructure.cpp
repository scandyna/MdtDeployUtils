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
#include <QLatin1Char>
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

void DestinationDirectoryStructure::setExecutablesDirectory(const QString & directory) noexcept
{
  assert( !directory.trimmed().isEmpty() );
  assert( !QDir::isAbsolutePath(directory) );

  mExecutablesDirectory = QDir::cleanPath(directory);
}

void DestinationDirectoryStructure::setSharedLibrariesDirectory(const QString & directory) noexcept
{
  assert( !directory.trimmed().isEmpty() );
  assert( !QDir::isAbsolutePath(directory) );

  mSharedLibrariesDirectory = QDir::cleanPath(directory);
}

void DestinationDirectoryStructure::setQtPluginsRootDirectory(const QString & path) noexcept
{
  assert( !path.trimmed().isEmpty() );
  assert( QDir::isRelativePath(path) );

  mQtPluginsRootRelativePath = QDir::cleanPath(path);
}

QString DestinationDirectoryStructure::qtPluginsToSharedLibrariesRelativePath() const noexcept
{
  assert( !mSharedLibrariesDirectory.isEmpty() );
  assert( !mQtPluginsRootRelativePath.isEmpty() );

  assert( !mSharedLibrariesDirectory.startsWith( QLatin1Char('/') ) );
  assert( !mQtPluginsRootRelativePath.startsWith( QLatin1Char('/') ) );
  assert( !mSharedLibrariesDirectory.endsWith( QLatin1Char('/') ) );
  assert( !mQtPluginsRootRelativePath.endsWith( QLatin1Char('/') ) );

  QString relativePath;

  /*
   * Qt plugins root is given here.
   * But, the relative path must be from the specific plugins sub-directory (plugins/platforms)
   * So, add 1 level to the A directory
   */
  return directoryAtoBrelativePath(mQtPluginsRootRelativePath, mSharedLibrariesDirectory, 1);
}

QString DestinationDirectoryStructure::executablesToSharedLibrariesRelativePath() const noexcept
{
  assert( !mSharedLibrariesDirectory.isEmpty() );
  assert( !mExecutablesDirectory.isEmpty() );

  assert( !mSharedLibrariesDirectory.startsWith( QLatin1Char('/') ) );
  assert( !mExecutablesDirectory.startsWith( QLatin1Char('/') ) );
  assert( !mSharedLibrariesDirectory.endsWith( QLatin1Char('/') ) );
  assert( !mExecutablesDirectory.endsWith( QLatin1Char('/') ) );

  return directoryAtoBrelativePath(mExecutablesDirectory, mSharedLibrariesDirectory);
}

QString DestinationDirectoryStructure::executablesToRootRelativePath() const noexcept
{
  assert( !mExecutablesDirectory.isEmpty() );

  return directoryAtoBrelativePath( mExecutablesDirectory, QString() );
}

QString DestinationDirectoryStructure::directoryAtoBrelativePath(const QString & a, const QString & b, int aAdditionalLevel) noexcept
{
  assert( !a.isEmpty() );
  assert( aAdditionalLevel >= 0 );

  assert( !a.startsWith( QLatin1Char('/') ) );
  assert( !b.startsWith( QLatin1Char('/') ) );
  assert( !a.endsWith( QLatin1Char('/') ) );
  assert( !b.endsWith( QLatin1Char('/') ) );

  /*
   * a: bin -> b: lib == ../
   * a: plugins/platforms -> b: lib == ../../lib
   *
   * count of ../ to cd up from a:
   * count of / + 1 + aAdditionalLevel
   */
  const int level = a.count( QLatin1Char('/') ) + 1 + aAdditionalLevel;
  QString relativePath = QLatin1String("..");
  for(int i=1; i < level; ++i){
    relativePath += QLatin1String("/..");
  }

  if( !b.isEmpty() ){
    relativePath += QLatin1Char('/') + b;
  }

  return relativePath;
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
