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
#ifndef MDT_DEPLOY_UTILS_IMPL_NOT_NULL_TERMINATED_STRING_ERROR_H
#define MDT_DEPLOY_UTILS_IMPL_NOT_NULL_TERMINATED_STRING_ERROR_H

#include "Mdt/DeployUtils/QRuntimeError.h"
#include "mdt_deployutils_export.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   */
  class MDT_DEPLOYUTILS_EXPORT NotNullTerminatedStringError : public QRuntimeError
  {
   public:

    /*! \brief Constructor
     */
    explicit NotNullTerminatedStringError(const QString & what)
      : QRuntimeError(what)
    {
    }

  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_NOT_NULL_TERMINATED_STRING_ERROR_H
