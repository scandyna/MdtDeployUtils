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
#include "SharedLibrariesDeployer.h"
#include "ExecutableFileReader.h"
#include "ExecutableFileWriter.h"
#include "BinaryDependenciesResultLibrary.h"
#include "CompilerFinder.h"
#include "FileCopier.h"
#include "RPath.h"
#include "Algorithm.h"
#include "FileInfoUtils.h"
#include <QLatin1String>
#include <QStringBuilder>
#include <memory>
#include <cassert>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

SharedLibrariesDeployer::SharedLibrariesDeployer(std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                                 QObject *parent) noexcept
  : QObject(parent),
    mQtDistributionDirectory(qtDistributionDirectory)
{
  assert(qtDistributionDirectory.get() != nullptr);

  connect(&mBinaryDependencies, &BinaryDependencies::message, this, &SharedLibrariesDeployer::statusMessage);
  connect(&mBinaryDependencies, &BinaryDependencies::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);
  connect(&mBinaryDependencies, &BinaryDependencies::debugMessage, this, &SharedLibrariesDeployer::debugMessage);
}

void SharedLibrariesDeployer::setSearchPrefixPathList(const PathList & pathList) noexcept
{
  mSearchPrefixPathList = pathList;
}

void SharedLibrariesDeployer::setCompilerLocation(const CompilerLocationRequest & location)
{
  assert( !location.isNull() );

  auto compilerFinder = std::make_shared<CompilerFinder>();
  switch( location.type() ){
    case CompilerLocationType::FromEnv:
      /// \todo Implement
      break;
    case CompilerLocationType::VcInstallDir:
      compilerFinder->setInstallDir(location.value(), Compiler::Msvc);
      break;
    case CompilerLocationType::CompilerPath:
      compilerFinder->findFromCxxCompilerPath( location.value() );
      break;
    case CompilerLocationType::Undefined:
      // Just to avoid compiler warnings
      break;
  }
  if( !compilerFinder->hasInstallDir() ){
    const QString msg = tr(
      "it was requested to find the compiler redistribute directory, but this failed "
      "(maybe your compiler is currently not supported for this feature)"
    );
    throw FindCompilerError(msg);
  }

  mBinaryDependencies.setCompilerFinder(compilerFinder);
}

void SharedLibrariesDeployer::setOverwriteBehavior(OverwriteBehavior overwriteBehavior) noexcept
{
  mOverwriteBehavior = overwriteBehavior;
}

void SharedLibrariesDeployer::setRemoveRpath(bool remove) noexcept
{
  mRemoveRpath = remove;
}

bool SharedLibrariesDeployer::hasToUpdateRpath(const CopiedSharedLibraryFile & file, const RPath & rpath, const PathList & systemWideLocations) const noexcept
{
  if(file.rpath == rpath){
    return false;
  }
  if( file.rpath.isEmpty() ){
    if( !systemWideLocations.containsPath( file.file.sourceFileInfo().absoluteFilePath() ) ){
      return false;
    }
  }

  return true;
}

BinaryDependenciesResult SharedLibrariesDeployer::findSharedLibrariesTargetDependsOn(const QFileInfo & target)
{
  assert( fileInfoIsAbsolutePath(target) );

  setCurrentPlatformFromFile(target);

  emitStartMessage(target);

  emitSearchPrefixPathListMessage();

  const BinaryDependenciesResult dependencies
    = mBinaryDependencies.findDependencies(target, mSearchPrefixPathList, mQtDistributionDirectory);

  emitFoundDependenciesMessage(dependencies);

  return dependencies;
}

BinaryDependenciesResultList SharedLibrariesDeployer::findSharedLibrariesTargetsDependsOn(const QFileInfoList & targets)
{
  assert( !targets.isEmpty() );

  setCurrentPlatformFromFile( targets.at(0) );

  emitStartMessage(targets);

  emitSearchPrefixPathListMessage();

  BinaryDependenciesResultList dependencies
    = mBinaryDependencies.findDependencies(targets, mSearchPrefixPathList, mQtDistributionDirectory);

  emitFoundDependenciesMessage(dependencies);

  return dependencies;
}

void SharedLibrariesDeployer::installSharedLibraries(const BinaryDependenciesResultList & libraries, const QString & destinationDirectoryPath)
{
  assert( libraries.isSolved() );
  assert( !destinationDirectoryPath.trimmed().isEmpty() );

  if( libraries.isEmpty() ){
    return;
  }

  emit statusMessage(
    tr("installing shared libraries")
  );

  const QString overwriteBehaviorMessage = tr("overwrite behavior: %1").arg( overwriteBehaviorToString(mOverwriteBehavior) );
  emit verboseMessage(overwriteBehaviorMessage);

  if( mPlatform.supportsRPath() ){
    if(mRemoveRpath){
      const QString rpathMessage = tr("RPATH will be removed in copied libraries");
      emit verboseMessage(rpathMessage);
    }else{
      const QString rpathMessage = tr("RPATH will be set to $ORIGIN in copied libraries");
      emit verboseMessage(rpathMessage);
    }
  }

  const CopiedSharedLibraryFileList copiedFiles = copySharedLibraries(libraries, destinationDirectoryPath);

  if( mPlatform.supportsRPath() ){
    setRPathToCopiedDependencies(copiedFiles);
  }
}

CopiedSharedLibraryFileList
SharedLibrariesDeployer::copySharedLibraries(const BinaryDependenciesResultList & resultList, const QString & destinationDirectoryPath)
{
  CopiedSharedLibraryFileList copiedFiles;

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(mOverwriteBehavior);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);

  fileCopier.createDirectory(destinationDirectoryPath);

  /// \todo should become getLibrariesToInstall() and build a ExecutableFileToInstallList
  const auto libraries = getLibrariesToRedistribute(resultList);

  for(const BinaryDependenciesResultLibrary & library : libraries){
    CopiedSharedLibraryFile copiedShLib;
    copiedShLib.file = fileCopier.copyFile(library.absoluteFilePath(), destinationDirectoryPath);
    if( copiedShLib.file.hasBeenCopied() ){
      copiedShLib.rpath = library.rPath();
      copiedFiles.push_back(copiedShLib);
    }
  }

  return copiedFiles;
}

void SharedLibrariesDeployer::copySharedLibrariesTargetDependsOn(const QFileInfo & targetFilePath, const QString & destinationDirectoryPath)
{
  assert( !targetFilePath.filePath().isEmpty() );
  assert( targetFilePath.isAbsolute() );
  assert( !destinationDirectoryPath.isEmpty() );

  copySharedLibrariesTargetsDependsOnImpl(QFileInfoList{targetFilePath}, destinationDirectoryPath);
}

void SharedLibrariesDeployer::setCurrentPlatformFromFile(const QFileInfo & file)
{
  assert( fileInfoIsAbsolutePath(file) );

  ExecutableFileReader reader;
  reader.openFile(file);
  mPlatform = reader.getFilePlatform();
  reader.close();
}

void SharedLibrariesDeployer::copySharedLibrariesTargetsDependsOnImpl(const QFileInfoList & targetFilePathList, const QString & destinationDirectoryPath)
{
  assert( !targetFilePathList.isEmpty() );
  assert( !destinationDirectoryPath.isEmpty() );

  const BinaryDependenciesResultList dependencies = findSharedLibrariesTargetsDependsOn(targetFilePathList);

  installSharedLibraries(dependencies, destinationDirectoryPath);

  return;
}

void SharedLibrariesDeployer::setRPathToCopiedSharedLibraries(const CopiedSharedLibraryFileList & copiedFiles, const RPath & rpath)
{
  assert( mPlatform.supportsRPath() );

  if( copiedFiles.empty() ){
    return;
  }

  emit statusMessage(
    tr("updating rpath for installed shared libraries")
  );

  ExecutableFileWriter writer;
  connect(&writer, &ExecutableFileWriter::message, this, &SharedLibrariesDeployer::verboseMessage);
  connect(&writer, &ExecutableFileWriter::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);

  const PathList systemWideLocations = PathList::getSystemLibraryKnownPathList(mPlatform);

  for(const CopiedSharedLibraryFile & copiedFile : copiedFiles){
    if( hasToUpdateRpath(copiedFile, rpath, systemWideLocations) ){
      const QString msg = tr("update rpath for %1").arg( copiedFile.file.destinationFileInfo().absoluteFilePath() );
      emit verboseMessage(msg);
      writer.openFile(copiedFile.file.destinationFileInfo(), mPlatform);
      writer.setRunPath(rpath);
    }
    writer.close();
  }
}

void SharedLibrariesDeployer::setRPathToCopiedDependencies(const CopiedSharedLibraryFileList & copiedFiles)
{
  assert( mPlatform.supportsRPath() );

  RPath rpath;
  if(!mRemoveRpath){
    rpath.appendPath( QLatin1String(".") );
  }

  setRPathToCopiedSharedLibraries(copiedFiles, rpath);
}

void SharedLibrariesDeployer::emitStartMessage(const QFileInfo & target) const noexcept
{
  emit statusMessage(
    tr("find dependencies for %1")
    .arg( target.fileName() )
  );
}

void SharedLibrariesDeployer::emitStartMessage(const QFileInfoList & targetFilePathList) const
{
  if( targetFilePathList.isEmpty() ){
    return;
  }

  const auto toQString = [](const QFileInfo & fi){
    return fi.fileName();
  };

  const QString targetFilePathListString = joinToQString( targetFilePathList, toQString, QLatin1String(", ") );

  emit statusMessage(
    tr("find dependencies for %1")
    .arg(targetFilePathListString)
  );
}

void SharedLibrariesDeployer::emitSearchPrefixPathListMessage() const
{
  const QString startMessage = tr("given search first prefix path list:");
  emit verboseMessage(startMessage);

  for(const QString & path : mSearchPrefixPathList){
    const QString msg = tr(" %1").arg(path);
    emit verboseMessage(msg);
  }
}

void SharedLibrariesDeployer::emitResultMessages(const BinaryDependenciesResult & result) const noexcept
{
  QString solvedText;
  if( result.isSolved() ){
    solvedText = tr("yes");
  }else{
    solvedText = tr("no");
  }
  const QString targetMessage = tr(" %1 (solved: %2):")
                                .arg(result.target().fileName(), solvedText);
  emit verboseMessage(targetMessage);

  for(const BinaryDependenciesResultLibrary & library : result){
    if( library.isFound() ){
      const QString msg = tr("  %1").arg( library.absoluteFilePath() );
      emit verboseMessage(msg);
    }else{
      if( library.shouldNotBeRedistributed() ){
        const QString msg = tr("  %1 (not to redistribute)").arg( library.libraryName() );
        emit verboseMessage(msg);
      }else{
        const QString msg = tr("  %1 (not found)").arg( library.libraryName() );
        emit verboseMessage(msg);
      }
    }
  }
}

void SharedLibrariesDeployer::emitFoundDependenciesMessage(const BinaryDependenciesResult & result) const noexcept
{
  const QString startMessage = tr("found dependencies:");
  emit verboseMessage(startMessage);

  emitResultMessages(result);
}

void SharedLibrariesDeployer::emitFoundDependenciesMessage(const BinaryDependenciesResultList & resultList) const noexcept
{
  const QString startMessage = tr("found dependencies:");
  emit verboseMessage(startMessage);

  for(const BinaryDependenciesResult & result : resultList){
    emitResultMessages(result);
  }
}

QString SharedLibrariesDeployer::overwriteBehaviorToString(OverwriteBehavior overwriteBehavior) noexcept
{
  switch(overwriteBehavior){
    case OverwriteBehavior::Keep:
      return tr("keep");
    case OverwriteBehavior::Overwrite:
      return tr("overwrite");
    case OverwriteBehavior::Fail:
      return tr("fail");
  }

  return QString();
}

}} // namespace Mdt{ namespace DeployUtils{
