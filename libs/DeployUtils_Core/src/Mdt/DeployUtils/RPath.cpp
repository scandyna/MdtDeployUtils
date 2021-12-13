/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#include "RPath.h"
#include <QDir>
#include <algorithm>

namespace Mdt{ namespace DeployUtils{

RPathEntry::RPathEntry(const QString & path) noexcept
  : mPath( QDir::cleanPath( path.trimmed() ) )
{
  assert( !mPath.isEmpty() );
}

bool operator==(const RPathEntry & a, const RPathEntry & b) noexcept
{
  return a.path() == b.path();
}

bool operator==(const RPath & a, const RPath & b) noexcept
{
  if( a.entriesCount() != b.entriesCount() ){
    return false;
  }

  return std::equal( a.cbegin(), a.cend(), b.cbegin() );
}

}} // namespace Mdt{ namespace DeployUtils{
