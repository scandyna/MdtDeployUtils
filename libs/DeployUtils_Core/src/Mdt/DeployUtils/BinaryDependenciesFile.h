/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2023 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_FILE_H
#define MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_FILE_H

#include "RPath.h"
#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QFileInfo>
#include <algorithm>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a file and its attributes used by BinaryDependencies
   */
  class MDT_DEPLOYUTILSCORE_EXPORT BinaryDependenciesFile
  {
   public:

    /*! \brief Construct a null file
     */
    BinaryDependenciesFile() = default;

    /*! \brief Copy construct a file from \a other
     */
    BinaryDependenciesFile(const BinaryDependenciesFile & other) = default;

    /*! \brief Copy assign \a other to this file
     */
    BinaryDependenciesFile & operator=(const BinaryDependenciesFile & other) = default;

    /*! \brief Move construct a file from \a other
     */
    BinaryDependenciesFile(BinaryDependenciesFile && other) = default;

    /*! \brief Move assign \a other to this file
     */
    BinaryDependenciesFile & operator=(BinaryDependenciesFile && other) noexcept = default;

    /*! \brief Check if this file is null
     *
     * This file is null if it has no file name.
     */
    bool isNull() const noexcept
    {
      return fileName().isEmpty();
    }

    /*! \brief Get the file info of this file
     */
    const QFileInfo & fileInfo() const noexcept
    {
      return mFile;
    }

    /*! \brief Get the file name of this file
     */
    QString fileName() const noexcept
    {
      return mFile.fileName();
    }

    /*! \brief Get the absolute path of this file
     */
    QString absoluteFilePath() const noexcept
    {
      return mFile.absoluteFilePath();
    }

    /*! \brief Get the absolute path of the directory this file lives in
     *
     * \pre this file must be a absolute path
     */
    QString absoluteDirectoryPath() const noexcept;

    /*! \brief Set the rpath to this file
     */
    void setRPath(const RPath & rpath) noexcept
    {
      mRPath = rpath;
    }

    /*! \brief Get the rpath of this file
     */
    const RPath & rPath() const noexcept
    {
      return mRPath;
    }

    /*! \brief Construct a file from given library name
     *
     * \pre \a name must only be a file name without any path
     */
    static
    BinaryDependenciesFile fromLibraryName(const QFileInfo & name) noexcept;

    /*! \brief Construct a file from \a fileInfo
     *
     * \pre \a fileInfo must have its absolute file path set
     */
    static
    BinaryDependenciesFile fromQFileInfo(const QFileInfo & fileInfo) noexcept;

   private:

    BinaryDependenciesFile(const QFileInfo & fileInfo)
     : mFile(fileInfo)
    {
    }

    QFileInfo mFile;
    RPath mRPath;
  };

  /*! \brief List of BinaryDependenciesFile
   */
  using BinaryDependenciesFileList = std::vector<BinaryDependenciesFile>;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_FILE_H
