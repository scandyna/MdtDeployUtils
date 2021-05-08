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
    QString absoluteFilePath() const
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
    LibraryName libraryName() const
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

    /*! \brief Check if \a extension refers to a shared library
     *
     * \a extension can also be a versionned shared library,
     * like so.1.2.3
     *
     * \pre \a extension must not begin with a dot ('.').
     *
     * \sa LibraryName::extension()
     * \sa QFileInfo::suffix()
     */
    static
    bool isSharedLibraryExtension(const QString & extension) noexcept
    {
      assert( !extension.startsWith( QChar::fromLatin1('.') ) );

      if( extension.startsWith( QLatin1String("so") ) ){
        return true;
      }
      if( QString::compare( extension, QLatin1String("dll"), Qt::CaseInsensitive ) == 0 ){
        return true;
      }
      if( QString::compare( extension, QLatin1String("dylib"), Qt::CaseSensitive ) == 0 ){
        return true;
      }

      return false;
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
     */
    static
    bool isSharedLibraryFile(const QString & filePath) noexcept;

    /*! \brief Get the library name from \a filePath
     */
    static
    LibraryName libraryNameFromFile(const QString & filePath) noexcept;

    /*! \brief Get a library info from a file
     *
     * \pre \a filePath must refer to a shared library
     * \sa isSharedLibraryFile()
     */
    static
    LibraryInfo fromFile(const QString & filePath) noexcept;

   private:

    LibraryName mLibraryName;
    QString mAbsoluteFilePath;
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::LibraryInfo)

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_INFO_H
