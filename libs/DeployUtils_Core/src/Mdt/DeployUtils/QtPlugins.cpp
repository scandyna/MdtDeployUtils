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
#include "QtPlugins.h"
#include "FileCopier.h"
#include "SharedLibrariesDeployer.h"
#include <QStringBuilder>
#include <QLatin1Char>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

QtPlugins::QtPlugins(std::shared_ptr<SharedLibrariesDeployer> & shLibDeployer, QObject *parent) noexcept
 : QObject(parent),
   mShLibDeployer(shLibDeployer)
{
  assert( shLibDeployer.get() != nullptr );
}


void QtPlugins::setAlreadyDeployedSharedLibraries(const QStringList & libraries) noexcept
{
}

void QtPlugins::setSearchPrefixPathList(const PathList & pathList) noexcept
{
}

void QtPlugins::deployQtPlugins(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior)
{
  const QStringList pluginsDirectories = getQtPluginsDirectoryNames(plugins);

  makeDestinationDirectoryStructure(pluginsDirectories, destination);
  copyPluginsToDestination(plugins, destination, overwriteBehavior);
  copySharedLibrariesPluginsDependsOn(plugins, destination, overwriteBehavior);

  /// \todo RPath for plugins and dependencies
}

bool QtPlugins::pathIsAbsoluteAndCouldBePluginsRoot(const QFileInfo & qtPluginsRoot) noexcept
{
  if( qtPluginsRoot.filePath().isEmpty() ){
    return false;
  }
  if( !qtPluginsRoot.isAbsolute() ){
    return false;
  }
  if( !qtPluginsRoot.isDir() ){
    return false;
  }
  if( qtPluginsRoot.fileName() != QLatin1String("plugins") ){
    return false;
  }

  return true;
}

void QtPlugins::makeDestinationDirectoryStructure(const QStringList & qtPluginsDirectories, const DestinationDirectory & destination)
{
  for(const QString & directory : qtPluginsDirectories){
    const QString path = QDir::cleanPath(destination.qtPluginsRootDirectoryPath() % QLatin1Char('/') % directory);
    emit verboseMessage(
      tr("creating directory %1")
      .arg(path)
    );
    FileCopier::createDirectory(path);
  }
}

void QtPlugins::copyPluginsToDestination(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior)
{
  if( plugins.empty() ){
    return;
  }

  emit verboseMessage(
    tr("copy qt plugins")
  );

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(overwriteBehavior);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &QtPlugins::verboseMessage);

  for(const auto & plugin : plugins){
    const QString destinationDirectoryPath = QDir::cleanPath( destination.qtPluginsRootDirectoryPath() % QLatin1Char('/') % plugin.directoryName() );
    fileCopier.copyFile(plugin.absoluteFilePath(), destinationDirectoryPath);
  }
}

void QtPlugins::copySharedLibrariesPluginsDependsOn(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior)
{
  assert( mShLibDeployer.get() != nullptr );

  const auto toFileInfo = [](const QtPluginFile & plugin){
    return plugin.fileInfo();
  };

  mShLibDeployer->copySharedLibrariesTargetsDependsOn( plugins, toFileInfo, destination.sharedLibrariesDirectoryPath() );
}

}} // namespace Mdt{ namespace DeployUtils{
