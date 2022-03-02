/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_NAME_IMPL_H
#define MDT_DEPLOY_UTILS_IMPL_LIBRARY_NAME_IMPL_H

#include "LibraryNameData.h"
#include "../LibraryNameExtension.h"
#include "../LibraryVersion.h"
#include <QChar>
#include <QString>
#include <QStringRef>
#include <QLatin1String>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <cassert>
#include <utility>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   */
  struct LibraryNameImpl{

    static
    QString extractPrefix(const QString & fullName) noexcept
    {
      if( fullName.startsWith( QLatin1String("lib"), Qt::CaseSensitive ) ){
        return QLatin1String("lib");
      }

      return QString();
    }

    /*! \internal
     *
     * \pre \a fullExtension must reference a string
     * \pre \a fullExtension must not reference a empty string
     * \pre \a fullExtension must begin with a dot ('.')
     * \pre \a fullExtension must be a shared library extension
     */
    static
    std::pair<LibraryNameExtension, LibraryVersion> extractSharedLibraryExtensionAndVersion(const QStringRef & fullExtension) noexcept
    {
      assert( !fullExtension.isNull() );
      assert( !fullExtension.isEmpty() );
      assert( fullExtension.startsWith( QChar::fromLatin1('.') ) );

      std::pair<LibraryNameExtension, LibraryVersion> extensionAndVersion;

      /*
       * OS-X .dylib
       * (Had to find a compare that accepts a QLatin1String as second argument)
       */
      if( QStringRef::compare( fullExtension.right(5), QLatin1String("dylib"), Qt::CaseSensitive ) == 0 ){
        extensionAndVersion.first = LibraryNameExtension::fromSharedLibraryExtensionName( QLatin1String("dylib") );
        /*
         * fullExtension could be:
         * .dylib
         * .1.dylib
         * .1.234.5.dylib
         */
        const int versionStartIndex = 1;
        const int versionLen = fullExtension.size() - 7;
        if( versionLen > 0 ){
          extensionAndVersion.second = LibraryVersion( fullExtension.mid(versionStartIndex, versionLen).toString() );
        }
        return extensionAndVersion;
      }

      /*
       * UNIX .so
       */
      if( QStringRef::compare( fullExtension.left(3), QLatin1String(".so"), Qt::CaseSensitive ) == 0 ){
        extensionAndVersion.first = LibraryNameExtension::fromSharedLibraryExtensionName( QLatin1String("so") );
        /*
         * fullExtension could be:
         * .so
         * .so.1
         * .so.1.234.5
         */
        const int versionStartIndex = 4;
        const int versionLen = fullExtension.size() - 4;
        if( versionLen > 0 ){
          extensionAndVersion.second = LibraryVersion( fullExtension.mid(versionStartIndex, versionLen).toString() );
        }
        return extensionAndVersion;
      }

      /*
       * Non versionned shared libraries
       */
      extensionAndVersion.first = LibraryNameExtension::fromSharedLibraryExtensionName( fullExtension.mid(1).toString() );

      return extensionAndVersion;
    }

    /*! \internal
     *
     * \pre \a fullName must contain \a prefix and \a fullExtension
     * For example, libQt5Core.so
     * - prefix: lib
     * - fullName: libQt5Core.so
     * - fullExtension: .so
     * Also ok example, Qt5Core:
     * - prefix: empty
     * - fullName: Qt5Core
     * - fullExtension: null
     */
    static
    QString extractName(const QString & prefix, const QString & fullName, const QStringRef & fullExtension) noexcept
    {
      const int start = prefix.size();

      int fullExtensionSize = 0;
      if( !fullExtension.isNull() ){
        fullExtensionSize = fullExtension.size();
      }

      const int count = fullName.size() - start - fullExtensionSize;
      assert( count >= 0 );

      return fullName.mid(start, count);
    }

    static
    LibraryNameData fromFullName(const QString & fullName) noexcept
    {
      LibraryNameData data;

      data.fullName = fullName;
      data.prefix = extractPrefix(fullName);

      static const char *extensionRegEx = "(\\.so(\\.[0-9]{1,4}){0,3}|(\\.[0-9]{1,4}){0,3}\\.dylib$|.dll)";
      QRegularExpression re;
      re.setPattern( QLatin1String(extensionRegEx) );
      re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
      const QRegularExpressionMatch extensionRegexMatch = re.match(fullName);

      if( !extensionRegexMatch.hasMatch() ){
        data.name = extractName( data.prefix, fullName, QStringRef() );
        return data;
      }

      const QStringRef fullExtension = extensionRegexMatch.capturedRef();

      data.name = extractName(data.prefix, fullName, fullExtension);
      data.extensionAndVersion = extractSharedLibraryExtensionAndVersion(fullExtension);

      return data;
    }

  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_LIBRARY_NAME_IMPL_H
