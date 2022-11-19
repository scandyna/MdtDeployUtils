/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#include "BinaryDependenciesFile.h"
#include <QLatin1String>
#include <QDir>

namespace Mdt{ namespace DeployUtils{

QString BinaryDependenciesFile::absoluteDirectoryPath() const noexcept
{
  /*
   * See Qt doc on QDir constructors
   * A empty path represents the current app directory (".")
   * So, QFileInfo::absolutePath() will be a full path to app directory,
   * nothing related to this current file (!!)
   */
  const QDir dir = mFile.dir();
  if( dir.dirName() == QLatin1String(".") ){
    return QString();
  }

  return dir.absolutePath();
}

BinaryDependenciesFile BinaryDependenciesFile::fromLibraryName(const QFileInfo & name) noexcept
{
  assert( ( name.path().trimmed() == QLatin1String(".") )
         || name.path().trimmed().isEmpty() );

  return BinaryDependenciesFile(name);
}

}} // namespace Mdt{ namespace DeployUtils{
