/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2022 Philippe Steinmann.
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
#include "CopySharedLibrariesTargetDependsOn.h"
#include "BinaryDependencies.h"
#include "CompilerFinder.h"
#include "FileCopier.h"
#include "PathList.h"
#include "RPath.h"
#include "ExecutableFileReader.h"
#include "ExecutableFileWriter.h"
#include <QLatin1String>
#include <cassert>
#include <memory>

// #include <QDebug>
// #include <iostream>

namespace Mdt{ namespace DeployUtils{

void CopySharedLibrariesTargetDependsOn::execute(const CopySharedLibrariesTargetDependsOnRequest & request)
{
  assert( !request.targetFilePath.trimmed().isEmpty() );
  assert( !request.destinationDirectoryPath.trimmed().isEmpty() );

  ExecutableFileReader reader;
  reader.openFile(request.targetFilePath);
  const Platform platform = reader.getFilePlatform();
  reader.close();

  const QString startMessage = tr("Copy dependencies for %1").arg(request.targetFilePath);
  emit statusMessage(startMessage);

  emitSearchPrefixPathListMessage(request.searchPrefixPathList);

  const QString overwriteBehaviorMessage = tr("Overwrite behavior: %1").arg( overwriteBehaviorToString(request.overwriteBehavior) );
  emit verboseMessage(overwriteBehaviorMessage);

  if( platform.supportsRPath() ){
    if(request.removeRpath){
      const QString rpathMessage = tr("RPATH will be removed in copied libraries");
      emit verboseMessage(rpathMessage);
    }else{
      const QString rpathMessage = tr("RPATH will be set to $ORIGIN in copied libraries");
      emit verboseMessage(rpathMessage);
    }
  }

  BinaryDependencies binaryDependencies;
  connect(&binaryDependencies, &BinaryDependencies::message, this, &CopySharedLibrariesTargetDependsOn::statusMessage);
  connect(&binaryDependencies, &BinaryDependencies::verboseMessage, this, &CopySharedLibrariesTargetDependsOn::verboseMessage);

  if( request.compilerLocationType != CompilerLocationType::Undefined ){
    auto compilerFinder = std::make_shared<CompilerFinder>();
    switch(request.compilerLocationType){
      case CompilerLocationType::FromEnv:
        /// \todo Implement
        break;
      case CompilerLocationType::VcInstallDir:
        compilerFinder->setInstallDir(request.compilerLocationValue, Compiler::Msvc);
        break;
      case CompilerLocationType::CompilerPath:
        compilerFinder->findFromCxxCompilerPath(request.compilerLocationValue);
        break;
      case CompilerLocationType::Undefined:
        // Just to avoid compiler warnings
        break;
    }
    if( !compilerFinder->hasInstallDir() ){
      const QString msg = tr(
        "it was requested to find the compiler redistribute directory, but this failed"
        "(maybe your compiler is currently not supported for this feature)"
      );
      throw FindCompilerError(msg);
    }
    binaryDependencies.setCompilerFinder(compilerFinder);
  }

  const QStringList dependencies = binaryDependencies.findDependencies( request.targetFilePath, PathList::fromStringList(request.searchPrefixPathList) );

  emitFoundDependenciesMessage(dependencies);

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(request.overwriteBehavior);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &CopySharedLibrariesTargetDependsOn::verboseMessage);
  fileCopier.copyFiles(dependencies, request.destinationDirectoryPath);

  if( platform.supportsRPath() ){
    setRPathToCopiedDependencies(fileCopier.copiedFilesDestinationPathList(), request, platform);
  }
}

void CopySharedLibrariesTargetDependsOn::emitSearchPrefixPathListMessage(const QStringList & pathList) const noexcept
{
  const QString startMessage = tr("search prefix path list:");
  emit verboseMessage(startMessage);

  for(const QString & path : pathList){
    const QString msg = tr(" %1").arg(path);
    emit verboseMessage(msg);
  }
}

void CopySharedLibrariesTargetDependsOn::emitFoundDependenciesMessage(const QStringList & dependencies) const noexcept
{
  const QString startMessage = tr("found dependencies:");
  emit verboseMessage(startMessage);

  for(const QString & dependency : dependencies){
    const QString msg = tr(" %1").arg(dependency);
    emit verboseMessage(msg);
  }
}

QString CopySharedLibrariesTargetDependsOn::overwriteBehaviorToString(OverwriteBehavior overwriteBehavior) noexcept
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

void CopySharedLibrariesTargetDependsOn::setRPathToCopiedDependencies(const QStringList & destinationFilePathList,
                                                                      const CopySharedLibrariesTargetDependsOnRequest & request,
                                                                      const Platform & platform)
{
  assert( platform.supportsRPath() );

  RPath rpath;
  if(!request.removeRpath){
    rpath.appendPath( QLatin1String(".") );
  }

  ExecutableFileWriter writer;
  connect(&writer, &ExecutableFileWriter::message, this, &CopySharedLibrariesTargetDependsOn::verboseMessage);
  connect(&writer, &ExecutableFileWriter::verboseMessage, this, &CopySharedLibrariesTargetDependsOn::verboseMessage);

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

}} // namespace Mdt{ namespace DeployUtils{
