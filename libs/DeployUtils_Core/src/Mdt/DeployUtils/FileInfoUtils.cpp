// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "FileInfoUtils.h"
#include <QString>
#include <QLatin1String>

namespace Mdt{ namespace DeployUtils{

bool fileInfoHasFileName(const QFileInfo & fileInfo) noexcept
{
  return !fileInfo.fileName().trimmed().isEmpty();
}

bool fileInfoIsFileNameWithoutPath(const QFileInfo & fileInfo) noexcept
{
  /*
   * See Qt doc on QDir constructors
   * A empty path represents the current app directory (".")
   * So, QFileInfo::path() will be a path to app directory,
   * nothing related to this current file (!!)
   */
  const QString path = fileInfo.path().trimmed();
  if( !path.isEmpty() && ( path != QLatin1String(".") ) ){
    return false;
  }

  return fileInfoHasFileName(fileInfo);
}

bool fileInfoIsAbsolutePath(const QFileInfo & fileInfo) noexcept
{
  return fileInfo.isAbsolute();
}

}} // namespace Mdt{ namespace DeployUtils{
