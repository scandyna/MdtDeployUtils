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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_POSITIONAL_ARGUMENT_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_POSITIONAL_ARGUMENT_H

#include "mdt_commandlineparser_export.h"
#include <QString>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Represents a positional argument for a ParserDefinition or a ParserDefinitionCommand
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserDefinitionPositionalArgument
  {
   public:

    ParserDefinitionPositionalArgument() = delete;

    /*! \brief Construct a argument with a name, a optional description and a optional syntax
     *
     * \pre \a name must not be empty
     *
     * \todo enforce those precontidions
     */
    ParserDefinitionPositionalArgument(const QString & name, const QString & description = QString(), const QString & syntax = QString())
    {
    }

    /*! \brief Copy construct a argument from \a other
     */
    ParserDefinitionPositionalArgument(const ParserDefinitionPositionalArgument & other) = default;

    /*! \brief Copy assign \a other to this argument
     */
    ParserDefinitionPositionalArgument & operator=(const ParserDefinitionPositionalArgument & other) = default;

    /*! \brief Move construct a argument from \a other
     */
    ParserDefinitionPositionalArgument(ParserDefinitionPositionalArgument && other) = default;

    /*! \brief Move assign \a other to this argument
     */
    ParserDefinitionPositionalArgument & operator=(ParserDefinitionPositionalArgument && other) = default;

  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_POSITIONAL_ARGUMENT_H
