/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#include "BinaryDependenciesFile.h"
#include "SearchPathList.h"
#include "SharedLibraryFinderLinux.h"
#include "SharedLibraryFinderWindows.h"
#include "LibraryName.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies.h"
#include "Mdt/DeployUtils/Platform.h"
#include "ExecutableFileReader.h"
#include <QDir>
#include <cassert>

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
  assert( binaryFilePath.isAbsolute() );

  ExecutableFileInfoList dependencies;

  auto target = BinaryDependenciesFile::fromQFileInfo(binaryFilePath);
//   ExecutableFileInfo target;
//   target.fileName = binaryFilePath.fileName();
//   target.directoryPath = binaryFilePath.absoluteDir().path();

  ExecutableFileReader reader;
  reader.openFile(binaryFilePath);
  const Platform platform = reader.getFilePlatform();
  reader.close();

  if( platform.operatingSystem() == OperatingSystem::Unknown ){
    const QString message = tr("'%1' targets a operating system that is not supported")
                            .arg( binaryFilePath.fileName() );
    throw FindDependencyError(message);
  }

  PathList searchPathList;
  if( platform.operatingSystem() == OperatingSystem::Linux ){
    searchPathList = SharedLibraryFinderLinux::buildSearchPathList( searchFirstPathPrefixList, platform.processorISA() );
  }else if( platform.operatingSystem() == OperatingSystem::Windows ){
//     searchPathList = buildSearchPathListWindows( binaryFilePath, searchFirstPathPrefixList, platform.processorISA() );
    searchPathList = SharedLibraryFinderWindows::buildSearchPathList(binaryFilePath, searchFirstPathPrefixList, platform.processorISA(), mCompilerFinder);
  }

  emitSearchPathListMessage(searchPathList);

  Impl::IsExistingSharedLibraryFunc isExistingSharedLibrary(reader, platform);
  Impl::FindDependenciesImpl impl;
  connect(&impl, &Impl::FindDependenciesImpl::verboseMessage, this, &BinaryDependencies::verboseMessage);
  impl.findDependencies(target, dependencies, searchPathList, reader, platform, isExistingSharedLibrary);

  return Impl::qStringListFromExecutableFileInfoList(dependencies);
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
