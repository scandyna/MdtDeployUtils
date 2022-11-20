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
#include "SharedLibrariesDeployer.h"
#include "ExecutableFileReader.h"
#include "ExecutableFileWriter.h"
#include "CompilerFinder.h"
#include "FileCopier.h"
#include "RPath.h"
#include "Algorithm.h"
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

CopiedSharedLibraryFileList
SharedLibrariesDeployer::copySharedLibraries(const QStringList & libraries, const QString & destinationDirectoryPath)
{
  CopiedSharedLibraryFileList copiedFiles;

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(mOverwriteBehavior);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);

  fileCopier.createDirectory(destinationDirectoryPath);

  for(const QString & library : libraries){
    CopiedSharedLibraryFile copiedShLib;
    copiedShLib.file = fileCopier.copyFile(library, destinationDirectoryPath);
    if( copiedShLib.file.hasBeenCopied() ){
      ExecutableFileReader reader;
      reader.openFile(library);
      copiedShLib.rpath = reader.getRunPath();
      reader.close();

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

void SharedLibrariesDeployer::copySharedLibrariesTargetsDependsOnImpl(const QFileInfoList & targetFilePathList, const QString & destinationDirectoryPath)
{
  assert( !targetFilePathList.isEmpty() );
//   assert( !targetFilePath.filePath().isEmpty() );
//   assert( targetFilePath.isAbsolute() );
  assert( !destinationDirectoryPath.isEmpty() );

  ExecutableFileReader reader;
  reader.openFile( targetFilePathList.at(0) );
  mPlatform = reader.getFilePlatform();
  reader.close();

  emitStartMessage(targetFilePathList);

  emitSearchPrefixPathListMessage();

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

//   mFoundDependencies = mBinaryDependencies.findDependencies(targetFilePathList, mSearchPrefixPathList, mQtDistributionDirectory);

  const BinaryDependenciesResultList dependencies
    = mBinaryDependencies.findDependencies(targetFilePathList, mSearchPrefixPathList, mQtDistributionDirectory);

  emitFoundDependenciesMessage(dependencies);

  mFoundDependencies = getLibrariesAbsoluteFilePathList(dependencies);

  const CopiedSharedLibraryFileList copiedFiles = copySharedLibraries(mFoundDependencies, destinationDirectoryPath);

  if( mPlatform.supportsRPath() ){
    setRPathToCopiedDependencies(copiedFiles);
  }
}

void SharedLibrariesDeployer::setRPathToCopiedSharedLibraries(const CopiedSharedLibraryFileList & copiedFiles, const RPath & rpath)
{
  assert( mPlatform.supportsRPath() );

  if( copiedFiles.empty() ){
    return;
  }

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
    tr("copy dependencies for %1")
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

void SharedLibrariesDeployer::emitFoundDependenciesMessage(const BinaryDependenciesResultList & resultList) const
{
  const QString startMessage = tr("found dependencies:");
  emit verboseMessage(startMessage);

//   for(const QString & dependency : mFoundDependencies){
//     const QString msg = tr(" %1").arg(dependency);
//     emit verboseMessage(msg);
//   }

  for(const BinaryDependenciesResult & result : resultList){
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
        const QString msg = tr("  %1 (not found)").arg( library.libraryName() );
        emit verboseMessage(msg);
      }
    }
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
