// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "ExecutableFileInstaller.h"
#include "FileInfoUtils.h"
#include "FileCopier.h"
#include "FileCopierFile.h"
#include "ExecutableFileReader.h"
#include "ExecutableFileWriter.h"
#include <cassert>


namespace Mdt{ namespace DeployUtils{

ExecutableFileInstaller::ExecutableFileInstaller(const Platform & platform, QObject *parent) noexcept
 : QObject(parent),
   mPlatform(platform)
{
  assert( !mPlatform.isNull() );
}

void ExecutableFileInstaller::setOverwriteBehavior(OverwriteBehavior overwriteBehavior) noexcept
{
  mOverwriteBehavior = overwriteBehavior;
}

void ExecutableFileInstaller::install(const ExecutableFileToInstall & file, const QFileInfo & directoryPath, const RPath & installRPath)
{
  assert( fileInfoIsAbsolutePath(directoryPath) );

  FileCopier fileCopier;
  fileCopier.setOverwriteBehavior(mOverwriteBehavior);
  connect(&fileCopier, &FileCopier::verboseMessage, this, &ExecutableFileInstaller::verboseMessage);

  const QString directoryPathStr = directoryPath.absoluteFilePath();
  fileCopier.createDirectory(directoryPathStr);
  const FileCopierFile copierFile = fileCopier.copyFile(file.fileInfo(), directoryPathStr);
  if( copierFile.hasBeenCopied() && mPlatform.supportsRPath() ){
    auto copiedFile = CopiedExecutableFile::fromCopierFileAndFileToInstall(copierFile, file);
    if( file.haveToReadRPathFromFile() ){
      readRPathFromSourceFile(copiedFile);
    }
    setRPathToCopiedFiles({copiedFile}, installRPath);
  }
}

bool ExecutableFileInstaller::hasToUpdateRpath(const CopiedExecutableFile & file, const RPath & rpath, const PathList & systemWideLocations) const noexcept
{
  if(file.sourceFileRPath() == rpath){
    return false;
  }
  if( file.sourceFileRPath().isEmpty() ){
    if( !systemWideLocations.containsPath( file.sourceFileAbsoluteDirectoryPath() ) ){
      return false;
    }
  }

  return true;
}

void ExecutableFileInstaller::readRPathFromSourceFile(CopiedExecutableFile & file)
{
  ExecutableFileReader reader;
  reader.openFile(file.sourceFileInfo(), mPlatform);
  file.setSourceFileRPath( reader.getRunPath() );

  reader.close();
}

void ExecutableFileInstaller::setRPathToCopiedFiles(const CopiedExecutableFileList & copiedFiles, const RPath & rpath)
{
  assert( mPlatform.supportsRPath() );

  if( copiedFiles.empty() ){
    return;
  }

  emit statusMessage(
    tr("updating rpath for installed files")
  );

  ExecutableFileWriter writer;
  connect(&writer, &ExecutableFileWriter::message, this, &ExecutableFileInstaller::verboseMessage);
  connect(&writer, &ExecutableFileWriter::verboseMessage, this, &ExecutableFileInstaller::verboseMessage);

  const PathList systemWideLocations = PathList::getSystemLibraryKnownPathList(mPlatform);

  for(const CopiedExecutableFile & copiedFile : copiedFiles){
    if( hasToUpdateRpath(copiedFile, rpath, systemWideLocations) ){
      const QString msg = tr(" update rpath for %1").arg( copiedFile.destinationFileInfo().absoluteFilePath() );
      emit verboseMessage(msg);
      writer.openFile(copiedFile.destinationFileInfo(), mPlatform);
      writer.setRunPath(rpath);
    }
    writer.close();
  }
}

}} // namespace Mdt{ namespace DeployUtils{
