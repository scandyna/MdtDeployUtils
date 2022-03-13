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
#include "Mdt/DeployUtils/Impl/IsExistingSharedLibrary.h"
#include "Mdt/DeployUtils/Platform.h"
#include "ExecutableFileReader.h"
#include <QDir>
#include <memory>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

BinaryDependencies::BinaryDependencies(QObject* parent)
 : QObject(parent),
   mImpl( std::make_unique<Impl::FindDependenciesImpl>() )
{
  connect(mImpl.get(), &Impl::FindDependenciesImpl::verboseMessage, this, &BinaryDependencies::verboseMessage);
  connect(mImpl.get(), &Impl::FindDependenciesImpl::debugMessage, this, &BinaryDependencies::debugMessage);
}

BinaryDependencies::~BinaryDependencies() noexcept
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
  assert( !binaryFilePath.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
  assert( binaryFilePath.isAbsolute() );

  return findDependencies(QFileInfoList{binaryFilePath}, searchFirstPathPrefixList);
}

QStringList BinaryDependencies::findDependencies(const QFileInfoList & binaryFilePathList, const PathList & searchFirstPathPrefixList)
{
  assert( !binaryFilePathList.isEmpty() );

  BinaryDependenciesFileList dependencies;

  const QFileInfo & firstBinaryFilePath = binaryFilePathList.at(0);

  ExecutableFileReader reader;
  reader.openFile(firstBinaryFilePath);
  const Platform platform = reader.getFilePlatform();
  reader.close();

  if( platform.operatingSystem() == OperatingSystem::Unknown ){
    const QString message = tr("'%1' targets a operating system that is not supported")
                            .arg( firstBinaryFilePath.fileName() );
    throw FindDependencyError(message);
  }

  Impl::IsExistingSharedLibrary isExistingShLibOp(reader, platform);
  std::shared_ptr<AbstractSharedLibraryFinder> shLibFinder;

  if( platform.operatingSystem() == OperatingSystem::Linux ){

    shLibFinder = std::make_shared<SharedLibraryFinderLinux>(isExistingShLibOp);
    SharedLibraryFinderLinux *shLibFinderLinux = static_cast<SharedLibraryFinderLinux*>( shLibFinder.get() );
    shLibFinderLinux->buildSearchPathList( searchFirstPathPrefixList, platform.processorISA() );

  }else if( platform.operatingSystem() == OperatingSystem::Windows ){

    shLibFinder = std::make_shared<SharedLibraryFinderWindows>(isExistingShLibOp);
    SharedLibraryFinderWindows *shLibFinderWindows = static_cast<SharedLibraryFinderWindows*>( shLibFinder.get() );
    shLibFinderWindows->buildSearchPathList(firstBinaryFilePath, searchFirstPathPrefixList, platform.processorISA(), mCompilerFinder);

  }

  emitSearchPathListMessage( shLibFinder->searchPathList() );

  mImpl->setSharedLibrariesFinder(shLibFinder);

  for(const QFileInfo & binaryFilePath : binaryFilePathList){
    assert( !binaryFilePath.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
    assert( binaryFilePath.isAbsolute() );
    auto target = BinaryDependenciesFile::fromQFileInfo(binaryFilePath);
    mImpl->findDependencies(target, dependencies, reader, platform);
  }

  return Impl::qStringListFromBinaryDependenciesFileList(dependencies);
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
