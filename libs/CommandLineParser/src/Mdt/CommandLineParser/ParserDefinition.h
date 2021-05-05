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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_H

#include "ParserDefinitionCommand.h"
#include "ParserDefinitionOption.h"
#include "mdt_commandlineparser_export.h"
#include <QCoreApplication>
#include <QString>
#include <boost/optional.hpp>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cstddef>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Helper class to define command line parsers
   *
   * Example of a parser definition:
   * \code
   * using namespace Mdt::CommandLineParser;
   *
   * ParserDefinition parserDefinition;
   * parserDefinition.setApplicationDescription("Copy utility");
   * parserDefinition.addHelpOption();
   * parserDefinition.addPositionalArgument(ArgumentType::File, "source", "Source file to copy");
   * parserDefinition.addPositionalArgument(ArgumentType::Directory, "destination", "Destination directory");
   * parserDefinition.addOption('f', "force", "Overwrite existing files.");
   * \endcode
   *
   * Example of a parser definition that has subcommands:
   * \code
   * using namespace Mdt::CommandLineParser;
   *
   * ParserDefinition parserDefinition;
   * parserDefinition.setApplicationDescription("C++ package manager");
   * parserDefinition.addHelpOption();
   *
   * ParserDefinitionCommand installCommand("install");
   * installCommand.setDescription("Installs the requirements specified in a recipe");
   * installCommand.addPositionalArgument("reference", "Reference of the for user/channel");
   * installCommand.addHelpOption();
   *
   * ParserDefinitionOption installCommandBuildOption('b', "build");
   * installCommandBuildOption.setValueName("BUILD");
   * installCommandBuildOption.setPossibleValues({"all","missing","never"});
   * installCommandBuildOption.setDefaultValue("never");
   * installCommandBuildOption.setDescription("Choose if you want to build from sources.");
   * installCommand.addOption(installCommandBuildOption);
   *
   * parserDefinition.addSubCommand(installCommand);
   * \endcode
   *
   * \todo Add version option
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserDefinition
  {
    Q_DECLARE_TR_FUNCTIONS(ParserDefinition)

   public:

    /*! \brief Set the application name
     *
     * Setting the application name is not mandatory,
     * but can be useful if the default is not what is expected
     *
     * \sa applicationName()
     */
    void setApplicationName(const QString & name);

    /*! \brief Get the application name
     *
     * If the application name has been set with setApplicationName(),
     * this name will be returned,
     * otherwise QCoreApplication::applicationName()
     */
    QString applicationName() const
    {
      if( !mApplicationName.isEmpty() ){
        return mApplicationName;
      }
      return QCoreApplication::applicationName();
    }

    /*! \brief Set the application description
     */
    void setApplicationDescription(const QString &description);

    /*! \brief Check if this parser has a application description
     */
    bool hasApplicationDescription() const noexcept
    {
      return !mApplicationDescription.isEmpty();
    }

    /*! \brief Get the application description
     */
    const QString & applicationDescription() const noexcept
    {
      return mApplicationDescription;
    }

    /*! \brief Add a option
     */
    void addOption(const ParserDefinitionOption & option);

    /*! \brief Add a option
     *
     * \pre \a name must be a valid option name
     * \sa ParserDefinitionOption::isValidName()
     */
    void addOption(const QString & name, const QString & description);

    /*! \brief Add a option
     *
     * \pre \a shortName must be a valid option short name
     * \pre \a name must be a valid option name
     * \sa ParserDefinitionOption::isValidShortName()
     * \sa ParserDefinitionOption::isValidName()
     */
    void addOption(char shortName, const QString & name, const QString & description);

    /*! \brief Add the help option
     */
    void addHelpOption();

    /*! \brief Check if this parser has options
     */
    bool hasOptions() const noexcept
    {
      return mMainCommand.hasOptions();
    }

    /*! \brief Get the count of options of this parser definition
     */
    int optionCount() const noexcept
    {
      return mMainCommand.optionCount();
    }

    /*! \brief Get the option at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < optionCount() )
     */
    const ParserDefinitionOption & optionAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < optionCount() );

      return mMainCommand.optionAt(index);
    }

    /*! \brief Add a positional argument
     *
     * \pre \a name must not be empty
     */
    void addPositionalArgument(const QString & name, const QString & description, const QString & syntax = QString());

    /*! \brief Add a positional argument
     *
     * \pre \a name must not be empty
     */
    void addPositionalArgument(ValueType type, const QString & name, const QString & description, const QString & syntax = QString());

    /*! \brief Check if this parser has positional arguments
     */
    bool hasPositionalArguments() const noexcept
    {
      return mMainCommand.hasPositionalArguments();
    }

    /*! \brief Get the main command
     */
    const ParserDefinitionCommand & mainCommand() const noexcept
    {
      return mMainCommand;
    }

    /*! \brief Add a subcommand
     *
     * \pre \a command must have a name
     */
    void addSubCommand(const ParserDefinitionCommand & command);

    /*! \brief Check if this parser has at least one subcommand
     */
    bool hasSubCommands() const noexcept
    {
      return !mSubCommands.empty();
    }

    /*! \brief Get the count of sub-commands
     */
    int subCommandCount() const noexcept
    {
      return static_cast<int>( mSubCommands.size() );
    }

    /*! \brief Get the index of the sub-command named \a name
     *
     * Returns the index of the sub-command named \a name ,
     * or a value < 0 if not found.
     */
    int findSubCommandIndexByName(const QString & name) const noexcept
    {
      const auto pred = [&name](const ParserDefinitionCommand & command){
        return command.name() == name;
      };

      const auto it = std::find_if(mSubCommands.cbegin(), mSubCommands.cend(), pred);
      if( it == mSubCommands.cend() ){
        return -1;
      }

      return static_cast<int>( std::distance( mSubCommands.cbegin(), it ) );
    }

    /*! \brief Find a sub-command by \a name
     */
    boost::optional<const ParserDefinitionCommand &> findSubCommandByName(const QString & name) const noexcept
    {
      const int index = findSubCommandIndexByName(name);

      if(index < 0){
        return boost::none;
      }

      return subCommandAt(index);
    }

    /*! \brief Check if this definition contains a sub-command named \a subCommandName
     */
    bool containsSubCommand(const QString & subCommandName) const noexcept
    {
      return findSubCommandIndexByName(subCommandName) >= 0;
    }

    /*! \brief Access the sub-command at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < subCommandCount() )
     */
    const ParserDefinitionCommand & subCommandAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < subCommandCount() );

      return mSubCommands[static_cast<size_t>(index)];
    }

    /*! \brief Get subcommands
     */
    const std::vector<ParserDefinitionCommand> & subCommands() const noexcept
    {
      return mSubCommands;
    }

    /*! \brief Get the help text
     */
    QString getHelpText() const;

    /*! \brief Get the help text for the subcommand named \a subCommandName
     */
    QString getSubCommandHelpText(const QString & subCommandName) const
    {
      const auto command = findSubCommandByName(subCommandName);
      if(command){
        return command->getHelpText(mApplicationName);
      }

      return QString();
    }

   private:

    bool includeOptionsInHelpText() const noexcept
    {
      return true;
    }

    QString getUsageText() const;
    QString getOptionsHelpText() const;
    QString getPositionalArgumentsHelpText() const;
    QString getAvailableSubCommandsHelpText() const;

    QString mApplicationName;
    QString mApplicationDescription;
    ParserDefinitionCommand mMainCommand;
    std::vector<ParserDefinitionCommand> mSubCommands;
    int mHelpTextLineMaxLength = 80;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_H
