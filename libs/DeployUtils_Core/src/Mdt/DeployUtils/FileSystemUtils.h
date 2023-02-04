// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_FILE_SYSTEM_UTILS_H
#define MDT_DEPLOY_UTILS_FILE_SYSTEM_UTILS_H

#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Check if given path is an absolute path
   *
   * \note This function uses methods that don't require
   * the file to exist on the file system
   *
   * \sa fileInfoIsAbsolutePath()
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  bool isAbsolutePath(const QString & path) noexcept;

  /*! \brief Returns a absolute path from given path
   *
   * This functions uses QFileInfo::absoluteFilePath().
   *
   * The purpose is to hide the QFileInfo::absoluteFilePath() name,
   * which can be confusing.
   *
   * Also, for a non absolute path,
   * QFileInfo::absoluteFilePath() returns a surprising path,
   * relative to the app current directory.
   * This is avoided here by a precondition.
   *
   * Example:
   * \code
   * const QString path = makeAbsolutePath( QString("/opt") );
   * // On Windows, path will be C:/opt
   * \endcode
   *
   * \note This function uses methods that don't require
   * the file to exist on the file system
   *
   * \pre \a path must be a absolute path
   * \sa fileInfoIsAbsolutePath()
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString makeAbsolutePath(const QFileInfo & path) noexcept;

  /*! \brief Get the absolute directory path where given file lives in
   *
   * \pre \a path must be a absolute path
   * \sa fileInfoIsAbsolutePath()
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString absoluteDirectoryPathFromAbsoluteFilePath(const QFileInfo & filePath) noexcept;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_FILE_SYSTEM_UTILS_H
