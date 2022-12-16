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
#include "QtDistributionDirectory.h"
#include "SharedLibraryFinderLinux.h"
#include "SharedLibraryFinderWindows.h"
#include "LibraryName.h"
#include "FileInfoUtils.h"

#include "Mdt/DeployUtils/Impl/BinaryDependencies.h"

#include "IsExistingValidSharedLibrary.h"
#include "Mdt/DeployUtils/Platform.h"
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

BinaryDependenciesResult
BinaryDependencies::findDependencies(const QFileInfo & binaryFilePath,
                                     const PathList & searchFirstPathPrefixList,
                                     std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory)
{
  assert( !binaryFilePath.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
  assert( binaryFilePath.isAbsolute() );
  assert(qtDistributionDirectory.get() != nullptr);

  /// \todo should be removed
  BinaryDependenciesFileList allDependencies;

  ExecutableFileReader reader;
  std::shared_ptr<AbstractSharedLibraryFinder> shLibFinder;

  const Platform platform = setupFindDependencies(reader, shLibFinder, searchFirstPathPrefixList, qtDistributionDirectory, binaryFilePath);

  BinaryDependenciesResult result( binaryFilePath, platform.operatingSystem() );

  auto target = BinaryDependenciesFile::fromQFileInfo(binaryFilePath);

  mImpl->findDependencies(target, result, allDependencies, reader, platform);

  return result;
}

BinaryDependenciesResultList
BinaryDependencies::findDependencies(const QFileInfoList & binaryFilePathList,
                                     const PathList & searchFirstPathPrefixList,
                                     std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory)
{
  assert( !binaryFilePathList.isEmpty() );
  assert(qtDistributionDirectory.get() != nullptr);

  /// \todo should be removed
  BinaryDependenciesFileList allDependencies;

  const QFileInfo & firstBinaryFilePath = binaryFilePathList.at(0);

  ExecutableFileReader reader;
  std::shared_ptr<AbstractSharedLibraryFinder> shLibFinder;

  const Platform platform = setupFindDependencies(reader, shLibFinder, searchFirstPathPrefixList, qtDistributionDirectory, firstBinaryFilePath);

  BinaryDependenciesResultList resultList( platform.operatingSystem() );

  for(const QFileInfo & binaryFilePath : binaryFilePathList){
    assert( !binaryFilePath.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
    assert( binaryFilePath.isAbsolute() );
    auto target = BinaryDependenciesFile::fromQFileInfo(binaryFilePath);
    BinaryDependenciesResult result( binaryFilePath, platform.operatingSystem() );
    mImpl->findDependencies(target, result, allDependencies, reader, platform);
    resultList.addResult(result);
  }

  /*
   * NOTE
   * It was tempted to return BinaryDependenciesFileList from this method,
   * in the hope to reuse the rpath that should have already been read.
   * In reality, the dependencies list does not contain the rpath.
   * So, do not try this again. Philippe Steinmann 2022/11/15
   */
//   return Impl::qStringListFromBinaryDependenciesFileList(allDependencies);

  return resultList;
}

Platform BinaryDependencies::setupFindDependencies(ExecutableFileReader & reader,
                                                   std::shared_ptr<AbstractSharedLibraryFinder> & shLibFinder,
                                                   const PathList & searchFirstPathPrefixList,
                                                   std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                                   const QFileInfo & target)
{
  assert( !reader.isOpen() );
  assert( shLibFinder.get() == nullptr );
  assert(qtDistributionDirectory.get() != nullptr);
  assert( fileInfoIsAbsolutePath(target) );

  reader.openFile(target);
  const Platform platform = reader.getFilePlatform();
  reader.close();

  if( platform.operatingSystem() == OperatingSystem::Unknown ){
    const QString message = tr("'%1' targets a operating system that is not supported")
                            .arg( target.fileName() );
    throw FindDependencyError(message);
  }

  const auto isExistingValidShLibOp = std::make_shared<const IsExistingValidSharedLibrary>(reader, platform);

  if( platform.operatingSystem() == OperatingSystem::Linux ){

    shLibFinder = std::make_shared<SharedLibraryFinderLinux>(isExistingValidShLibOp, qtDistributionDirectory);
    SharedLibraryFinderLinux *shLibFinderLinux = static_cast<SharedLibraryFinderLinux*>( shLibFinder.get() );
    shLibFinderLinux->buildSearchPathList( searchFirstPathPrefixList, platform.processorISA() );

  }else if( platform.operatingSystem() == OperatingSystem::Windows ){

    shLibFinder = std::make_shared<SharedLibraryFinderWindows>(isExistingValidShLibOp, qtDistributionDirectory);
    SharedLibraryFinderWindows *shLibFinderWindows = static_cast<SharedLibraryFinderWindows*>( shLibFinder.get() );
    shLibFinderWindows->buildSearchPathList(target, searchFirstPathPrefixList, platform.processorISA(), mCompilerFinder);

  }
  assert( shLibFinder.get() != nullptr );

  connect(shLibFinder.get(), &AbstractSharedLibraryFinder::statusMessage, this, &BinaryDependencies::message);
  connect(shLibFinder.get(), &AbstractSharedLibraryFinder::verboseMessage, this, &BinaryDependencies::verboseMessage);
  connect(shLibFinder.get(), &AbstractSharedLibraryFinder::debugMessage, this, &BinaryDependencies::debugMessage);

  emitSearchPathListMessage( shLibFinder->searchPathList() );

  mImpl->setSharedLibrariesFinder(shLibFinder);

  return platform;
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
