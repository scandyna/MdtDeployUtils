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
#include "QtSharedLibraryFile.h"
#include "LibraryName.h"
#include <QLatin1String>

namespace Mdt{ namespace DeployUtils{

bool QtSharedLibraryFile::isQtSharedLibrary(const QFileInfo & library) noexcept
{
  const LibraryName libraryName( library.fileName() );

  return libraryName.nameWithoutDebugSuffix().startsWith( QLatin1String("Qt") );
}

QString QtSharedLibraryFile::getMajorVersionStringFromFileName(const QString & fileName) noexcept
{
  assert( fileName.length() >= 3 );
  assert( isQtSharedLibrary(fileName) );

  /*
   * libQt5Core.so
   * Qt5Core.dll
   */
  int versionStartIndex;
  if( fileName.startsWith( QLatin1String("lib") ) ){
    assert( fileName.length() >= 6 );
    versionStartIndex = 5;
  }else{
    versionStartIndex = 2;
  }

  assert( fileName.at(versionStartIndex).isNumber() );

  int versionCharCount = 1;
  if( ( fileName.length() > (versionStartIndex + 2) )
      && ( fileName.at(versionStartIndex + 1).isNumber() ) ){
    versionCharCount = 2;
  }

  return fileName.mid(versionStartIndex, versionCharCount);
}

}} // namespace Mdt{ namespace DeployUtils{
