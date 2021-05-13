/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_NAME_EXTENSION_H
#define MDT_DEPLOY_UTILS_LIBRARY_NAME_EXTENSION_H

#include "mdt_deployutils_export.h"
#include <QString>
#include <QChar>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Representation of a library name extension
   *
   * A shared library extension contains only the extension itself,
   * without the dot ('.'), and without any version.
   * \sa isSharedLibraryExtension()
   *
   * \sa LibraryName
   */
  class MDT_DEPLOYUTILS_EXPORT LibraryNameExtension
  {
   public:

    /*! \brief Construct a null extension
     */
    explicit LibraryNameExtension() noexcept = default;

    /*! \brief Copy construct a extension from \a other
     */
    LibraryNameExtension(const LibraryNameExtension & other) = default;

    /*! \brief Copy assign \a other to this extension
     */
    LibraryNameExtension & operator=(const LibraryNameExtension & other) = default;

    /*! \brief Move construct a extension from \a other
     */
    LibraryNameExtension(LibraryNameExtension && other) noexcept = default;

    /*! \brief Move assign \a other to this extension
     */
    LibraryNameExtension & operator=(LibraryNameExtension && other) noexcept = default;

    /*! \brief Check if this extension is null
     */
    bool isNull() const noexcept
    {
      return mName.isEmpty();
    }

    /*! \brief Get the name of this extension
     */
    QString name() const noexcept
    {
      return mName;
    }

    /*! \brief Check if this extension is a so (shared object)
     */
    bool isSo() const noexcept
    {
      return isSo(mName);
    }

    /*! \brief Check if \a extensionName is a so (shared object)
     *
     * \pre \a extensionName must not begin with a dot ('.').
     */
    static
    bool isSo(const QString & extensionName) noexcept
    {
      assert( !extensionName.startsWith( QChar::fromLatin1('.') ) );

      return ( QString::compare( extensionName, QLatin1String("so"), Qt::CaseSensitive ) == 0 );
    }

    /*! \brief Check if this extension is a dylib (shared object)
     */
    bool isDylib() const noexcept
    {
      return isDylib(mName);
    }

    /*! \brief Check if \a extensionName is a dylib (shared object)
     *
     * \pre \a extensionName must not begin with a dot ('.').
     */
    static
    bool isDylib(const QString & extensionName) noexcept
    {
      assert( !extensionName.startsWith( QChar::fromLatin1('.') ) );

      return ( QString::compare( extensionName, QLatin1String("dylib"), Qt::CaseSensitive ) == 0 );
    }

    /*! \brief Check if this extension is a dll (shared object)
     */
    bool isDll() const noexcept
    {
      return isDll(mName);
    }

    /*! \brief Check if \a extensionName is a dll (shared object)
     *
     * \pre \a extensionName must not begin with a dot ('.').
     */
    static
    bool isDll(const QString & extensionName) noexcept
    {
      assert( !extensionName.startsWith( QChar::fromLatin1('.') ) );

      return ( QString::compare( extensionName, QLatin1String("dll"), Qt::CaseInsensitive ) == 0 );
    }

    /*! \brief Check if \a extensionName refers to a shared library
     *
     * Here are the recognized shared library extensions:
     * - so : Linux shared library
     * - dylib : OS-X dynamic library
     * - dll (case insensitive): Windows dynamic-link library
     *
     * \pre \a extensionName must not begin with a dot ('.').
     */
    static
    bool isSharedLibraryExtension(const QString & extensionName) noexcept
    {
      assert( !extensionName.startsWith( QChar::fromLatin1('.') ) );

      if( isSo(extensionName) ){
        return true;
      }
      if( isDylib(extensionName) ){
        return true;
      }
      if( isDll(extensionName) ){
        return true;
      }

      return false;
    }

    /*! \brief Get a shared library extension from \a extensionName
     *
     * \pre \a extensionName must not begin with a dot ('.')
     * \pre \a extensionName must be a shared library extension
     * \sa isSharedLibraryExtension()
     */
    static
    LibraryNameExtension fromSharedLibraryExtensionName(const QString & extensionName) noexcept
    {
      assert( !extensionName.startsWith( QChar::fromLatin1('.') ) );
      assert( isSharedLibraryExtension(extensionName) );

      return LibraryNameExtension(extensionName);
    }

   private:

    explicit LibraryNameExtension(const QString & extensionName) noexcept
     : mName(extensionName)
    {
    }

    QString mName;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_NAME_EXTENSION_H
