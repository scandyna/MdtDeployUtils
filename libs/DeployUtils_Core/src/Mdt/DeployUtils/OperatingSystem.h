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
#ifndef MDT_DEPLOY_UTILS_OPERATING_SYSTEM_H
#define MDT_DEPLOY_UTILS_OPERATING_SYSTEM_H

#include <QMetaType>

namespace Mdt{ namespace DeployUtils{

  /*! \brief OS enum
   */
  enum class OperatingSystem
  {
    Unknown,  /*!< Unknown OS */
    Linux,    /*!< Linux OS */
    Windows   /*!< Windows OS */
  };

}} // namespace Mdt{ namespace DeployUtils{
Q_DECLARE_METATYPE(Mdt::DeployUtils::OperatingSystem)

#endif // #ifndef MDT_DEPLOY_UTILS_OPERATING_SYSTEM_H
