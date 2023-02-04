// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "FileSystemUtils.h"
#include "FileInfoUtils.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

bool isAbsolutePath(const QString & path) noexcept
{
  return fileInfoIsAbsolutePath(path);
}

QString makeAbsolutePath(const QFileInfo & path) noexcept
{
  assert( fileInfoIsAbsolutePath(path) );

  return path.absoluteFilePath();
}

QString absoluteDirectoryPathFromAbsoluteFilePath(const QFileInfo & filePath) noexcept
{
  assert( fileInfoIsAbsolutePath(filePath) );

  return filePath.absolutePath();
}

}} // namespace Mdt{ namespace DeployUtils{
