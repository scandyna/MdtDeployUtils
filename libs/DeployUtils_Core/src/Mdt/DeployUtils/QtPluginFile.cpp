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
#include "QtPluginFile.h"
#include "ExecutableFileReader.h"
#include <QLatin1String>

namespace Mdt{ namespace DeployUtils{

bool QtPluginFile::pathCouldBePluginsDirectory(const QFileInfo & path) noexcept
{
  if( !path.isDir() ){
    return false;
  }
  /*
   * QFileInfo::dir() returns the parent directory
   */
  if( path.dir().dirName() != QLatin1String("plugins") ){
    return false;
  }

  return true;
}

bool QtPluginFile::fileCouldBePlugin(const QFileInfo & fileInfo) noexcept
{
  assert( !fileInfo.filePath().isEmpty() );
  assert( fileInfo.isAbsolute() );

  if( !pathCouldBePluginsDirectory( QFileInfo( fileInfo.dir().path() ) ) ){
    return false;
  }

  ExecutableFileReader reader;
  try{
    reader.openFile(fileInfo);
  }catch(...){
    return false;
  }

  /// \todo once implemented, should use isSharedLibrary()
  return reader.isExecutableOrSharedLibrary();
}

QStringList getQtPluginsDirectoryNames(const QtPluginFileList & qtPlugins) noexcept
{
  QStringList directoryNames;

  for(const auto & qtPlugin : qtPlugins){
    if( !directoryNames.contains( qtPlugin.directoryName() ) ){
      directoryNames.append( qtPlugin.directoryName() );
    }
  }

  return directoryNames;
}

QStringList getQtPluginsFilePathList(const QtPluginFileList & qtPlugins) noexcept
{
  QStringList pathList;

  for(const auto & plugin : qtPlugins){
    pathList.append( plugin.absoluteFilePath() );
  }

  return pathList;
}

}} // namespace Mdt{ namespace DeployUtils{
