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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_POSITIONAL_ARGUMENT_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_POSITIONAL_ARGUMENT_H

#include "ParserDefinitionPositionalArgument.h"
#include "BashCompletion/Action.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Represents a positional argument for a BashCompletionGenerator or a BashCompletionGeneratorCommand
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionGeneratorPositionalArgument
  {
   public:

    BashCompletionGeneratorPositionalArgument() = delete;

    /*! \brief Construct a argument with a name
     *
     * \pre \a name must not be empty
     */
    BashCompletionGeneratorPositionalArgument(const QString & name)
     : mName( name.trimmed() )
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Copy construct a argument from \a other
     */
    BashCompletionGeneratorPositionalArgument(const BashCompletionGeneratorPositionalArgument & other) = default;

    /*! \brief Copy assign \a other to this argument
     */
    BashCompletionGeneratorPositionalArgument & operator=(const BashCompletionGeneratorPositionalArgument & other) = default;

    /*! \brief Move construct a argument from \a other
     */
    BashCompletionGeneratorPositionalArgument(BashCompletionGeneratorPositionalArgument && other) noexcept = default;

    /*! \brief Move assign \a other to this argument
     */
    BashCompletionGeneratorPositionalArgument & operator=(BashCompletionGeneratorPositionalArgument && other) noexcept = default;

    /*! \brief Get the name of this argument
     */
    const QString & name() const noexcept
    {
      return mName;
    }

    /*! \brief Set the action to complete the value of this option
     */
    void setAction(const BashCompletion::Action & action) noexcept
    {
      mAction = action;
    }

    /*! \brief Check if this option has a action attached
     */
    bool hasAction() const noexcept
    {
      return mAction.isDefined();
    }

    /*! \brief Access the action of this option
     */
    const BashCompletion::Action & action() const noexcept
    {
      return mAction;
    }

    /*! \brief Get a Bash completion argument from \a argument
     */
    static
    BashCompletionGeneratorPositionalArgument fromParserDefinitionPositionalArgument(const ParserDefinitionPositionalArgument & argument);

   private:

    QString mName;
    BashCompletion::Action mAction;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_POSITIONAL_ARGUMENT_H
