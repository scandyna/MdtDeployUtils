// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_TO_INSTALL_H
#define MDT_DEPLOY_UTILS_EXECUTABLE_FILE_TO_INSTALL_H

#include "RPath.h"
#include "mdt_deployutilscore_export.h"
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Informations about a executable file to install with ExecutableFileInstaller
   *
   */
  class MDT_DEPLOYUTILSCORE_EXPORT ExecutableFileToInstall
  {
   public:

    ExecutableFileToInstall() = delete;

    /*! \brief Get the file info of this file
     */
    const QFileInfo & fileInfo() const noexcept
    {
      return mFile;
    }

    /*! \brief Check if the rpath have to be read from the file
     */
    bool haveToReadRPathFromFile() const noexcept
    {
      return mHaveToReadRPathFromFile;
    }

    /*! \brief Get the rpath of this file
     */
    const RPath & rPath() const noexcept
    {
      return mRPath;
    }

    /*! \brief Construct a file from given path
     *
     * \pre \a filePath must be a absolute path
     */
    static
    ExecutableFileToInstall fromFilePath(const QFileInfo & filePath) noexcept;

    /*! \brief Construct a file from given path and rpath
     *
     * For platforms that supports rpath,
     * it can happen that the rpath informations have already been read before.
     * Giving those here avoids ExecutableFileInstaller to read them again.
     *
     * \pre \a filePath must be a absolute path
     * \note \a rpath can be empty
     */
    static
    ExecutableFileToInstall fromFilePathAndRPath(const QFileInfo & filePath, const RPath & rpath) noexcept;

   private:

    ExecutableFileToInstall(bool haveToReadRPathFromFile, const QFileInfo & filePath, const RPath & rpath) noexcept
     : mHaveToReadRPathFromFile(haveToReadRPathFromFile),
       mFile(filePath),
       mRPath(rpath)
    {
    }

    bool mHaveToReadRPathFromFile;
    QFileInfo mFile;
    RPath mRPath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_TO_INSTALL_H
