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
#include "QtModulePlugins.h"
#include "QtPlugins.h"
#include "QtModuleList.h"
#include "FileCopier.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QDir>
#include <QFileInfoList>
#include <cassert>
#include <algorithm>
#include <iterator>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

QtPluginFileList QtModulePlugins::getQtPluginsQtLibrariesDependsOn(const QtSharedLibraryFileList & qtLibraries,
                                                                   const QtPluginsSet & pluginsSet) const noexcept
{
  QtPluginFileList plugins;

  if( qtLibraries.empty() ){
    return plugins;
  }

  emit statusMessage(
    tr("search required qt plugins")
  );

  const auto qtModules = QtModuleList::fromQtSharedLibraries(qtLibraries);

  const QString qtPluginsRoot = findPluginsRootFromQtLibrary(qtLibraries[0]);
  emit verboseMessage(
    tr("will take qt plugins from %1")
    .arg(qtPluginsRoot)
  );

  plugins = getPluginsForModules(qtModules, qtPluginsRoot, pluginsSet);

  return plugins;
}

template<typename Predicate>
QStringList qtModulePluginsGetPluginsDirectoriesForModuleImpl(const Predicate & p) noexcept
{
  QStringList directories;

  struct ModulePluginsDirectoryPair
  {
    QtModule module;
    const char *pluginsDirectory;
  };

  static
  const ModulePluginsDirectoryPair modulePluginsDirectoryMap[] = {
    {QtModule::Gui, "accessible"}, // Qt4
    {QtModule::Gui, "iconengines"},
    {QtModule::Gui, "imageformats"},
    {QtModule::Gui, "platforms"},
    {QtModule::Gui, "platforminputcontexts"},
    {QtModule::VirtualKeyboard, "platforminputcontexts"},
    {QtModule::VirtualKeyboard, "virtualkeyboard"}
  };

  const auto addDirectoryIfMatches = [&directories, &p](const ModulePluginsDirectoryPair & pair){
    if( p( pair.module, QLatin1String(pair.pluginsDirectory) ) ){
      directories.append( QLatin1String(pair.pluginsDirectory) );
    }
  };

  std::for_each(std::cbegin(modulePluginsDirectoryMap), std::cend(modulePluginsDirectoryMap), addDirectoryIfMatches);

  return directories;
}

QStringList QtModulePlugins::getPluginsDirectoriesForModule(QtModule module) noexcept
{
  const auto pred = [module](QtModule candidateModule, const QString &){
    return candidateModule == module;
  };

  return qtModulePluginsGetPluginsDirectoriesForModuleImpl(pred);
}

QStringList QtModulePlugins::getExistingPluginsDirectoriesForModule(QtModule module, const QFileInfo & qtPluginsRoot) noexcept
{
  assert( pathIsAbsoluteAndCouldBePluginsRoot(qtPluginsRoot) );

  QDir qtPluginsRootDir = qtPluginsRoot.absoluteFilePath();
  assert( qtPluginsRootDir.isAbsolute() );
  assert( qtPluginsRootDir.exists() );
  assert( qtPluginsRootDir.dirName() == QLatin1String("plugins") );

  const auto pred = [module, &qtPluginsRootDir](QtModule candidateModule, const QString & directory){
    if(candidateModule != module){
      return false;
    }
    if( !qtPluginsRootDir.cd(directory) ){
      return false;
    }
    qtPluginsRootDir.cdUp();

    return true;
  };

  return qtModulePluginsGetPluginsDirectoriesForModuleImpl(pred);
}


void qtModulePluginsAddDirectoryToList(const QStringList & directories, QStringList & list) noexcept
{
  for(const QString & directory : directories){
    if( !list.contains(directory) ){
      list.append(directory);
    }
  }
}

QStringList QtModulePlugins::getPluginsDirectoriesForModules(const QtModuleList & modules) noexcept
{
  QStringList directories;

  for(QtModule module : modules){
    qtModulePluginsAddDirectoryToList(getPluginsDirectoriesForModule(module), directories);
  }

  return directories;
}

QStringList QtModulePlugins::getExistingPluginsDirectoriesForModules(const QtModuleList & modules, const QFileInfo & qtPluginsRoot) noexcept
{
  assert( pathIsAbsoluteAndCouldBePluginsRoot(qtPluginsRoot) );

  QStringList directories;

  for(QtModule module : modules){
    qtModulePluginsAddDirectoryToList(getExistingPluginsDirectoriesForModule(module, qtPluginsRoot), directories);
  }

  return directories;
}

QtPluginFileList QtModulePlugins::getPluginsForModules(const QtModuleList & modules, const QFileInfo & qtPluginsRoot,
                                                       const QtPluginsSet & pluginsSet) const noexcept
{
  assert( pathIsAbsoluteAndCouldBePluginsRoot(qtPluginsRoot) );

  QtPluginFileList plugins;

//   emit debugMessage(
//     tr("Qt plugins set: %1")
//     .arg( pluginsSet.toDebugString() )
//   );

  const QStringList pluginsDirectories = getExistingPluginsDirectoriesForModules(modules, qtPluginsRoot);
  for(const QString & pluginsDirectory : pluginsDirectories){
    emit verboseMessage(
      tr("looking in %1")
      .arg(pluginsDirectory)
    );
    const QDir dir( QDir::cleanPath( qtPluginsRoot.absoluteFilePath() % QLatin1Char('/') % pluginsDirectory ) );
    const auto files = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    for(const auto & file : files){
      if( QtPluginFile::fileCouldBePlugin(file) ){
        const auto qtPluginFile = QtPluginFile::fromQFileInfo(file);
        if( pluginsSet.shouldDeployPlugin(qtPluginFile) ){
          plugins.emplace_back(qtPluginFile);
        }else{
          emit debugMessage(
            tr("%1 will not be deployed (a Qt plugins set have been provided)")
            .arg( qtPluginFile.fileInfo().fileName() )
          );
        }
      }
    }
  }

  return plugins;
}

bool QtModulePlugins::pathIsAbsoluteAndCouldBePluginsRoot(const QFileInfo & qtPluginsRoot) noexcept
{
  return QtPlugins::pathIsAbsoluteAndCouldBePluginsRoot(qtPluginsRoot);
}

QString QtModulePlugins::findPluginsRootFromQtLibraryPath(const QFileInfo & qtLibraryPath)
{
  assert( !qtLibraryPath.filePath().isEmpty() );
  assert( qtLibraryPath.isAbsolute() );

  QString qtPluginsRoot;
  QDir dir = qtLibraryPath.dir();

  if( dir.absolutePath().startsWith( QLatin1String("/usr/lib/") ) ){
    const QString qtDir = QLatin1String("qt") + QtSharedLibraryFile::getMajorVersionStringFromFileName( qtLibraryPath.fileName() );
    qtPluginsRoot = QDir::cleanPath( dir.absolutePath() % QLatin1Char('/') % qtDir % QLatin1String("/plugins") );
  }else{
    qtPluginsRoot = QDir::cleanPath( dir.absolutePath() % QLatin1String("/../plugins") );
  }

  if( !QDir(qtPluginsRoot).exists() ){
    const QString msg = tr("could not find Qt plugins dir '%1'")
                        .arg(qtPluginsRoot);
    throw FindQtPluginError(msg);
  }

  return qtPluginsRoot;
}

QString QtModulePlugins::findPluginsRootFromQtLibrary(const QtSharedLibraryFile & qtLibrary)
{
  return findPluginsRootFromQtLibraryPath( qtLibrary.fileInfo() );
}

}} // namespace Mdt{ namespace DeployUtils{
