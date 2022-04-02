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
#include "RPath.h"
#include "ExecutableFileWriter.h"
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

void QtPlugins::deployQtPlugins(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior)
{
  const QStringList pluginsDirectories = getQtPluginsDirectoryNames(plugins);

  makeDestinationDirectoryStructure(pluginsDirectories, destination);
  const QStringList copiedPlugins = copyPluginsToDestination(plugins, destination, overwriteBehavior);
  copySharedLibrariesPluginsDependsOn(plugins, destination);

  if( mShLibDeployer->currentPlatform().supportsRPath() ){
   setRPathToCopiedPlugins(copiedPlugins, mShLibDeployer->currentPlatform(), destination);
  }
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

QStringList QtPlugins::copyPluginsToDestination(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior)
{
  if( plugins.empty() ){
    return QStringList();
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

  return fileCopier.copiedFilesDestinationPathList();
}

void QtPlugins::copySharedLibrariesPluginsDependsOn(const QtPluginFileList & plugins, const DestinationDirectory & destination)
{
  assert( mShLibDeployer.get() != nullptr );

  const auto toFileInfo = [](const QtPluginFile & plugin){
    return plugin.fileInfo();
  };

  mShLibDeployer->copySharedLibrariesTargetsDependsOn( plugins, toFileInfo, destination.sharedLibrariesDirectoryPath() );
}

void QtPlugins::setRPathToCopiedPlugins(const QStringList & destinationFilePathList, const Platform & platform, const DestinationDirectory & destination)
{
  assert( platform.supportsRPath() );

  if( destinationFilePathList.isEmpty() ){
    return;
  }

  RPath rpath;
  rpath.appendPath( destination.structure().qtPluginsToSharedLibrariesRelativePath() );

  emit statusMessage(
    tr("update Rpath for copied Qt plugins if required")
  );

  ExecutableFileWriter writer;
  connect(&writer, &ExecutableFileWriter::message, this, &QtPlugins::verboseMessage);
  connect(&writer, &ExecutableFileWriter::verboseMessage, this, &QtPlugins::verboseMessage);

  for(const QString & filePath : destinationFilePathList){
    writer.openFile(filePath, platform);
    if(writer.getRunPath() != rpath){
      const QString msg = tr("update rpath for %1").arg(filePath);
      emit verboseMessage(msg);
      writer.setRunPath(rpath);
    }
    writer.close();
  }
}

}} // namespace Mdt{ namespace DeployUtils{
