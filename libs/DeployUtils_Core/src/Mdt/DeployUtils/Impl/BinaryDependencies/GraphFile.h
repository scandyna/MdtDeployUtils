// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_FILE_H
#define MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_FILE_H

#include "Mdt/DeployUtils/RPath.h"
#include "Mdt/DeployUtils/FileInfoUtils.h"
#include "Mdt/DeployUtils/BinaryDependenciesFile.h"
#include <QFileInfo>
#include <QString>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

  /*! \internal Helper class used as vertex data in a binary dependencies graph
   */
  class GraphFile
  {
   public:

    /*! \brief Construct a null file
     */
    GraphFile() = default;

    /*! \brief Copy construct a file from \a other
     */
    GraphFile(const GraphFile & other) = default;

    /*! \brief Copy assign \a other to this file
     */
    GraphFile & operator=(const GraphFile & other) = default;

    /*! \brief Move construct a file from \a other
     */
    GraphFile(GraphFile && other) = default;

    /*! \brief Move assign \a other to this file
     */
    GraphFile & operator=(GraphFile && other) noexcept = default;

    /*! \brief Check if this file is null
     *
     * This file is null if it has no file name.
     */
    bool isNull() const noexcept
    {
      return !fileInfoHasFileName(mFile);
    }

    /*! \brief Check if this file has its file name
     */
    bool hasFileName() const noexcept
    {
      return fileInfoHasFileName(mFile);
    }

    /*! \brief Check if this file has its absolute path
     */
    bool hasAbsolutePath() const noexcept
    {
      return fileInfoIsAbsolutePath(mFile);
    }

    /*! \brief Get the file name of this file
     */
    QString fileName() const noexcept
    {
      return mFile.fileName();
    }

    /*! \brief Get the file info of this file
     */
    const QFileInfo & fileInfo() const noexcept
    {
      return mFile;
    }

    /*! \brief Check if this file has to be read
     */
    bool hasToBeRead() const noexcept
    {
      if( isReaden() ){
        return false;
      }
      if( isNotFound() ){
        return false;
      }
      if( shouldNotBeRedistributed() ){
        return false;
      }
      if( !hasAbsolutePath() ){
        return false;
      }

      return true;
    }

    /*! \brief Check if this file have been readen
     *
     * A file is readen when:
     * - its absolute file path have been set
     * - it has been read to extract its direct dependencies
     *   and maybe its rpath
     */
    bool isReaden() const noexcept
    {
      return mIsReaden;
    }

    /*! \brief Set the absolute file path
     *
     * \pre \a path must be a absolute path
     * \sa fileInfoIsAbsolutePath()
     */
    void setAbsoluteFilePath(const QFileInfo & path) noexcept
    {
      assert( fileInfoIsAbsolutePath(path) );

      mFile = path;
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

    /*! \brief Mark this file as not found
     *
     * A file is not found after it has been searched on the file system,
     * but could not be found.
     */
    void markAsNotFound() noexcept
    {
      mIsNotFound = true;
    }

    /*! \brief Check if this file has not been found
     */
    bool isNotFound() const noexcept
    {
      return mIsNotFound;
    }

    /*! \brief Mark this file as not to be distributed
     */
    void markAsNotToBeRedistributed() noexcept
    {
      mShouldNotBeRedistributed = true;
    }

    /*! \brief Check if this file should not be redistributed
     */
    bool shouldNotBeRedistributed() const noexcept
    {
      return mShouldNotBeRedistributed;
    }

    /*! \brief Check if this file has been searched
     *
     * This file has been searched if:
     * - It has its absolute file path (successfully found)
     * - It has been market as not found
     * - It has been marked as not to be distributed
     */
    bool hasBeenSearched() const noexcept
    {
      if( shouldNotBeRedistributed() ){
        return true;
      }
      if( isNotFound() ){
        return true;
      }
      if( hasAbsolutePath() ){
        return true;
      }

      return false;
    }

    /*! \brief Mark this file as readen
     *
     * \pre This file must have its absolute path
     */
    void markAsReaden() noexcept
    {
      assert( hasAbsolutePath() );

      mIsReaden = true;
    }

    /*! \brief Get a BinaryDependenciesFile from this file
     *
     * \pre This file must not be null
     * \sa isNull()
     */
    BinaryDependenciesFile toBinaryDependenciesFile() const noexcept
    {
      assert( !isNull() );

      auto bdFile = BinaryDependenciesFile::fromQFileInfo(mFile);
      bdFile.setRPath(mRPath);

      return bdFile;
    }

    /*! \brief Construct a file from given library name
     *
     * \pre \a name must only be a file name without any path
     * \sa fileInfoIsFileNameWithoutPath()
     */
    static
    GraphFile fromLibraryName(const QString & name) noexcept
    {
      assert( fileInfoIsFileNameWithoutPath(name) );

      return GraphFile(name);
    }

    /*! \brief Construct a file from \a fileInfo
     *
     * \pre \a fileInfo must at least have a file name
     * \sa fileInfoHasFileName()
     */
    static
    GraphFile fromQFileInfo(const QFileInfo & fileInfo) noexcept
    {
      assert( fileInfoHasFileName(fileInfo) );

      return GraphFile(fileInfo);
    }

   private:

    GraphFile(const QFileInfo & fileInfo)
      : mFile(fileInfo)
    {
    }

    bool mIsReaden = false;
    bool mIsNotFound = false;
    bool mShouldNotBeRedistributed = false;
    QFileInfo mFile;
    RPath mRPath;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace BinaryDependencies{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BINARY_DEPENDENCIES_GRAPH_FILE_H
