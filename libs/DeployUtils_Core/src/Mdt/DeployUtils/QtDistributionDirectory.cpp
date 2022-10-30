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
#include "QtDistributionDirectory.h"
#include "QtSharedLibraryFile.h"
#include "QtConfReader.h"
#include "Algorithm.h"
#include <QStringBuilder>
#include <QLatin1Char>
#include <QLatin1String>
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

bool QtDistributionDirectory::isValidExisting() const noexcept
{
  if( isNull() ){
    return false;
  }

  if( !QDir( rootAbsolutePath() ).exists() ){
    return false;
  }
  if( !QDir( sharedLibrariesDirectoryAbsolutePath() ).exists() ){
    return false;
  }
  if( !QDir( pluginsRootAbsolutePath() ).exists() ){
    return false;
  }

  return true;
}

void QtDistributionDirectory::clear() noexcept
{
  mRootAbsolutePath.clear();
  mSharedLibrariesDirectoryRelativePath.clear();
  setPluginsRootRelativePath( QLatin1String("plugins") );
}

void QtDistributionDirectory::setupFromQtSharedLibrary(const QFileInfo & qtLibraryPath, OperatingSystem os)
{
  assert( isNull() );
  assert( !qtLibraryPath.filePath().isEmpty() );
  assert( qtLibraryPath.isAbsolute() );
  assert( QtSharedLibraryFile::isQtSharedLibrary(qtLibraryPath) );
  assert(os != OperatingSystem::Unknown);

  const QString qtConfFilePath = findQtConfFileFromQtSharedLibrary(qtLibraryPath);
  if( !qtConfFilePath.isEmpty() ){
    QtConfReader reader;
    const QtConf qtConf = reader.readFile(qtConfFilePath);
    setEntriesFromQtConf(qtConf, os, qtConfFilePath);
  }

  // Setup what is not in qt.conf by guessing
  if( !hasRootPath() ){
    setRootAbsolutePath( guessRootAbsolutePathFromQtSharedLibrary(qtLibraryPath) );
  }
  if( !hasSharedLibrariesDirectory() ){
    setSharedLibrariesDirectoryRelativePath( guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary(qtLibraryPath) );
  }
}

void QtDistributionDirectory::setEntriesFromQtConf(const QtConf & qtConf, OperatingSystem os, const QFileInfo & qtConfFilePath) noexcept
{
  assert(os != OperatingSystem::Unknown);
  assert( !qtConfFilePath.filePath().isEmpty() );
  assert( qtConfFilePath.isAbsolute() );

  if( qtConf.containsPrefixPath() ){
    if( qtConf.prefixPathIsAbsolute() ){
      setRootAbsolutePath( qtConf.prefixPath() );
    }else{
      const QDir qtConfDir = qtConfFilePath.absoluteDir();
      const QString rootPath = QDir::cleanPath( qtConfDir.path() % QLatin1Char('/') % qtConf.prefixPath() );
      setRootAbsolutePath(rootPath);
    }
  }

  if( (os == OperatingSystem::Linux) && qtConf.containsLibrariesPath() ){
    setSharedLibrariesDirectoryRelativePath( qtConf.librariesPath() );
  }

  if( qtConf.containsPluginsPath() ){
    setPluginsRootRelativePath( qtConf.pluginsPath() );
  }
}

void QtDistributionDirectory::setRootAbsolutePath(const QString & path) noexcept
{
  assert( !path.trimmed().isEmpty() );
  assert( QDir::isAbsolutePath(path) );

  mRootAbsolutePath = path;
}

QString QtDistributionDirectory::guessRootAbsolutePathFromQtSharedLibrary(const QFileInfo & qtLibraryPath) noexcept
{
  assert( !qtLibraryPath.filePath().isEmpty() );
  assert( qtLibraryPath.isAbsolute() );
  assert( QtSharedLibraryFile::isQtSharedLibrary(qtLibraryPath) );

  if( qtLibraryPath.absoluteFilePath().startsWith( QLatin1String("/usr") ) ){
    return QLatin1String("/usr");
  }

  return QDir::cleanPath( qtLibraryPath.absoluteDir().path() % QLatin1String("/..") );
}

void QtDistributionDirectory::setSharedLibrariesDirectoryRelativePath(const QString & path) noexcept
{
  assert( !path.trimmed().isEmpty() );
  assert( QDir::isRelativePath(path) );

  mSharedLibrariesDirectoryRelativePath = path;
}

QString QtDistributionDirectory::guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary(const QFileInfo & qtLibraryPath) noexcept
{
  assert( !qtLibraryPath.filePath().isEmpty() );
  assert( qtLibraryPath.isAbsolute() );
  assert( QtSharedLibraryFile::isQtSharedLibrary(qtLibraryPath) );

  const QDir qtLibraryDir = qtLibraryPath.absoluteDir();

  /*
   * Linux system wide, could be:
   * - /usr/lib        -> lib
   * - /usr/lib/arch   -> lib/arch
   */
  if( qtLibraryDir.path().startsWith( QLatin1String("/usr") ) ){
    return relativePathToBase( qtLibraryDir.path(), QLatin1String("/usr") );
  }

  return qtLibraryDir.dirName();
}

QString QtDistributionDirectory::sharedLibrariesDirectoryAbsolutePath() const noexcept
{
  assert( hasRootPath() );

  return QDir::cleanPath(mRootAbsolutePath % QLatin1Char('/') % mSharedLibrariesDirectoryRelativePath);
}

bool QtDistributionDirectory::containsSharedLibrary(const QFileInfo & sharedLibraryPath) const noexcept
{
  assert( !isNull() );
  assert( !sharedLibraryPath.filePath().isEmpty() );
  assert( sharedLibraryPath.isAbsolute() );

  if( QDir::cleanPath( sharedLibraryPath.absoluteDir().path() ) != sharedLibrariesDirectoryAbsolutePath() ){
    return false;
  }

  return sharedLibraryPath.exists();
}

void QtDistributionDirectory::setPluginsRootRelativePath(const QString & path) noexcept
{
  assert( !path.trimmed().isEmpty() );
  assert( QDir::isRelativePath(path) );

  mPluginsRootRelativePath = path;
}

QString QtDistributionDirectory::pluginsRootAbsolutePath() const noexcept
{
  assert( hasRootPath() );

  return QDir::cleanPath(mRootAbsolutePath % QLatin1Char('/') % mPluginsRootRelativePath);
}

QString QtDistributionDirectory::findQtConfFileFromQtSharedLibrary(const QFileInfo & qtLibraryPath) noexcept
{
  assert( !qtLibraryPath.filePath().isEmpty() );
  assert( qtLibraryPath.isAbsolute() );
  assert( QtSharedLibraryFile::isQtSharedLibrary(qtLibraryPath) );

  const QString qtConfFileName = QLatin1String("qt.conf");
  QString qtConfFilePath;

  const QDir dir = qtLibraryPath.absoluteDir();

  // Maybe are we in bin
  if( dir.exists(qtConfFileName) ){
    return QDir::cleanPath(dir.path() % QLatin1Char('/') % qtConfFileName);
  }
  // Maybe are we in lib
  qtConfFilePath = QDir::cleanPath( dir.path() % QLatin1String("/../bin/") % qtConfFileName );
  if( dir.exists(qtConfFilePath) ){
    return qtConfFilePath;
  }
  // Maybe are we on a Debian multi-arch
  const QString version = QtSharedLibraryFile::getMajorVersionStringFromFileName( qtLibraryPath.fileName() );
  assert( !version.isEmpty() );
  qtConfFilePath = QDir::cleanPath( dir.path() % QLatin1String("/qt") % version % QLatin1Char('/') % qtConfFileName );
  if( dir.exists(qtConfFilePath) ){
    return qtConfFilePath;
  }

  return QString();
}

}} // namespace Mdt{ namespace DeployUtils{
