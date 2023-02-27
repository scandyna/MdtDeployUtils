// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_COPIED_EXECUTABLE_FILE_H
#define MDT_DEPLOY_UTILS_COPIED_EXECUTABLE_FILE_H

#include "ExecutableFileToInstall.h"
#include "FileCopierFile.h"
#include "RPath.h"
#include "mdt_deployutilscore_export.h"
#include <QFileInfo>
#include <QString>
#include <vector>

namespace Mdt{ namespace DeployUtils{

  /*! \internal Copied file used by ExecutableFileInstaller
   */
  class MDT_DEPLOYUTILSCORE_EXPORT CopiedExecutableFile
  {
   public:

    CopiedExecutableFile() = delete;

    /*! \brief Get the source file info
     *
     * \post Returned file info is a absolute path
     */
    const QFileInfo & sourceFileInfo() const noexcept
    {
      return mSourceFileInfo;
    }

    /*! \brief Get the absolute path of the source file directory
     */
    QString sourceFileAbsoluteDirectoryPath() const noexcept;

    /*! \brief Get the destination file info
     *
     * \post Returned file info is a absolute path
     */
    const QFileInfo & destinationFileInfo() const noexcept
    {
      return mDestinationFileInfo;
    }

    /*! \brief Set the source file rpath
     */
    void setSourceFileRPath(const RPath & rpath) noexcept;

    /*! \brief Get the source file rpath
     */
    const RPath & sourceFileRPath() const noexcept
    {
      return mRPath;
    }

    /*! \brief Construct from given files
     *
     * \pre \a copierFile must have been copied
     */
    static
    CopiedExecutableFile fromCopierFileAndFileToInstall(const FileCopierFile & copierFile, const ExecutableFileToInstall & fileToInstall) noexcept;

   private:

    CopiedExecutableFile(const QFileInfo & sourcePath, const QFileInfo & destinationPath, const RPath & rpath) noexcept;

    QFileInfo mSourceFileInfo;
    QFileInfo mDestinationFileInfo;
    RPath mRPath;
  };

  /*! \internal
   */
  using CopiedExecutableFileList = std::vector<CopiedExecutableFile>;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_COPIED_EXECUTABLE_FILE_H
