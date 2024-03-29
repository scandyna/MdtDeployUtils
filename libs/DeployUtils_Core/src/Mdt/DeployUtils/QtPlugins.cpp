/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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
#include <Mdt/ExecutableFile/ExecutableFileReader.h>
#include <QStringBuilder>
#include <QLatin1Char>
#include <cassert>

using Mdt::ExecutableFile::ExecutableFileReader;


namespace Mdt{ namespace DeployUtils{

QtPlugins::QtPlugins(std::shared_ptr<SharedLibrariesDeployer> & shLibDeployer, QObject *parent) noexcept
 : QObject(parent),
   mShLibDeployer(shLibDeployer)
{
  assert( shLibDeployer.get() != nullptr );
}

void QtPlugins::installQtPlugins(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior)
{
  emit statusMessage(
    tr("installing Qt plugins")
  );

  const QStringList pluginsDirectories = getQtPluginsDirectoryNames(plugins);

  makeDestinationDirectoryStructure(pluginsDirectories, destination);
  const CopiedSharedLibraryFileList copiedPlugins = copyPluginsToDestination(plugins, destination, overwriteBehavior);

  if( mShLibDeployer->currentPlatform().supportsRPath() ){
   setRPathToCopiedPlugins(copiedPlugins, destination);
  }
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

CopiedSharedLibraryFileList
QtPlugins::copyPluginsToDestination(const QtPluginFileList & plugins,
                                    const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior)
{
  assert( mShLibDeployer.get() != nullptr );

  CopiedSharedLibraryFileList copiedPlugins;

  if( plugins.empty() ){
    return copiedPlugins;
  }

  emit verboseMessage(
    tr("copy qt plugins")
  );

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(overwriteBehavior);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &QtPlugins::verboseMessage);

  ExecutableFileReader reader;

  for(const auto & plugin : plugins){
    const QString destinationDirectoryPath = QDir::cleanPath( destination.qtPluginsRootDirectoryPath() % QLatin1Char('/') % plugin.directoryName() );
    CopiedSharedLibraryFile copiedPlugin;
    copiedPlugin.file = fileCopier.copyFile(plugin.absoluteFilePath(), destinationDirectoryPath);
    if( copiedPlugin.file.hasBeenCopied() ){
      reader.openFile( plugin.absoluteFilePath(), mShLibDeployer->currentPlatform() );
      copiedPlugin.rpath = reader.getRunPath();
      reader.close();
      copiedPlugins.push_back(copiedPlugin);
    }
  }

  return copiedPlugins;
}

void QtPlugins::setRPathToCopiedPlugins(const CopiedSharedLibraryFileList & copiedPlugins,
                                        const DestinationDirectory & destination)
{
  assert( mShLibDeployer.get() != nullptr );
  assert( mShLibDeployer->currentPlatform().supportsRPath() );

  RPath rpath;
  rpath.appendPath( destination.structure().qtPluginsToSharedLibrariesRelativePath() );

  emit statusMessage(
    tr("update Rpath for copied Qt plugins if required")
  );

  mShLibDeployer->setRPathToCopiedSharedLibraries(copiedPlugins, rpath);
}

}} // namespace Mdt{ namespace DeployUtils{
