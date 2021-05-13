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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_NAME_H
#define MDT_DEPLOY_UTILS_LIBRARY_NAME_H

#include "LibraryVersion.h"
#include "LibraryNameExtension.h"
#include "Impl/LibraryNameData.h"
#include "mdt_deployutils_export.h"
#include <QChar>
#include <QLatin1Char>
#include <QString>
#include <QStringRef>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Representation of a shared library name
   *
   * A shared library can be composed of several parts:
   * - A basename (ex: Qt5Core)
   * - A prefix (ex: lib)
   * - A version
   * - A extension (so, dll, dylib)
   *
   * On Windows, the full name will be composed with the basename with the dll extension.
   * Example: Qt5Core.dll .
   *
   * On Linux, the full name will commonly be composed, at least,
   * with the \a lib prefix, the basename and the so extension.
   * Example: libQt5Core.so .
   *
   * On Apple OS-X, the full name will commonly be composed, at least,
   * with the \a lib prefix, the basename and the dylib extension.
   * Example: libQt5Core.dylib .
   *
   * Here is a example for Qt5Core shared library:
   * |   OS    | Full name        | Versionned full name   |
   * |:-------:|:-----------------|:-----------------------|
   * | Linux   | libQt5Core.so    | libQt5Core.so.5.9.3    |
   * | OS-X    | libQt5Core.dylib | libQt5Core.5.9.3.dylib |
   * | Windows | Qt5Core.dll      | n/a                    |
   *
   */
  class MDT_DEPLOYUTILS_EXPORT LibraryName
  {
   public:

    /*! \brief Construct a null library name
     */
    LibraryName() = default;

    /*! \brief Construct a library name from a name
     *
     * \a fullName can be passed without any prefix, extension, version.
     * Example: Qt5Core
     * A more platform specific name can also be passed,
     *  for example libQt5Core.so
     * A versionned version can also be passed,
     *  for example libQt5Core.so.5 , libQt5Core.so.5.5 , libQt5Core.so.5.5.1
     */
    LibraryName(const QString & fullName);

    /*! \brief Copy construct a library name from a other
     */
    LibraryName(const LibraryName &) = default;

    /*! \brief Copy assign to this library name from a other
     */
    LibraryName & operator=(const LibraryName &) = default;

    /*! \brief Move construct a library name from a other
     */
    LibraryName(LibraryName &&) = default;

    /*! \brief Move assign to this library name from a other
     */
    LibraryName & operator=(LibraryName &&) = default;

    /*! \brief Check is this library name is null
     */
    bool isNull() const noexcept
    {
      return mData.fullName.isEmpty();
    }

    /*! \brief Get library name prefix
     *
     * Returns the library name prefix if it was set.
     */
    QString prefix() const noexcept
    {
      return mData.prefix;
    }

    /*! \brief Get library name
     *
     * Returns the library name without any prefix or extension or version.
     *  For example, if this library name was constructed with libQt5Core.so.5.5.1
     *  this method returns Qt5Core
     */
    const QString & name() const noexcept
    {
      return mData.name;
    }

    /*! \brief Check a debug suffix is present in the name
     *
     * This has only sense for dll (Windows),
     *  when the convention of suffixing 'd' or 'D'
     *  to the name is used.
     *
     * For example:
     *  - Qt5Core.dll : not debug, this method returns false
     *  - Qt5Cored.dll: debug, this method returns true
     */
    bool hasNameDebugSuffix() const noexcept
    {
      if( name().isEmpty() ){
        return false;
      }
      const QChar nameSuffix = name().at(name().size() - 1);

      return ( (nameSuffix == QLatin1Char('d')) || (nameSuffix == QLatin1Char('D')) );
    }

    /*! \brief Get library name extension
     */
    const LibraryNameExtension & extension() const noexcept
    {
      return mData.extensionAndVersion.first;
    }

    /*! \brief Get library version
     */
    const LibraryVersion & version() const noexcept
    {
      return mData.extensionAndVersion.second;
    }

    /*! \brief Get library full name
     *
     * Returns the library name as it was set.
     */
    QString fullName() const noexcept
    {
      return mData.fullName;
    }

   private:

    Impl::LibraryNameData mData;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_NAME_H
