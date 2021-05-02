/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ***********************************************************************************************/
#ifndef MDT_DEPLOY_UTILS_QRUNTIME_ERROR_H
#define MDT_DEPLOY_UTILS_QRUNTIME_ERROR_H

#include "mdt_deployutils_export.h"
#include <QString>
#include <QByteArray>
#include <stdexcept>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Base class for runtime exceptions using QString messages
   */
  class MDT_DEPLOYUTILS_EXPORT QRuntimeError : public std::runtime_error
  {
   public:

    /*! \brief Constructor
     */
    explicit QRuntimeError(const QString & what)
     : runtime_error( what.toLocal8Bit().toStdString() )
    {
    }
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QRUNTIME_ERROR_H
