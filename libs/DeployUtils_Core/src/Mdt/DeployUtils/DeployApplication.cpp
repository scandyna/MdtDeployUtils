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
#include "DeployApplication.h"
#include "DestinationDirectory.h"
#include "OperatingSystem.h"
#include "FileCopier.h"
#include "PathList.h"
#include "RPath.h"
#include "ExecutableFileReader.h"
#include "ExecutableFileWriter.h"
#include "QtSharedLibrary.h"
#include "QtSharedLibraryFile.h"
#include "QtModulePlugins.h"
#include "QtPlugins.h"
#include "QtConf.h"
#include "QtConfWriter.h"
#include "DestinationDirectoryQtConf.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QStringList>
#include <QDir>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

DeployApplication::DeployApplication(QObject *parent)
  : QObject(parent),
    mQtDistributionDirectory( std::make_shared<QtDistributionDirectory>() )
{
}

void DeployApplication::execute(const DeployApplicationRequest & request)
{
  assert( !request.targetFilePath.trimmed().isEmpty() );
  assert( !request.destinationDirectoryPath.trimmed().isEmpty() );
  assert( !request.runtimeDestination.trimmed().isEmpty() );
  assert( !request.libraryDestination.trimmed().isEmpty() );

  emit statusMessage(
    tr("Deploy application for executable %1")
    .arg(request.targetFilePath)
  );

  ExecutableFileReader reader;
  reader.openFile(request.targetFilePath);
  mPlatform = reader.getFilePlatform();
  reader.close();

  if( mPlatform.operatingSystem() == OperatingSystem::Unknown ){
    const QString message = tr("'%1' targets a operating system that is not supported")
                            .arg(request.targetFilePath);
    throw FindDependencyError(message);
  }

  if( QDir::isAbsolutePath(request.runtimeDestination) ){
    const QString message = tr("runtime destination must not be a absolute path, given: ")
                            .arg(request.runtimeDestination);
    throw DeployApplicationError(message);
  }

  if( QDir::isAbsolutePath(request.libraryDestination) ){
    const QString message = tr("library destination must not be a absolute path, given: ")
                            .arg(request.libraryDestination);
    throw DeployApplicationError(message);
  }

  const auto destination = DestinationDirectory::fromPathAndStructure(
    request.destinationDirectoryPath,
    destinationDirectoryStructureFromRuntimeAndLibraryDestination( request, mPlatform.operatingSystem() )
  );

  emit statusMessage(
    tr("Destination directory is %1")
    .arg( destination.path() )
  );

  emit verboseMessage(
    tr("Executable targets %1")
    .arg( osName( mPlatform.operatingSystem() ) )
  );

  setupShLibDeployer(request);

  const BinaryDependenciesResult librariesExecutableDependsOn = mShLibDeployer->findSharedLibrariesTargetDependsOn(request.targetFilePath);
  if( !librariesExecutableDependsOn.isSolved() ){
    throwApplicationDependenciesNotSolvedError(librariesExecutableDependsOn);
  }

  const QtPluginFileList qtPlugins = getRequiredQtPlugins(librariesExecutableDependsOn, request);

  BinaryDependenciesResultList libraries = findSharedLibrariesQtPluginsDependsOn(qtPlugins);
  if( !libraries.isSolved() ){
    throwQtPluginsDependenciesNotSolvedError(libraries);
  }

  libraries.addResult(librariesExecutableDependsOn);
  assert( libraries.isSolved() );

  makeDirectoryStructure(destination);
  installExecutable( request, destination.structure() );
  ///copySharedLibrariesTargetDependsOn(request);
  ///deployRequiredQtPlugins(destination, request);

  installSharedLibraries(libraries);

  installQtPlugins(qtPlugins, destination, request.shLibOverwriteBehavior);

  writeQtConfFile(destination);
}

DestinationDirectoryStructure
DeployApplication::destinationDirectoryStructureFromRuntimeAndLibraryDestination(const DeployApplicationRequest & request, OperatingSystem os) noexcept
{
  assert( !request.runtimeDestination.trimmed().isEmpty() );
  assert( !request.libraryDestination.trimmed().isEmpty() );
  assert( os != OperatingSystem::Unknown );

  DestinationDirectoryStructure structure;

  structure.setExecutablesDirectory(request.runtimeDestination);

  switch(os){
    case OperatingSystem::Linux:
      structure.setSharedLibrariesDirectory(request.libraryDestination);
      break;
    case OperatingSystem::Windows:
      structure.setSharedLibrariesDirectory(request.runtimeDestination);
      break;
    case OperatingSystem::Unknown:
      break;
  }

  structure.setQtPluginsRootDirectory( DestinationDirectoryStructure::qtPluginsRootDirectoryFromOs(os) );

  return structure;
}

QString DeployApplication::getMissingLibrariesListText(const BinaryDependenciesResult & result) const noexcept
{
  assert( !result.isSolved() );

  QStringList missingLibraries;
  for(const BinaryDependenciesResultLibrary & library : result){
    if( library.isMissing() ){
      missingLibraries.append( library.libraryName() );
    }
  }

  return missingLibraries.join( QLatin1String(", ") );
}

void DeployApplication::throwQtPluginsDependenciesNotSolvedError(const BinaryDependenciesResultList & resultList) const
{
  assert( !resultList.isSolved() );

  QString notSolvedPluginsMsg;
  for(const BinaryDependenciesResult & result : resultList){
    if( !result.isSolved() ){
      notSolvedPluginsMsg += tr("\n %1: missing libraries: %2")
                             .arg( result.target().fileName(), getMissingLibrariesListText(result) );
    }
  }

  const QString msg = tr("some Qt plugins depends on shared libraries that have not been found: %1")
                      .arg(notSolvedPluginsMsg);
  throw FindDependencyError(msg);
}

void DeployApplication::throwApplicationDependenciesNotSolvedError(const BinaryDependenciesResult & result) const
{
  assert( !result.isSolved() );

  const QString msg = tr("some shared libraries %1 depends on could not be found. missing libraries: %2")
                      .arg( result.target().fileName(), getMissingLibrariesListText(result) );
  throw FindDependencyError(msg);
}

void DeployApplication::setupShLibDeployer(const DeployApplicationRequest & request)
{
  if(mShLibDeployer.get() == nullptr){
    mShLibDeployer = std::make_shared<SharedLibrariesDeployer>(mQtDistributionDirectory);
    connect(mShLibDeployer.get(), &SharedLibrariesDeployer::statusMessage, this, &DeployApplication::statusMessage);
    connect(mShLibDeployer.get(), &SharedLibrariesDeployer::verboseMessage, this, &DeployApplication::verboseMessage);
    connect(mShLibDeployer.get(), &SharedLibrariesDeployer::debugMessage, this, &DeployApplication::debugMessage);
  }

  mShLibDeployer->setSearchPrefixPathList( PathList::fromStringList(request.searchPrefixPathList) );
  mShLibDeployer->setOverwriteBehavior(request.shLibOverwriteBehavior);
  mShLibDeployer->setRemoveRpath(request.removeRpath);

  /// \todo else: clear compiler finder !
  if( !request.compilerLocation.isNull() ){
    mShLibDeployer->setCompilerLocation(request.compilerLocation);
  }
}

void DeployApplication::makeDirectoryStructure(const DestinationDirectory & destination)
{
  assert( !mPlatform.isNull() );

  emit verboseMessage(
    tr("Make directory structure")
  );

  mBinDirDestinationPath = destination.executablesDirectoryPath();
  emit debugMessage(
    tr("Create directory %1")
    .arg(mBinDirDestinationPath)
  );
  FileCopier::createDirectory(mBinDirDestinationPath);

  mLibDirDestinationPath = destination.sharedLibrariesDirectoryPath();
  if(mLibDirDestinationPath != mBinDirDestinationPath){
    emit debugMessage(
      tr("Create directory %1")
      .arg(mLibDirDestinationPath)
    );
    FileCopier::createDirectory(mLibDirDestinationPath);
  }
}

void DeployApplication::installExecutable(const DeployApplicationRequest & request, const DestinationDirectoryStructure & destinationStructure)
{
  assert( !mBinDirDestinationPath.isEmpty() );
  assert( !request.targetFilePath.isEmpty() );
  assert( !destinationStructure.isNull() );

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(OverwriteBehavior::Overwrite);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &DeployApplication::verboseMessage);

  fileCopier.copyFile(request.targetFilePath, mBinDirDestinationPath);
  assert( fileCopier.copiedFilesDestinationPathList().size() == 1 );

  if( mPlatform.supportsRPath() ){
    setRPathToInstalledExecutable(fileCopier.copiedFilesDestinationPathList().at(0), request, destinationStructure);
  }
}

void DeployApplication::setRPathToInstalledExecutable(const QString & executableFilePath, const DeployApplicationRequest & request,
                                                      const DestinationDirectoryStructure & destinationStructure)
{
  assert( mPlatform.supportsRPath() );
  assert( !destinationStructure.isNull() );

  RPath rpath;
  if(!request.removeRpath){
    rpath.appendPath( destinationStructure.executablesToSharedLibrariesRelativePath() );
  }

  ExecutableFileWriter writer;
  connect(&writer, &ExecutableFileWriter::message, this, &DeployApplication::verboseMessage);
  connect(&writer, &ExecutableFileWriter::verboseMessage, this, &DeployApplication::verboseMessage);

  writer.openFile(executableFilePath, mPlatform);
  if(writer.getRunPath() != rpath){
    const QString msg = tr("update rpath for %1").arg(executableFilePath);
    emit verboseMessage(msg);
    writer.setRunPath(rpath);
  }
  writer.close();
}

void DeployApplication::installSharedLibraries(const BinaryDependenciesResultList & libraries)
{
  assert( !mLibDirDestinationPath.isEmpty() );
  assert( mShLibDeployer.get() != nullptr );

  mShLibDeployer->installSharedLibraries(libraries, mLibDirDestinationPath);
}

void DeployApplication::copySharedLibrariesTargetDependsOn(const DeployApplicationRequest & request)
{
  assert( !mLibDirDestinationPath.isEmpty() );
  assert( mShLibDeployer.get() != nullptr );

  // Take the given target, it has the RPath informations
  mShLibDeployer->copySharedLibrariesTargetDependsOn(request.targetFilePath, mLibDirDestinationPath);

  /// mSharedLibrariesTargetDependsOn = mShLibDeployer->foundDependencies();
}


QtPluginFileList DeployApplication::getRequiredQtPlugins(const BinaryDependenciesResult & libraries, const DeployApplicationRequest & request)
{
  assert( libraries.isSolved() );
  assert( mShLibDeployer.get() != nullptr );
  assert( mQtDistributionDirectory.get() != nullptr );

  emit verboseMessage(
    tr("get Qt libraries out from dependencies (will be used to know which Qt plugins are required)")
  );

  const QStringList librariesToRedistribute = getLibrariesToRedistributeFilePathList(libraries);

  const QtSharedLibraryFileList qtSharedLibraries = QtSharedLibrary::getQtSharedLibraries(librariesToRedistribute);

  QtModulePlugins qtModulePlugins;
  connect(&qtModulePlugins, &QtModulePlugins::statusMessage, this, &DeployApplication::statusMessage);
  connect(&qtModulePlugins, &QtModulePlugins::verboseMessage, this, &DeployApplication::verboseMessage);
  connect(&qtModulePlugins, &QtModulePlugins::debugMessage, this, &DeployApplication::debugMessage);

  return qtModulePlugins.getQtPluginsQtLibrariesDependsOn(qtSharedLibraries, request.qtPluginsSet, *mQtDistributionDirectory);
}

BinaryDependenciesResultList DeployApplication::findSharedLibrariesQtPluginsDependsOn(const QtPluginFileList & plugins)
{
  assert( mShLibDeployer.get() != nullptr );
  assert( !mPlatform.isNull() );

  if( plugins.empty() ){
    return BinaryDependenciesResultList( mPlatform.operatingSystem() );
  }

  return mShLibDeployer->findSharedLibrariesTargetsDependsOn( toFileInfoList(plugins) );
}

void DeployApplication::installQtPlugins(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior)
{
  assert( mShLibDeployer.get() != nullptr );

  QtPlugins qtPluginsInstaller(mShLibDeployer);
  connect(&qtPluginsInstaller, &QtPlugins::statusMessage, this, &DeployApplication::statusMessage);
  connect(&qtPluginsInstaller, &QtPlugins::verboseMessage, this, &DeployApplication::verboseMessage);
  connect(&qtPluginsInstaller, &QtPlugins::debugMessage, this, &DeployApplication::debugMessage);

  qtPluginsInstaller.installQtPlugins(plugins, destination, overwriteBehavior);
}

void DeployApplication::writeQtConfFile(const DestinationDirectory & destination)
{
  assert( !destination.structure().isNull() );

  emit statusMessage(
    tr("writing qt.conf")
  );

  QtConf conf;
  setQtConfPathEntries( conf, destination.structure() );

  QtConfWriter writer;
  connect(&writer, &QtConfWriter::verboseMessage, this, &DeployApplication::verboseMessage);

  writer.writeConfToDirectory( conf, destination.executablesDirectoryPath() );
}

QString DeployApplication::osName(OperatingSystem os) noexcept
{
  assert(os != OperatingSystem::Unknown);

  switch(os){
    case OperatingSystem::Linux:
      return QLatin1String("Linux");
    case OperatingSystem::Windows:
      return QLatin1String("Windows");
    case OperatingSystem::Unknown:
      break;
  }

  return QString();
}

}} // namespace Mdt{ namespace DeployUtils{
