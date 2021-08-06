/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_CMAKE_STYLE_MESSAGE_LOGGER_H
#define MDT_DEPLOY_UTILS_CMAKE_STYLE_MESSAGE_LOGGER_H

#include "AbstractMessageLoggerBackend.h"
#include "mdt_deployutilscore_export.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief Console message logger backend that imitates how CMake prints messages
   *
   * \sa MessageLogger
   * \sa https://cmake.org/cmake/help/latest/command/message.html
   */
  class MDT_DEPLOYUTILSCORE_EXPORT CMakeStyleMessageLogger : public AbstractMessageLoggerBackend
  {
   public:

    /*! \brief Log a information
     */
    void info(const QString & message) override;

    /*! \brief Log a error
     */
    void error(const QString & message) override;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_CMAKE_STYLE_MESSAGE_LOGGER_H
