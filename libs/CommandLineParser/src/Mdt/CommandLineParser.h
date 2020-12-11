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
#ifndef MDT_COMMAND_LINE_PARSER_H
#define MDT_COMMAND_LINE_PARSER_H

#include "mdt_commandlineparser_export.h"

namespace Mdt{

  /*! \brief Helper class to create command line parsers
   *
   * To parse command line arguments, QCommandLineParser is a helpful tool.
   *
   * This library uses QCommandLineParser, but adds some things missing in it.
   *
   * QCommandLineParser has no natural notion of sub-commands.
   * It is possible to create such sub-commands using a QCommandLineParser for each one.
   * A drawback is that the QCommandLineParser::helpText() will return the application name,
   * but the sub-command name is expected.
   *
   * Once QCommandLineParser has been set-up, the defined arguments and options cannot be get:
   * \code
   * QCommandLineParser parser;
   * parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
   * parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));
   *
   * qDebug() << parser.positionalArguments();
   * \endcode
   *
   * Above code will not work, because we did not call %parse() or %process().
   * The returned arguments are those that are passed to the command line
   * and that are not recognized as options,
   * which is expected for parsing.
   *
   * For some cases, getting the arguments and options given can be useful,
   * for exaple to list available sub-commands in the help,
   * or to generate a Bash-completion script.
   */
  class MDT_COMMANDLINEPARSER_EXPORT CommandLineParser
  {
  };

} // namespace Mdt{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_H
