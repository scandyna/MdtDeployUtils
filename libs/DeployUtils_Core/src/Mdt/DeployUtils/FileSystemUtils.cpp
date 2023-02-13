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

QString relativePathToBase(const QString & path, const QString & base) noexcept
{
  assert( !path.trimmed().isEmpty() );
  assert( !base.trimmed().isEmpty() );
  assert( path.startsWith(base) );

  using sizeType = QString::size_type;

  /*
   * path:   /rootDir/lib
   * base:   /rootDir
   * result: lib
   *
   * path:   /usr/lib
   * base:   /usr
   * result: lib
   *
   * path:   /usr/lib
   * base:   /usr/
   * result: lib
   *
   * path:   /usr/lib/
   * base:   /usr
   * result: lib/
   *
   * path:   /usr/lib/
   * base:   /usr/
   * result: lib/
   */
  sizeType toRemove = path.length() - base.length();
  if( !base.endsWith( QLatin1Char('/') ) ){
    --toRemove;
  }

  return path.right(toRemove);
}

bool pathIsInBase(const QString & path, const QString & base) noexcept
{
  return path.startsWith(base);
}

}} // namespace Mdt{ namespace DeployUtils{
