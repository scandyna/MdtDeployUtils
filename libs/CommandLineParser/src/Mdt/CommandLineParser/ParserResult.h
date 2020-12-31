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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_H
#define MDT_COMMAND_LINE_PARSER_PARSER_RESULT_H

#include "ParserResultCommand.h"
#include "ParserDefinitionOption.h"
#include "ParserDefinitionCommand.h"
#include "mdt_commandlineparser_export.h"
#include <QString>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Result from a Parser
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserResult
  {
   public:

    /*! \brief Set the error text to this result
     */
    void setErrorText(const QString & text)
    {
      mErrorText = text.trimmed();
    }

    /*! \brief Check if this result has error
     */
    bool hasError() const noexcept
    {
      return !mErrorText.isEmpty();
    }

    /*! \brief Get the error text of this result
     */
    const QString & errorText() const noexcept
    {
      return mErrorText;
    }

    /*! \brief Set the main command result
     */
    void setMainCommand(const ParserResultCommand & command)
    {
      mMainCommand = command;
    }

    /*! \brief Check if this result has any option
     *
     * \note If you use subcommands,
     * also query if the expected subcommand has any option.
     */
    bool hasOptions() const noexcept
    {
      return mMainCommand.hasOptions();
    }

    /*! \brief Check if \a option is set in this result
     *
     * \note If you use subcommands,
     * also query if a option is set for the expected subcommand.
     */
    bool isSet(const ParserDefinitionOption & option) const
    {
      return mMainCommand.isSet(option);
    }

    /*! \brief Check if \a optionName is set in this result
     *
     * \note If you use subcommands,
     * also query if a option is set for the expected subcommand.
     */
    bool isSet(const QString & optionName) const
    {
      return mMainCommand.isSet(optionName);
    }

//     /*! \brief Check if \a optionShortName is set in this result
//      *
//      * \note If you use subcommands,
//      * also query if a option is set for the expected subcommand.
//      */
//     bool isSet(char optionShortName) const
//     {
//       return mMainCommand.isSet(optionShortName);
//     }

    /*! \brief Check if the help option is set in this result
     *
     * \note If you use subcommands,
     * also query if the help option is set for the expected subcommand.
     */
    bool isHelpOptionSet() const
    {
      return mMainCommand.isHelpOptionSet();
    }

    /*! \brief Check if this result has any positional argument
     *
     * \note If you use subcommands,
     * also query if the expected subcommand has any positional arguments.
     */
    bool hasPositionalArguments() const noexcept
    {
      return mMainCommand.hasPositionalArguments();
    }

    /*! \brief Get the count of arguments of this result
     *
     * \note If you use subcommands,
     *  also query the positional arguments for the expected subcommand.
     */
    int positionalArgumentCount() const noexcept
    {
      return mMainCommand.positionalArgumentCount();
    }

    /*! \brief Check if this command has a positional argument at \a index
     *
     * \pre \a index must be >= 0
     * \note If you use subcommands,
     *  also query the positional arguments for the expected subcommand.
     */
    bool hasPositionalArgumentAt(int index) const noexcept
    {
      assert( index >= 0 );

      return mMainCommand.hasPositionalArgumentAt(index);
    }

    /*! \brief Get the positional argument at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < argumentCount() )
     * \note If you use subcommands,
     *  also query the positional arguments for the expected subcommand.
     */
    const QString & positionalArgumentAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < positionalArgumentCount() );

      return mMainCommand.positionalArgumentAt(index);
    }

    /*! \brief Set the subcommand to this result
     */
    void setSubCommand(const ParserResultCommand & command)
    {
      mSubCommand = command;
    }

    /*! \brief Check if this result has a subcommand
     */
    bool hasSubCommand() const noexcept
    {
      return mSubCommand.hasName();
    }

    /*! \brief Get the subcommand of this result
     */
    const ParserResultCommand & subCommand() const noexcept
    {
      return mSubCommand;
    }

   private:

    ParserResultCommand mMainCommand;
    ParserResultCommand mSubCommand;
    QString mErrorText;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_H
