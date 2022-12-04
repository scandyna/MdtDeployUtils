// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "FileInfoUtils.h"

namespace Mdt{ namespace DeployUtils{

bool fileInfoHasFileName(const QFileInfo & fileInfo) noexcept
{
  return !fileInfo.fileName().trimmed().isEmpty();
}

bool fileInfoIsAbsolutePath(const QFileInfo & fileInfo) noexcept
{
  return fileInfo.isAbsolute();
}

}} // namespace Mdt{ namespace DeployUtils{
