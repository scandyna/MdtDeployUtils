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
#include "QtSharedLibrary.h"
#include <QLatin1String>
#include <QString>
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

QtSharedLibraryFileList QtSharedLibrary::getQtSharedLibraries(const QStringList & sharedLibraries) noexcept
{
  QtSharedLibraryFileList qtLibraries;

  for(const QString & library : sharedLibraries){
    const QFileInfo libraryFi(library);
    if( QtSharedLibraryFile::isQtSharedLibrary(libraryFi) ){
      qtLibraries.push_back( QtSharedLibraryFile::fromQFileInfo(libraryFi) );
    }
  }

  return qtLibraries;
}

bool QtSharedLibrary::sharedLibraryIsInQtDistribution(const QFileInfo & sharedLibrary) noexcept
{
  assert( !sharedLibrary.filePath().isEmpty() );
  assert( sharedLibrary.isAbsolute() );

  if( sharedLibrary.absoluteFilePath().startsWith( QLatin1String("/usr/lib") ) ){
    return true;
  }

  QDir directory = sharedLibrary.dir();
  const QString directoryName = directory.dirName();

  if( ( directoryName != QLatin1String("lib") ) && ( directoryName != QLatin1String("bin") ) ){
    return false;
  }

  if( !directory.cdUp() ){
    return false;
  }

  if( !directory.exists( QLatin1String("plugins") ) ){
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
