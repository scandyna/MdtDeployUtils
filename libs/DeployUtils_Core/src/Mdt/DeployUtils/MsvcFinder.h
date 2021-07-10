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
#ifndef MDT_DEPLOY_UTILS_MSVC_FINDER_H
#define MDT_DEPLOY_UTILS_MSVC_FINDER_H

#include "FindMsvcError.h"
#include "MsvcVersion.h"
#include "mdt_deployutils_export.h"
#include <QObject>
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper to find MSVC installation
   */
  class MDT_DEPLOYUTILS_EXPORT MsvcFinder : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit MsvcFinder(QObject* parent = nullptr);

    /*! \brief Find the root of MSVC for given \a version
     *
     * For example, for Visual Studio 2017,
     * the result could be .......
     *
     * \pre \a version must be valid
     */
    QString findMsvcRoot(const MsvcVersion & version);
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_MSVC_FINDER_H
