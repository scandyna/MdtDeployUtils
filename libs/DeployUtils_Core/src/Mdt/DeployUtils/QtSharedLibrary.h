/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_QT_SHARED_LIBRARY_H
#define MDT_DEPLOY_UTILS_QT_SHARED_LIBRARY_H

#include "QtSharedLibraryFile.h"
#include "QtSharedLibraryError.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper class for Qt shared libraries
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtSharedLibrary : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit QtSharedLibrary(QObject *parent = nullptr) noexcept
     : QObject(parent)
    {
    }

    /*! \brief Get the Qt shared libraries out from \a sharedLibraries
     *
     * \pre each library in \a sharedLibraries must be a absolute path
     */
    static
    QtSharedLibraryFileList getQtSharedLibraries(const QStringList & sharedLibraries) noexcept;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_SHARED_LIBRARY_H
