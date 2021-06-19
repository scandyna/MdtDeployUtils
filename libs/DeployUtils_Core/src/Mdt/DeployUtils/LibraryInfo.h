/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_H
#define MDT_DEPLOY_UTILS_LIBRARY_INFO_H

#include "LibraryName.h"
#include "mdt_deployutils_export.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <QMetaType>
#include <cassert>
// #include <initializer_list>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Data value class that stores informations about a library
   */
  class MDT_DEPLOYUTILS_EXPORT LibraryInfo
  {
   public:

    /*! \brief Set absolute file path
     */
//     void setAbsoluteFilePath(const QString & path);

    /*! \brief Get absolute file path
     */
    QString absoluteFilePath() const noexcept
    {
      return mAbsoluteFilePath;
    }

    /*! \brief Set library name
     *
     * \a name should be the full library file name (f.ex. libQt5Core.so, or Qt5Core.dll).
     *  To specify only the library name (like Qt5Core), use setLibraryName().
     *
     * \pre \a name must at least have a library name and a extension.
     */
//     void setLibraryPlatformName(const QString & name);

    /*! \brief Set library name
     *
     * \sa LibraryName
     */
//     void setLibraryName(const LibraryName & libraryName);

    /*! \brief Get library name
     */
    const LibraryName & libraryName() const noexcept
    {
      return mLibraryName;
    }

    /*! \brief Check if this library info is null
     */
    bool isNull() const
    {
      return ( mLibraryName.isNull() || mAbsoluteFilePath.isEmpty() );
    }

    /*! \brief Check if library info a and b are equal
     */
    friend
    bool operator==(const LibraryInfo & a, const LibraryInfo & b)
    {
      if( QString::compare( a.mAbsoluteFilePath, b.mAbsoluteFilePath, Qt::CaseSensitive ) != 0 ){
        return false;
      }
      if( QString::compare( a.mLibraryName.name(), b.mLibraryName.name(), Qt::CaseSensitive ) != 0 ){
        return false;
      }
      return true;
    }

    /*! \brief Check if library info a and b are different
     */
    friend
    bool operator!=(const LibraryInfo & a, const LibraryInfo & b)
    {
      return !(a == b);
    }

    /*! \brief Check if \a filePath refers to a shared library file
     *
     * Returns true if \a filePath refers to a shared library.
     *
     * For example, Qt5Core is not a shared library file name,
     * so false is returned.
     *
     * Qt5Core.dll, or libQt5Core.so are valid shared libraries,
     * so true is returned.
     *
     * /opt/libQt5Core.so is also a valid shared library,
     * so true is returned.
     *
     * \sa LibraryNameExtension
     */
    static
    bool isSharedLibraryFile(const QString & filePath) noexcept;


    /*! \brief Get a library info from a file
     *
     * \pre \a filePath must refer to a shared library
     * \sa isSharedLibraryFile()
     */
    static
    LibraryInfo fromFile(const QString & filePath) noexcept
    {
      return LibraryInfo();
    }

   private:

    /*! \brief Get the library name from \a filePath
     */
    static
    LibraryName libraryNameFromFile(const QString & filePath) noexcept;

    LibraryName mLibraryName;
    QString mAbsoluteFilePath;
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::LibraryInfo)

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_H
