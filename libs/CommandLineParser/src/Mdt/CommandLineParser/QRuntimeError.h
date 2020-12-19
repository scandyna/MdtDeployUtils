/****************************************************************************
 **
 ** Copyright (C) 2011-2020 Philippe Steinmann.
 **
 ** This file is part of MdtApplication library.
 **
 ** MdtApplication is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** MdtApplication is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with MdtApplication.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_COMMAND_LINE_PARSER_QRUNTIME_ERROR_H
#define MDT_COMMAND_LINE_PARSER_QRUNTIME_ERROR_H

#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QByteArray>
#include <stdexcept>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Base class for runtime exceptions using QString messages
   */
  class MDT_COMMANDLINEPARSER_EXPORT QRuntimeError : public std::runtime_error
  {
   public:

    /*! \brief Constructor
     */
    explicit QRuntimeError(const QString & what)
     : runtime_error( what.toLocal8Bit().toStdString() )
    {
    }
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_QRUNTIME_ERROR_H
