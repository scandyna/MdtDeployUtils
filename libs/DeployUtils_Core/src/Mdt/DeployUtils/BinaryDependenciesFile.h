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
#include <QStringList>
#include <QFileInfo>
#include <algorithm>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a file and its attributes used by BinaryDependencies
   *
   * \todo see https://gitlab.com/scandyna/mdtdeployutils/-/issues/5 (BinaryDependenciesFile has to much attributes and is confusing)
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

    /*! \brief Set the list of dependencies of this file
     */
    void setDependenciesFileNames(const QStringList & fileNames) noexcept
    {
      mDependenciesFileNames = fileNames;
    }

    /*! \brief Get the list of dependencies of this file
     */
    const QStringList & dependenciesFileNames() const noexcept
    {
      return mDependenciesFileNames;
    }

    /*! \brief Remove the dependencies with a given predicate \a p
     *
     * \a p is a unary predicate which returns ​true if the element should be removed.
     *
     * \sa https://en.cppreference.com/w/cpp/algorithm/remove
     */
    template<typename UnaryPredicate>
    void removeDependenciesFileNames(UnaryPredicate p) noexcept
    {
      mDependenciesFileNames.erase(
        std::remove_if( mDependenciesFileNames.begin(), mDependenciesFileNames.end(), p),
        mDependenciesFileNames.end()
      );
    }

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
     * \sa doc of QFileInfo::absoluteFilePath()
     */
    static
    BinaryDependenciesFile fromQFileInfo(const QFileInfo & fileInfo) noexcept
    {
      assert( !fileInfo.filePath().isEmpty() );
      assert( fileInfo.isAbsolute() );

      return BinaryDependenciesFile(fileInfo);
    }

   private:

    BinaryDependenciesFile(const QFileInfo & fileInfo)
     : mFile(fileInfo)
    {
    }

    QFileInfo mFile;
    QStringList mDependenciesFileNames;
    RPath mRPath;
  };

  /*! \brief List of BinaryDependenciesFile
   */
  using BinaryDependenciesFileList = std::vector<BinaryDependenciesFile>;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_BINARY_DEPENDENCIES_FILE_H
