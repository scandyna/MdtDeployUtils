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

  /*! \brief Returns a abolute path made relative to a base path
   *
   * Examples:
   * \code
   * const QString relativePath = relativePathToBase("/usr/lib", "/usr");
   * // relativePath: lib
   *
   * const QString relativePath = relativePathToBase("/usr/lib/x86_64-linux-gnu", "/usr");
   * // relativePath: lib/x86_64-linux-gnu
   * \endcode
   *
   * If given path ends with a slash, it will also be returned in the result:
   * \code
   * const QString relativePath = relativePathToBase("/usr/lib/", "/usr");
   * // relativePath: lib/
   * \endcode
   *
   * It does not matter if given base ends with a slash:
   * \code
   * const QString relativePath = relativePathToBase("/usr/lib", "/usr");
   * // relativePath: lib
   *
   * const QString relativePath = relativePathToBase("/usr/lib", "/usr/");
   * // relativePath: lib
   * \endcode
   *
   * \note This function does not require that path exist on the file system,
   *  it only does string manipulations
   * \sa https://en.cppreference.com/w/cpp/filesystem/relative
   * \pre \a path and \a base must not be empty
   * \pre \a path must start with \a base
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString relativePathToBase(const QString & path, const QString & base) noexcept;

  /*! \brief Check if given path is in given base
   *
   * Examples:
   * \code
   * b = pathIsInBase("/usr/lib", "/usr");
   * // b is true
   *
   * b = pathIsInBase("/usr/lib", "/lib");
   * // b is false
   * \endcode
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  bool pathIsInBase(const QString & path, const QString & base) noexcept;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_FILE_SYSTEM_UTILS_H
