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
#ifndef MDT_DEPLOY_UTILS_ABSTRACT_MESSAGE_LOGGER_BACKEND_H
#define MDT_DEPLOY_UTILS_ABSTRACT_MESSAGE_LOGGER_BACKEND_H

#include "mdt_deployutilscore_export.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Interface to implement a specific message logger backend
   *
   * \sa MessageLogger
   */
  class MDT_DEPLOYUTILSCORE_EXPORT AbstractMessageLoggerBackend
  {
   public:

    AbstractMessageLoggerBackend() = default;
    virtual ~AbstractMessageLoggerBackend() noexcept = default;

    AbstractMessageLoggerBackend(const AbstractMessageLoggerBackend &) = delete;
    AbstractMessageLoggerBackend & operator=(const AbstractMessageLoggerBackend &) = delete;
    AbstractMessageLoggerBackend(AbstractMessageLoggerBackend &&) = delete;
    AbstractMessageLoggerBackend & operator=(AbstractMessageLoggerBackend &&) = delete;

    /*! \brief Log a information
     */
    virtual void info(const QString & message) = 0;

    /*! \brief Log a error
     */
    virtual void error(const QString & message) = 0;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ABSTRACT_MESSAGE_LOGGER_BACKEND_H
