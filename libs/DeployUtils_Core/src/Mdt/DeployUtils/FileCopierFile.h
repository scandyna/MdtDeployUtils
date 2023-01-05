/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_FILE_COPIER_FILE_H
#define MDT_DEPLOY_UTILS_FILE_COPIER_FILE_H

#include "mdt_deployutilscore_export.h"
#include <QFileInfo>
#include <vector>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a file with its source and destination paths
   *
   * \sa FileCopier
   */
  class MDT_DEPLOYUTILSCORE_EXPORT FileCopierFile
  {
   public:

    /*! \brief Set the source file info
     *
     * \pre \a file must have its absolute file path set
     * \sa fileInfoIsAbsolutePath()
     */
    void setSourceFileInfo(const QFileInfo & file) noexcept;

    /*! \brief Get the source file info
     */
    const QFileInfo & sourceFileInfo() const noexcept
    {
      return mSourceFileInfo;
    }

    /*! \brief Get the absolute path of the source file
     */
    QString sourceAbsoluteFilePath() const noexcept
    {
      return mSourceFileInfo.absoluteFilePath();
    }

    /*! \brief Set the destination file info
     *
     * \pre \a file must have its absolute file path set
     * \sa fileInfoIsAbsolutePath()
     */
    void setDestinationFileInfo(const QFileInfo & file) noexcept;

    /*! \brief Get the destination file info
     */
    const QFileInfo & destinationFileInfo() const noexcept
    {
      return mDestinationFileInfo;
    }

    /*! \brief Get the absolute path of the destination file
     */
    QString destinationAbsoluteFilePath() const noexcept
    {
      return mDestinationFileInfo.absoluteFilePath();
    }

    /*! \brief Mark this file as been copied
     */
    void setAsBeenCopied() noexcept
    {
      mHasBeenCopied = true;
    }

    /*! \brief Returns true if source file has been copied to the destination file
     */
    bool hasBeenCopied() const noexcept
    {
      return mHasBeenCopied;
    }

   private:

    bool mHasBeenCopied = false;
    QFileInfo mSourceFileInfo;
    QFileInfo mDestinationFileInfo;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_FILE_COPIER_FILE_H
