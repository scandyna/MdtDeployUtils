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
#include "Algorithm.h"
#include <QLatin1Char>

namespace Mdt{ namespace DeployUtils{

QString relativePathToBase(const QString & path, const QString & base) noexcept
{
  assert( !path.trimmed().isEmpty() );
  assert( !base.trimmed().isEmpty() );
  assert( path.startsWith(base) );

  using sizeType = QString::size_type;

  /*
   * path:   /rootDir/lib
   * base:   /rootDir
   * result: lib
   *
   * path:   /usr/lib
   * base:   /usr
   * result: lib
   *
   * path:   /usr/lib
   * base:   /usr/
   * result: lib
   *
   * path:   /usr/lib/
   * base:   /usr
   * result: lib/
   *
   * path:   /usr/lib/
   * base:   /usr/
   * result: lib/
   */
  sizeType toRemove = path.length() - base.length();
  if( !base.endsWith( QLatin1Char('/') ) ){
    --toRemove;
  }

  return path.right(toRemove);
}

}} // namespace Mdt{ namespace DeployUtils{
