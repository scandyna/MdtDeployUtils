// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "ExecutableFileToInstall.h"
#include "FileInfoUtils.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

ExecutableFileToInstall ExecutableFileToInstall::fromFilePath(const QFileInfo & filePath) noexcept
{
  assert( fileInfoIsAbsolutePath(filePath) );

  return ExecutableFileToInstall( true, filePath, RPath() );
}

ExecutableFileToInstall ExecutableFileToInstall::fromFilePathAndRPath(const QFileInfo & filePath, const RPath & rpath) noexcept
{
  assert( fileInfoIsAbsolutePath(filePath) );

  return ExecutableFileToInstall(false, filePath, rpath);
}

}} // namespace Mdt{ namespace DeployUtils{
