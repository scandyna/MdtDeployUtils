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
#ifndef MDT_DEPLOY_UTILS_FILE_OPEN_ERROR_H
#define MDT_DEPLOY_UTILS_FILE_OPEN_ERROR_H

#include "QRuntimeError.h"
#include "mdt_deployutils_export.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Error when open a file fails
   */
  class MDT_DEPLOYUTILS_EXPORT FileOpenError : public QRuntimeError
  {
   public:

    /*! \brief Constructor
     */
    explicit FileOpenError(const QString & what)
      : QRuntimeError(what)
    {
    }

  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_FILE_OPEN_ERROR_H
