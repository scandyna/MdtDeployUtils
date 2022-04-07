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
#include "QtConfWriter.h"
#include <QSettings>
#include <QDir>
#include <QLatin1String>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

QtConfWriter::QtConfWriter(QObject *parent) noexcept
 : QObject(parent)
{
}

void QtConfWriter::writeConfToDirectory(const QtConf & conf, const QString & directoryPath)
{
  assert( !directoryPath.trimmed().isEmpty() );

  const QString confFilePath = QDir::cleanPath( directoryPath + QLatin1String("/qt.conf") );
  emit verboseMessage(
    tr("writing %1")
    .arg(confFilePath)
  );

  QSettings settings(confFilePath, QSettings::IniFormat);

  if( conf.containsPrefixPath() ){
    settings.setValue( QLatin1String("Paths/Prefix"), conf.prefixPath() );
  }
  if( conf.containsPluginsPath() ){
    settings.setValue( QLatin1String("Paths/Plugins"), conf.pluginsPath() );
  }

  settings.sync();
  if(settings.status() != QSettings::NoError){
    const QString msg = tr("writing %1 failed").arg(confFilePath);
    throw WriteQtConfError(msg);
  }
}

}} // namespace Mdt{ namespace DeployUtils{
