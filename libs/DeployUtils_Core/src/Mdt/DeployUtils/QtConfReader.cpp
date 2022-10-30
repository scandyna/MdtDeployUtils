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
#include "QtConfReader.h"
#include <QSettings>
#include <QDir>
#include <QLatin1String>
#include <QVariant>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

QtConfReader::QtConfReader(QObject *parent) noexcept
 : QObject(parent)
{
}

QtConf QtConfReader::readFile(const QFileInfo & filePath)
{
  assert( !filePath.filePath().isEmpty() );
  assert( filePath.isAbsolute() );
  assert( filePath.isFile() );

  QtConf qtConf;

  QSettings settings(filePath.absoluteFilePath(), QSettings::IniFormat);
  if(settings.status() != QSettings::NoError){
    const QString msg = tr("reading %1 failed").arg( filePath.absoluteFilePath() );
    throw ReadQtConfError(msg);
  }

  const QVariant prefix = settings.value( QLatin1String("Paths/Prefix") );
  if( !prefix.isNull() ){
    qtConf.setPrefixPath( prefix.toString() );
  }

  const QVariant libraries = settings.value( QLatin1String("Paths/Libraries") );
  if( !libraries.isNull() ){
    qtConf.setLibrariesPath( libraries.toString() );
  }

  const QVariant plugins = settings.value( QLatin1String("Paths/Plugins") );
  if( !plugins.isNull() ){
    qtConf.setPluginsPath( plugins.toString() );
  }

  return qtConf;
}

}} // namespace Mdt{ namespace DeployUtils{
