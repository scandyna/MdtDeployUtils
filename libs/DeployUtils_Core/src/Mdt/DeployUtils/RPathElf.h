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
#ifndef MDT_DEPLOY_UTILS_RPATH_ELF_H
#define MDT_DEPLOY_UTILS_RPATH_ELF_H

#include "RPath.h"
#include "RPathFormatError.h"
#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QObject>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper class to convert RPath from and to ELF RPATH string
   *
   * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
   */
  class MDT_DEPLOYUTILSCORE_EXPORT RPathElf : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Get a rpath entry from \a path
     *
     * \pre \a path must not be empty
     * \exception RPathFormatError
     */
    static
    RPathEntry rPathEntryFromString(const QString & path);

    /*! \brief Get a rpath from \a rpathString
     *
     * \exception RPathFormatError
     */
    static
    RPath rPathFromString(const QString & rpathString);

    /*! \brief Get the string version of \a rpathEntry
     */
    static
    QString rPathEntryToString(const RPathEntry & rpathEntry) noexcept;

    /*! \brief Get the string version of \a rpath
     */
    static
    QString rPathToString(const RPath & rpath) noexcept;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_RPATH_ELF_H
