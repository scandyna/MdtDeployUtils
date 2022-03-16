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

namespace Mdt{ namespace DeployUtils{

SharedLibrariesDeployer::SharedLibrariesDeployer(QObject *parent) noexcept
  : QObject(parent)
{
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

  mFoundDependencies = mBinaryDependencies.findDependencies(targetFilePathList, mSearchPrefixPathList);

  emitFoundDependenciesMessage();

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(mOverwriteBehavior);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);
  fileCopier.copyFiles(mFoundDependencies, destinationDirectoryPath);

  if( mPlatform.supportsRPath() ){
    setRPathToCopiedDependencies( fileCopier.copiedFilesDestinationPathList() );
  }
}

void SharedLibrariesDeployer::setRPathToCopiedDependencies(const QStringList & destinationFilePathList)
{
  assert( mPlatform.supportsRPath() );

  RPath rpath;
  if(!mRemoveRpath){
    rpath.appendPath( QLatin1String(".") );
  }

  ExecutableFileWriter writer;
  connect(&writer, &ExecutableFileWriter::message, this, &SharedLibrariesDeployer::verboseMessage);
  connect(&writer, &ExecutableFileWriter::verboseMessage, this, &SharedLibrariesDeployer::verboseMessage);

  for(const QString & filePath : destinationFilePathList){
    writer.openFile(filePath, mPlatform);
    if(writer.getRunPath() != rpath){
      const QString msg = tr("update rpath for %1").arg(filePath);
      emit verboseMessage(msg);
      writer.setRunPath(rpath);
    }
    writer.close();
  }
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
