// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "CopiedExecutableFile.h"
#include "FileInfoUtils.h"
#include "FileSystemUtils.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

QString CopiedExecutableFile::sourceFileAbsoluteDirectoryPath() const noexcept
{
  return absoluteDirectoryPathFromAbsoluteFilePath(mSourceFileInfo);
}

void CopiedExecutableFile::setSourceFileRPath(const RPath & rpath) noexcept
{
  mRPath = rpath;
}

CopiedExecutableFile
CopiedExecutableFile::fromCopierFileAndFileToInstall(const FileCopierFile & copierFile, const ExecutableFileToInstall & fileToInstall) noexcept
{
  assert( copierFile.hasBeenCopied() );

  return CopiedExecutableFile( copierFile.sourceFileInfo(), copierFile.destinationFileInfo(), fileToInstall.rPath() );
}

CopiedExecutableFile::CopiedExecutableFile(const QFileInfo & sourcePath, const QFileInfo & destinationPath, const RPath & rpath) noexcept
 : mSourceFileInfo(sourcePath),
   mDestinationFileInfo(destinationPath),
   mRPath(rpath)
{
  assert( fileInfoIsAbsolutePath(mSourceFileInfo) );
  assert( fileInfoIsAbsolutePath(mDestinationFileInfo) );
}

}} // namespace Mdt{ namespace DeployUtils{
