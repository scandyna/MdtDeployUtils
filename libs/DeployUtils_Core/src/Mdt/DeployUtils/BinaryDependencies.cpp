/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2023 Philippe Steinmann.
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
#include "QtDistributionDirectory.h"
#include "SharedLibraryFinderLinux.h"
#include "SharedLibraryFinderWindows.h"
#include "LibraryName.h"
#include "FileInfoUtils.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies/Graph.h"
#include "IsExistingValidSharedLibrary.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QDir>
#include <memory>
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

BinaryDependenciesResult
BinaryDependencies::findDependencies(const QFileInfo & binaryFilePath,
                                     const PathList & searchFirstPathPrefixList,
                                     std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory)
{
  assert( fileInfoIsAbsolutePath(binaryFilePath) );
  assert(qtDistributionDirectory.get() != nullptr);

  ExecutableFileReader reader;
  std::shared_ptr<AbstractSharedLibraryFinder> shLibFinder;

  const Platform platform = setupFindDependencies(reader, shLibFinder, searchFirstPathPrefixList, qtDistributionDirectory, binaryFilePath);

  using Impl::BinaryDependencies::Graph;

  Graph graph(platform);
  connect(&graph, &Graph::verboseMessage, this, &BinaryDependencies::verboseMessage);
  connect(&graph, &Graph::debugMessage, this, &BinaryDependencies::debugMessage);

  graph.addTarget(binaryFilePath);
  graph.findTransitiveDependencies(*shLibFinder, reader);

  return graph.getResult(binaryFilePath);
}

BinaryDependenciesResultList
BinaryDependencies::findDependencies(const QFileInfoList & binaryFilePathList,
                                     const PathList & searchFirstPathPrefixList,
                                     std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory)
{
  assert( !binaryFilePathList.isEmpty() );
  assert(qtDistributionDirectory.get() != nullptr);

  ExecutableFileReader reader;
  std::shared_ptr<AbstractSharedLibraryFinder> shLibFinder;

  const QFileInfo & firstBinaryFilePath = binaryFilePathList.at(0);

  const Platform platform = setupFindDependencies(reader, shLibFinder, searchFirstPathPrefixList, qtDistributionDirectory, firstBinaryFilePath);

  using Impl::BinaryDependencies::Graph;

  Graph graph(platform);
  connect(&graph, &Graph::verboseMessage, this, &BinaryDependencies::verboseMessage);
  connect(&graph, &Graph::debugMessage, this, &BinaryDependencies::debugMessage);

  graph.addTargets(binaryFilePathList);
  graph.findTransitiveDependencies(*shLibFinder, reader);

  return graph.getResultList(binaryFilePathList);
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
