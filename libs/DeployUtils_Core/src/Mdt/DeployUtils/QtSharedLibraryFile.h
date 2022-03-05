/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_QT_SHARED_LIBRARY_FILE_H
#define MDT_DEPLOY_UTILS_QT_SHARED_LIBRARY_FILE_H

#include "mdt_deployutilscore_export.h"
#include <QFileInfo>
#include <QString>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a Qt shared library file
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtSharedLibraryFile
  {
   public:

    /*! \brief Construct a null file
     */
    QtSharedLibraryFile() = default;

    /*! \brief Copy construct a file from \a other
     */
    QtSharedLibraryFile(const QtSharedLibraryFile & other) = default;

    /*! \brief Copy assign \a other to this file
     */
    QtSharedLibraryFile & operator=(const QtSharedLibraryFile & other) = default;

    /*! \brief Move construct a file from \a other
     */
    QtSharedLibraryFile(QtSharedLibraryFile && other) = default;

    /*! \brief Move assign \a other to this file
     */
    QtSharedLibraryFile & operator=(QtSharedLibraryFile && other) noexcept = default;

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

    /*! \brief Check if \a library is a Qt shared library
     */
    static
    bool isQtSharedLibrary(const QFileInfo & library) noexcept;

    /*! \brief Construct a file from \a fileInfo
     *
     * \pre \a fileInfo must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \pre \a fileInfo must be a Qt shared library
     * \sa isQtSharedLibrary()
     */
    static
    QtSharedLibraryFile fromQFileInfo(const QFileInfo & fileInfo) noexcept
    {
      assert( !fileInfo.filePath().isEmpty() );
      assert( fileInfo.isAbsolute() );
      assert( isQtSharedLibrary(fileInfo) );

      return QtSharedLibraryFile(fileInfo);
    }

    /*! \brief Get the major version string out from \a fileName
     *
     * This helper works for Qt named shared libraries:
     * - libQt5Core.so -> returns 5
     * - Qt5Core.dll -> returns 5
     *
     * \pre \a fileName must be at least 3 char long
     *   If it starts with \a lib , it must be at least 6 chars long
     * \pre \a fileName must be of the Qt format (Qt5Core, libQt6Widgets, ...)
     */
    static
    QString getMajorVersionStringFromFileName(const QString & fileName) noexcept;

   private:

    QtSharedLibraryFile(const QFileInfo & fileInfo)
     : mFile(fileInfo)
    {
    }

    QFileInfo mFile;
  };

  /*! \brief List of QtSharedLibraryFile
   */
  using QtSharedLibraryFileList = std::vector<QtSharedLibraryFile>;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_SHARED_LIBRARY_FILE_H
