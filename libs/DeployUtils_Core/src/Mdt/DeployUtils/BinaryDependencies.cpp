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
#include "LibraryName.h"
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

void BinaryDependencies::setCompilerFinder(const std::shared_ptr<CompilerFinder> & compilerFinder) noexcept
{
  assert( compilerFinder.get() != nullptr );
  assert( compilerFinder->hasInstallDir() );

  mCompilerFinder = compilerFinder;
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
  bool isWindowsDebugBuild = false;

  ExecutableFileReader reader;
  reader.openFile(binaryFilePath);
  const Platform platform = reader.getFilePlatform();
  if( platform.operatingSystem() == OperatingSystem::Windows ){
    const bool targetContainsDebugSymbols = reader.containsDebugSymbols();
    const QStringList directDependentDllNames = reader.getNeededSharedLibraries();
    isWindowsDebugBuild = checkIfIsWindowsDebugBuild(targetContainsDebugSymbols, directDependentDllNames);
  }
  reader.close();

  if( platform.operatingSystem() == OperatingSystem::Unknown ){
    const QString message = tr("'%1' targets a operating system that is not supported")
                            .arg( binaryFilePath.fileName() );
    throw FindDependencyError(message);
  }

  PathList searchPathList;
  if( platform.operatingSystem() == OperatingSystem::Linux ){
    searchPathList = buildSearchPathListLinux( searchFirstPathPrefixList, platform.processorISA() );
  }else if( platform.operatingSystem() == OperatingSystem::Windows ){
    BuildType buildType;
    if( isWindowsDebugBuild ){
      buildType = BuildType::Debug;
    }else{
      buildType = BuildType::Release;
    }
    searchPathList = buildSearchPathListWindows(binaryFilePath, searchFirstPathPrefixList, platform.processorISA(), buildType);
    emitBuildTypeMessage(buildType);
  }

  emitSearchPathListMessage(searchPathList);

  Impl::IsExistingSharedLibraryFunc isExistingSharedLibrary(reader, platform);
  Impl::FindDependenciesImpl impl;
  connect(&impl, &Impl::FindDependenciesImpl::verboseMessage, this, &BinaryDependencies::verboseMessage);
  impl.findDependencies(target, dependencies, searchPathList, reader, platform, isExistingSharedLibrary);

  return Impl::qStringListFromExecutableFileInfoList(dependencies);
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

PathList BinaryDependencies::buildSearchPathListWindows(const QFileInfo & binaryFilePath, const PathList & searchFirstPathPrefixList,
                                                        ProcessorISA processorISA, BuildType buildType) const noexcept
{
  PathList searchPathList;

  SearchPathList searchFirstPathList;
  searchFirstPathList.setIncludePathPrefixes(true);
  searchFirstPathList.setPathSuffixList({QLatin1String("bin"),QLatin1String("qt5/bin")});
  searchFirstPathList.appendPath( binaryFilePath.absoluteDir().path() );
  searchFirstPathList.setPathPrefixList(searchFirstPathPrefixList);

  searchPathList.appendPathList( searchFirstPathList.pathList() );
  if( hasCompilerInstallDir() ){
    searchPathList.appendPath( mCompilerFinder->findRedistDirectory(processorISA, buildType) );
  }

  return searchPathList;
}

bool BinaryDependencies::checkIfIsWindowsDebugBuild(bool targetHasDebugSymbols, const QStringList & directDependentDllNames) noexcept
{
  if( !targetHasDebugSymbols ){
    return false;
  }
  for( const QString & dllName : directDependentDllNames ){
    const LibraryName libName(dllName);
    if( libName.hasNameDebugSuffix() ){
      return true;
    }
  }

  return false;
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

void BinaryDependencies::emitBuildTypeMessage(BuildType buildType) const
{
  if(buildType == BuildType::Debug){
    const QString msg = tr("build type is Debug");
    emit verboseMessage(msg);
  }else{
    const QString msg = tr("build type is Release (like)");
    emit verboseMessage(msg);
  }
}

bool BinaryDependencies::hasCompilerInstallDir() const noexcept
{
  if( mCompilerFinder.get() == nullptr ){
    return false;
  }
  return mCompilerFinder->hasInstallDir();
}

}} // namespace Mdt{ namespace DeployUtils{
