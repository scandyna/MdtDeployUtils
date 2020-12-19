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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_FILE_WRITE_ERROR_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_FILE_WRITE_ERROR_H

#include "mdt_commandlineparser_export.h"
#include <stdexcept>
#include <string>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Error thrown by BashCompletionGenerator
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionScriptFileWriteError : public std::runtime_error
  {
   public:

    /*! \brief Constructor
     */
    explicit BashCompletionScriptFileWriteError(const std::string & what)
      : runtime_error(what)
    {
    }

  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_FILE_WRITE_ERROR_H
