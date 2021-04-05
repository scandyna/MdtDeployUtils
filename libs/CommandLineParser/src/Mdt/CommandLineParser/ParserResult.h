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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_H
#define MDT_COMMAND_LINE_PARSER_PARSER_RESULT_H

#include "ParserResultCommand.h"
#include "ParserDefinitionOption.h"
#include "ParserDefinitionCommand.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Result from a Parser
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserResult
  {
   public:

    /*! \brief Set the main command result
     */
    void setMainCommand(const ParserResultCommand & command)
    {
      mMainCommand = command;
    }

    /*! \brief Get the main command result
     */
    const ParserResultCommand & mainCommand() const noexcept
    {
      return mMainCommand;
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
     * Will return true if \a option was set on the command line,
     * either by its long name or by its short name if \a option has a short name.
     *
     * \note If you use subcommands,
     * also query if a option is set for the expected subcommand.
     */
    bool isSet(const ParserDefinitionOption & option) const noexcept
    {
      return mMainCommand.isSet(option);
    }

    /*! \brief Check if \a optionLongName is set in this result
     *
     * Will return true if the long option was set on the command line,
     * otherwise false.
     *
     * \sa isSet()
     *
     * \note If you use subcommands,
     * also query if a option is set for the expected subcommand.
     */
    bool isOptionLongNameSet(const QString & optionLongName) const noexcept
    {
      return mMainCommand.isOptionLongNameSet(optionLongName);
    }

    /*! \brief Check if \a optionShortName is set in this result
     *
     * Will return true if the short option was set on the command line,
     * otherwise false.
     *
     * \sa isSet()
     */
    bool isOptionShortNameSet(char optionShortName) const noexcept
    {
      return mMainCommand.isOptionShortNameSet(optionShortName);
    }

    /*! \brief Check if the help option is set in this result
     *
     * \note If you use subcommands,
     * also query if the help option is set for the expected subcommand.
     */
    bool isHelpOptionSet() const noexcept
    {
      return mMainCommand.isHelpOptionSet();
    }

    /*! \brief Returns a list of option values found for the given \a option
     *
     * For options found by the parser, the list will contain an entry for each time the option was encountered by the parser.
     *
     * If the option wasn't specified on the command line, the default values are returned.
     *
     * \note If you use subcommands,
     * also query about values for the expected subcommand.
     */
    QStringList getValues(const ParserDefinitionOption & option) const noexcept
    {
      return mMainCommand.getValues(option);
    }

    /*! \brief Returns a list of option values found for the given \a optionLongName
     *
     * Will return a list of values for each option named \a optionLongName
     * present in this result command.
     *
     * Note that the list will not contain values
     * for the option given by its short name in this result command.
     *
     * \sa getValues()
     *
     * \note If you use subcommands,
     * also query about values for the expected subcommand.
     */
    QStringList getOptionLongNameValues(const QString & optionLongName) const noexcept
    {
      return mMainCommand.getOptionLongNameValues(optionLongName);
    }

    /*! \brief Returns a list of option values found for the given \a optionShortName
     *
     * Will return a list of values for each option named \a optionShortName
     * present in this result command.
     *
     * Note that the list will not contain values
     * for the option given by its long name in this result command.
     *
     * \sa getValues()
     *
     * \note If you use subcommands,
     * also query about values for the expected subcommand.
     */
    QStringList getOptionShortNameValues(char optionShortName) const noexcept
    {
      return mMainCommand.getOptionShortNameValues(optionShortName);
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
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_H
