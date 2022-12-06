// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_FILE_INFO_UTILS_H
#define MDT_DEPLOY_UTILS_FILE_INFO_UTILS_H

#include "mdt_deployutilscore_export.h"
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Check if given file info has its file name
   *
   * \note This function uses methods that don't require
   * the file to exist on the file system
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  bool fileInfoHasFileName(const QFileInfo & fileInfo) noexcept;

  /*! \brief Check if given file is a file name without any path
   *
   * \note This function uses methods that don't require
   * the file to exist on the file system
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  bool fileInfoIsFileNameWithoutPath(const QFileInfo & fileInfo) noexcept;

  /*! \brief Check if given file info is an absolute path
   *
   * \note This function uses methods that don't require
   * the file to exist on the file system
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  bool fileInfoIsAbsolutePath(const QFileInfo & fileInfo) noexcept;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_FILE_INFO_UTILS_H
