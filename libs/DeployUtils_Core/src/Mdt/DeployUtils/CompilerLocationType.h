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
#ifndef MDT_DEPLOY_UTILS_COMPILER_LOCATION_TYPE_H
#define MDT_DEPLOY_UTILS_COMPILER_LOCATION_TYPE_H

namespace Mdt{ namespace DeployUtils{

  /*! \brief Type of compiler location
   *
   * \todo Should VcInstallDir be removed ?
   */
  enum class CompilerLocationType
  {
    Undefined,    /*!< Compiler will not be located */
    FromEnv,      /*!< Use a environment variable name */
    VcInstallDir, /*!<  */
    CompilerPath  /*!< Use the full path to the compiler. This can be used with CMAKE_CXX_COMPILER */
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_COMPILER_LOCATION_TYPE_H
