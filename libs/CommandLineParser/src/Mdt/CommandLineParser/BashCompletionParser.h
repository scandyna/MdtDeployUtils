/****************************************************************************
 **
 ** Copyright (C) 2011-2021 Philippe Steinmann.
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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_H

#include "ParserDefinition.h"
#include "CommandLine/CommandLine.h"
#include "mdt_commandlineparser_export.h"

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Handles the Bash completion
   *
   * If the first positional argument in \a commandLine is a request from the Bash completion script,
   * it will be handled.
   *
   * Returns true if a Bash completion has been handled, otherwise false.
   *
   * Example:
   * \code
   * using namespace Mdt::CommandLineParser;
   *
   * Parser parser;
   *
   * parser.parse(parserDefinition, arguments);
   * // Error handling omitted here
   *
   * if( handleBashCompletion(parser.commandLine(), parserDefinition) ){
   *   exit(0);
   * }
   *
   * if( parser.hasError() ){
   *   // Error handling here
   * }
   * \endcode
   */
  MDT_COMMANDLINEPARSER_EXPORT
  bool handleBashCompletion(const CommandLine::CommandLine & commandLine, const ParserDefinition & parserDefinition);

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_H
