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
#include "QtConf.h"
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

void QtConf::setPrefixPath(const QString & path) noexcept
{
  assert( !path.trimmed().isEmpty() );

  mPrefixPath = path;
}

bool QtConf::prefixPathIsAbsolute() const noexcept
{
  return QDir::isAbsolutePath(mPrefixPath);
}

void QtConf::setPluginsPath(const QString & path) noexcept
{
  assert( !path.trimmed().isEmpty() );

  mPluginsPath = path;
}

}} // namespace Mdt{ namespace DeployUtils{
