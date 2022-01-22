/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_LOG_LEVEL_H
#define MDT_DEPLOY_UTILS_LOG_LEVEL_H

#include "mdt_deployutilscore_export.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief Log level
   *
   * The log level is inspired by CMake.
   *
   * \sa https://cmake.org/cmake/help/latest/command/message.html
   * \sa https://cmake.org/cmake/help/latest/manual/cmake.1.html
   * \sa https://cmake.org/cmake/help/latest/variable/CMAKE_MESSAGE_LOG_LEVEL.html#variable:CMAKE_MESSAGE_LOG_LEVEL
   */
  enum class LogLevel
  {
    Error,    /*!< Only output errors */
    Status,   /*!< Output messages of STATUS level or higher */
    Verbose,  /*!< Output messages of VERBOSE level or higher */
    Debug     /*!< Output messages of DEBUG level or higher */
  };

  /*! \brief Check if status messages should be outputted given \a level
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  bool shouldOutputStatusMessages(LogLevel level) noexcept;

  /*! \brief Check if verbose messages should be outputted given \a level
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  bool shouldOutputVerboseMessages(LogLevel level) noexcept;

  /*! \brief Check if debug messages should be outputted given \a level
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  bool shouldOutputDebugMessages(LogLevel level) noexcept;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_LOG_LEVEL_H
