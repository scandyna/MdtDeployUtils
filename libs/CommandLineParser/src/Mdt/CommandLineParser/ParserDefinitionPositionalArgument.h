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
#include <cassert>

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
     */
    ParserDefinitionPositionalArgument(const QString & name, const QString & description, const QString & syntax = QString())
     : mName( name.trimmed() ),
       mDescription( description.trimmed() ),
       mSyntax( syntax.trimmed() )
    {
      assert( !name.trimmed().isEmpty() );
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

    /*! \brief Get the name of this argument
     */
    const QString & name() const noexcept
    {
      return mName;
    }

    /*! \brief Get the description of this argument
     */
    const QString & description() const noexcept
    {
      return mDescription;
    }

    /*! \brief Check if this argument has a syntax
     */
    bool hasSyntax() const noexcept
    {
      return !mSyntax.isEmpty();
    }

    /*! \brief Get the syntax of this argument
     */
    const QString & syntax() const noexcept
    {
      return mSyntax;
    }

   private:

    QString mName;
    QString mDescription;
    QString mSyntax;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_POSITIONAL_ARGUMENT_H
