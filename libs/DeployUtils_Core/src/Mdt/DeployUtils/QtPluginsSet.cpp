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
#include "QtPluginsSet.h"
#include "LibraryName.h"
#include <QByteArray>
#include <QDataStream>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

void QtPluginsSet::set(const QString & directoryName, const QStringList & pluginBaseNames) noexcept
{
  assert( !directoryName.trimmed().isEmpty() );
  assert( !pluginBaseNames.isEmpty() );

  mMap.insert(directoryName, pluginBaseNames);
}

bool QtPluginsSet::shouldDeployPlugin(const QtPluginFile & plugin) const noexcept
{
  const QString directoryName = plugin.directoryName();

  if( !mMap.contains(directoryName) ){
    return true;
  }

  const QString qPluginBaseName = LibraryName( plugin.fileInfo().fileName() ).nameWithoutDebugSuffix();
  assert( qPluginBaseName.size() > 1 );
  const QString pluginBaseName = qPluginBaseName.right(qPluginBaseName.size() - 1);

  return contains(directoryName, pluginBaseName);
}

// QString QtPluginsSet::toDebugString() const noexcept
// {
//   QByteArray ba;
//   QDataStream s(&ba, QIODevice::WriteOnly);
//
//   s << mMap;
//
//   return QString::fromLatin1(ba);
// }

}} // namespace Mdt{ namespace DeployUtils{
