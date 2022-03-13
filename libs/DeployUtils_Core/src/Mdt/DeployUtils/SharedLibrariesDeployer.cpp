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
#include "BinaryDependencies.h"
#include "FileCopier.h"
#include "RPath.h"
#include <QLatin1String>
#include <QStringBuilder>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

void SharedLibrariesDeployer::setSearchPrefixPathList(const PathList & pathList) noexcept
{
  mSearchPrefixPathList = pathList;
}

void SharedLibrariesDeployer::setCompilerLocation(const CompilerLocationRequest & location)
{
  assert( !location.isNull() );

  mCompilerFinder = std::make_shared<CompilerFinder>();
  switch( location.type() ){
    case CompilerLocationType::FromEnv:
      /// \todo Implement
      break;
    case CompilerLocationType::VcInstallDir:
      mCompilerFinder->setInstallDir(location.value(), Compiler::Msvc);
      break;
    case CompilerLocationType::CompilerPath:
      mCompilerFinder->findFromCxxCompilerPath( location.value() );
      break;
    case CompilerLocationType::Undefined:
      // Just to avoid compiler warnings
      break;
  }
  if( !mCompilerFinder->hasInstallDir() ){
    const QString msg = tr(
      "it was requested to find the compiler redistribute directory, but this failed "
      "(maybe your compiler is currently not supported for this feature)"
    );
    throw FindCompilerError(msg);
  }
}

void SharedLibrariesDeployer::setOverwriteBehavior(OverwriteBehavior overwriteBehavior) noexcept
{
  mOverwriteBehavior = overwriteBehavior;
}

void SharedLibrariesDeployer::setRemoveRpath(bool remove) noexcept
{
  mRemoveRpath = remove;
}

void SharedLibrariesDeployer::copySharedLibrariesTargetDependsOn(const QFileInfo & targetFilePath, const QString & destinationDirectoryPath)
{
  assert( !targetFilePath.filePath().isEmpty() );
  assert( targetFilePath.isAbsolute() );
  assert( !destinationDirectoryPath.isEmpty() );

//   mFoundDependencies.clear();

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
  const Platform platform = reader.getFilePlatform();
  reader.close();

//   if( platform.operatingSystem() == OperatingSystem::Unknown ){
//     const QString message = tr("'%1' targets a operating system that is not supported")
//                             .arg( targetFilePathList.at(0).fileName() );
//     throw FindDependencyError(message);
//   }

  emitStartMessage(targetFilePathList);

  emitSearchPrefixPathListMessage();

  const QString overwriteBehaviorMessage = tr("overwrite behavior: %1").arg( overwriteBehaviorToString(mOverwriteBehavior) );
  emit verboseMessage(overwriteBehaviorMessage);

  if( platform.supportsRPath() ){
    if(mRemoveRpath){
      const QString rpathMessage = tr("RPATH will be removed in copied libraries");
      emit verboseMessage(rpathMessage);
    }else{
      const QString rpathMessage = tr("RPATH will be set to $ORIGIN in copied libraries");
      emit verboseMessage(rpathMessage);
    }
  }

  BinaryDependencies binaryDependencies;
  connect(&binaryDependencies, &BinaryDependencies::message, this, &SharedLibrariesDeployer::statusMessage);
  connect(&binaryDependencies, &BinaryDependencies::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);

  if(mCompilerFinder.get() != nullptr){
    binaryDependencies.setCompilerFinder(mCompilerFinder);
  }

  mFoundDependencies = binaryDependencies.findDependencies(targetFilePathList, mSearchPrefixPathList);
//   addFoundDependencies(depenedncies);

  emitFoundDependenciesMessage();

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(mOverwriteBehavior);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);
  fileCopier.copyFiles(mFoundDependencies, destinationDirectoryPath);

  if( platform.supportsRPath() ){
    setRPathToCopiedDependencies(fileCopier.copiedFilesDestinationPathList(), platform);
  }
}

void SharedLibrariesDeployer::setRPathToCopiedDependencies(const QStringList & destinationFilePathList, const Platform & platform)
{
  assert( platform.supportsRPath() );

  RPath rpath;
  if(!mRemoveRpath){
    rpath.appendPath( QLatin1String(".") );
  }

  ExecutableFileWriter writer;
  connect(&writer, &ExecutableFileWriter::message, this, &SharedLibrariesDeployer::verboseMessage);
  connect(&writer, &ExecutableFileWriter::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);

  for(const QString & filePath : destinationFilePathList){
    writer.openFile(filePath, platform);
    if(writer.getRunPath() != rpath){
      const QString msg = tr("update rpath for %1").arg(filePath);
      emit verboseMessage(msg);
      writer.setRunPath(rpath);
    }
    writer.close();
  }

}

// void SharedLibrariesDeployer::addFoundDependencies(const QStringList & depenedncies) noexcept
// {
//   mFoundDependencies.append(depenedncies);
//   mFoundDependencies.removeDuplicates();
// }

void SharedLibrariesDeployer::emitStartMessage(const QFileInfoList & targetFilePathList) const
{
  if( targetFilePathList.isEmpty() ){
    return;
  }

  const int size = targetFilePathList.size();
  assert(size >= 1);

  QString targetFilePathListString = targetFilePathList.at(0).fileName();
  for(int i=1; i < size; ++i){
    targetFilePathListString += QLatin1String(", ") % targetFilePathList.at(i).fileName();
  }

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

void SharedLibrariesDeployer::emitFoundDependenciesMessage() const
{
  const QString startMessage = tr("found dependencies:");
  emit verboseMessage(startMessage);

  for(const QString & dependency : mFoundDependencies){
    const QString msg = tr(" %1").arg(dependency);
    emit verboseMessage(msg);
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
