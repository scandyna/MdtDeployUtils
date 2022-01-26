/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_FILE_COPIER_H
#define MDT_DEPLOY_UTILS_FILE_COPIER_H

#include "FileCopyError.h"
#include "OverwriteBehavior.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Provides utilities for files and directories manipulation
   */
  class MDT_DEPLOYUTILSCORE_EXPORT FileCopier : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit FileCopier(QObject* parent = nullptr);

    /*! \brief Create a directory
     *
     * Will create a directory designed by \a directoryPath
     *  if it not allready exists.
     *  If parent directories are missing,
     *  they will also be created.
     *
     * \exception FileCopyError
     */
    static
    void createDirectory(const QString & directoryPath);

    /*! \brief Set the overwrite behavior for this file copier
     *
     * The default overwrite behavior is OverwriteBehavior::Fail
     *
     * \sa overwriteBehavior()
     */
    void setOverwriteBehavior(OverwriteBehavior behavior) noexcept;

    /*! \brief Get the overwrite behavior
     *
     * \sa setOverwriteBehavior()
     */
    OverwriteBehavior overwriteBehavior() const noexcept
    {
      return mOverwriteBehavior;
    }

    /*! \brief Copy \a sourceFilePath to \a destinationDirectoryPath
     *
     * If the source file allready exists in the destination location,
     * but its not the same as the source
     * (source and destination locations are different),
     * the behavior is defined by \a overwriteBehavior :
     * - If \a overwriteBehavior is OverwriteBehavior::Keep, the destination file will not be changed at all.
     * - If \a overwriteBehavior is OverwriteBehavior::Overwrite, the destination file will replaced.
     * - If \a overwriteBehavior is OverwriteBehavior::Fail, a fatal error is thrown.
     *
     * \pre \a sourceFilePath must refer to a existing file
     * \pre \a destinationDirectoryPath must be a existing directory
     * \exception FileCopyError
     * \sa overwriteBehavior()
     * \sa createDirectory()
     */
    void copyFile(const QString & sourceFilePath, const QString & destinationDirectoryPath);

    /*! \brief Copy a list of files to \a destinationDirectoryPath
     *
     * \exception FileCopyError
     * \sa copyFile()
     */
    void copyFiles(const QStringList & sourceFilePathList, const QString & destinationDirectoryPath);

    /*! \brief Get a list of destination paths to the files that have been copied
     *
     * \sa clearCopiedFilesDestinationPathList()
     */
    const QStringList & copiedFilesDestinationPathList() const noexcept
    {
      return mCopiedFilesDestinationPathList;
    }

    /*! \brief Clear the list of destination paths to the files that have been copied
     *
     * \sa copiedFilesDestinationPathList()
     */
    void clearCopiedFilesDestinationPathList() noexcept
    {
      mCopiedFilesDestinationPathList.clear();
    }

    /*! \brief Check if \a directoryPath is a path to a existing directory
     */
    static
    bool isExistingDirectory(const QString & directoryPath) noexcept;

    /*! \brief Get the destination file path from \a sourceFilePath and \a destinationDirectoryPath
     */
    static
    QString getDestinationFilePath(const QString & sourceFilePath, const QString & destinationDirectoryPath) noexcept;

   signals:

//     void message(const QString & message) const;
    void verboseMessage(const QString & message) const;

   private:

    static
    QString getDestinationFilePath(const QFileInfo & sourceFile, const QString & destinationDirectoryPath) noexcept;

    OverwriteBehavior mOverwriteBehavior = OverwriteBehavior::Fail;
    QStringList mCopiedFilesDestinationPathList;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_FILE_COPIER_H
