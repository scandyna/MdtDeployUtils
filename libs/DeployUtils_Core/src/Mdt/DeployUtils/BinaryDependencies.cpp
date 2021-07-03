/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#include "BinaryDependencies.h"
#include "SearchPathList.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies.h"
#include "Mdt/DeployUtils/Platform.h"
#include "ExecutableFileReader.h"
#include <QDir>
#include <cassert>

// #include <QDebug>
// #include <iostream>

namespace Mdt{ namespace DeployUtils{

BinaryDependencies::BinaryDependencies(QObject* parent)
 : QObject(parent)
{
}

QStringList BinaryDependencies::findDependencies(const QFileInfo & binaryFilePath, const PathList & searchFirstPathPrefixList)
{
  using Impl::ExecutableFileInfo;
  using Impl::ExecutableFileInfoList;

  assert( !binaryFilePath.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()

  ExecutableFileInfoList dependencies;

  ExecutableFileInfo target;
  target.fileName = binaryFilePath.fileName();
  target.directoryPath = binaryFilePath.absoluteDir().path();

  ExecutableFileReader reader;
  reader.openFile(binaryFilePath);
  const Platform platform = reader.getFilePlatform();
  reader.close();

  if( platform.operatingSystem() == OperatingSystem::Unknown ){
    const QString message = tr("'%1' targets a operating system that is not supported")
                            .arg( binaryFilePath.fileName() );
    throw FindDependencyError(message);
  }

  const PathList searchPathList = buildSearchPathList(binaryFilePath, searchFirstPathPrefixList, platform);

  emitSearchPathListMessage(searchPathList);

  Impl::IsExistingSharedLibraryFunc isExistingSharedLibrary(reader, platform);
  Impl::FindDependenciesImpl impl;
  connect(&impl, &Impl::FindDependenciesImpl::verboseMessage, this, &BinaryDependencies::verboseMessage);
  impl.findDependencies(target, dependencies, searchPathList, reader, platform, isExistingSharedLibrary);

  return Impl::qStringListFromExecutableFileInfoList(dependencies);
}

PathList BinaryDependencies::buildSearchPathList(const QFileInfo & binaryFilePath, const PathList & searchFirstPathPrefixList, const Platform & platform) const noexcept
{
  assert( !platform.isNull() );

  switch( platform.operatingSystem() ){
    case OperatingSystem::Linux:
      return buildSearchPathListLinux( searchFirstPathPrefixList, platform.processorISA() );
    case OperatingSystem::Windows:
      return buildSearchPathListWindows( binaryFilePath, searchFirstPathPrefixList, platform.processorISA() );
    case OperatingSystem::Unknown:
      break;
  }

  return PathList();
}

PathList BinaryDependencies::buildSearchPathListLinux(const PathList & searchFirstPathPrefixList, ProcessorISA processorISA) const noexcept
{
  PathList searchPathList;

  SearchPathList searchFirstPathList;
  searchFirstPathList.setIncludePathPrefixes(true);
  searchFirstPathList.setPathSuffixList({QLatin1String("lib"),QLatin1String("qt5/lib")});
  searchFirstPathList.setPathPrefixList(searchFirstPathPrefixList);

  searchPathList.appendPathList( searchFirstPathList.pathList() );
  searchPathList.appendPathList( PathList::getSystemLibraryKnownPathListLinux(processorISA) );

  return searchPathList;
}

PathList BinaryDependencies::buildSearchPathListWindows(const QFileInfo & binaryFilePath,
                                                        const PathList & searchFirstPathPrefixList, ProcessorISA processorISA) const noexcept
{
  PathList searchPathList;

  SearchPathList searchFirstPathList;
  searchFirstPathList.setIncludePathPrefixes(true);
  searchFirstPathList.setPathSuffixList({QLatin1String("bin"),QLatin1String("qt5/bin")});
  searchFirstPathList.appendPath( binaryFilePath.absoluteDir().path() );
  searchFirstPathList.setPathPrefixList(searchFirstPathPrefixList);

  searchPathList.appendPathList( searchFirstPathList.pathList() );
  searchPathList.appendPathList( PathList::getSystemLibraryKnownPathListWindows(processorISA) );

  return searchPathList;
}

void BinaryDependencies::emitSearchPathListMessage(const PathList & pathList) const
{
  const QString startMessage = tr("search path list:");
  emit verboseMessage(startMessage);

  for(const QString & path : pathList){
    const QString msg = tr(" %1").arg(path);
    emit verboseMessage(msg);
  }
}

}} // namespace Mdt{ namespace DeployUtils{
