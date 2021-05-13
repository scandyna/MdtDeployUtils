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
#ifndef MDT_DEPLOY_UTILS_LIBRARY_VERSION_H
#define MDT_DEPLOY_UTILS_LIBRARY_VERSION_H

#include "mdt_deployutils_export.h"
#include <QString>
#include <QStringRef>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Representation of a version of a library
   */
  class MDT_DEPLOYUTILS_EXPORT LibraryVersion
  {
   public:

    /*! \brief Construct a null version
     */
    LibraryVersion() = default;

    /*! \brief Construct a library version
     *
     * \pre versionMajor must be >= 0
     * \pre if versionPatch is >=0 , versionMinor must also be >= 0
     */
    LibraryVersion(int versionMajor, int versionMinor = -1, int versionPatch = -1);

    /*! \brief Construct a library version from a version string
     *
     * If \a version does not represent a valid version string,
     *  this will be a null library version.
     *
     * \sa isNull()
     */
    LibraryVersion(const QString & version)
     : LibraryVersion(QStringRef(&version))
    {
    }

    /*! \brief Construct a library version from a version string
     *
     * \sa LibraryVersion(const QString &)
     */
    LibraryVersion(const QStringRef & version);

    /*! \brief Copy construct a version from a other
     */
    LibraryVersion(const LibraryVersion &) = default;

    /*! \brief Copy assign to this version from a other
     */
    LibraryVersion & operator=(const LibraryVersion &) = default;

    /*! \brief Move construct a version from a other
     */
    LibraryVersion(LibraryVersion &&) = default;

    /*! \brief Move assign to this version from a other
     */
    LibraryVersion & operator=(LibraryVersion &&) = default;

    /*! \brief Check is this library version is null
     */
    bool isNull() const
    {
      return (mVersionMajor < 0);
    }

    /*! \brief Get version major
     */
    int versionMajor() const
    {
      return mVersionMajor;
    }

    /*! \brief Get version minor
     */
    int versionMinor() const
    {
      return mVersionMinor;
    }

    /*! \brief Get version patch
     */
    int versionPatch() const
    {
      return mVersionPatch;
    }

    /*! \brief Get a string representation of this library version
     */
    QString toString() const;

   private:

    int mVersionMajor = -1;
    int mVersionMinor = -1;
    int mVersionPatch = -1;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LIBRARY_VERSION_H
