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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_COMMAND_H

#include "BashCompletionGeneratorOption.h"
#include "BashCompletionGeneratorPositionalArgument.h"
#include "ParserDefinitionCommand.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QStringList>
#include <cassert>
#include <vector>
#include <cstddef>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Command for BashCompletionGenerator
   *
   * A command holds a list of arguments and options
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionGeneratorCommand
  {
   public:

    /*! \brief Construct a command with no name
     *
     * This is useful for the main command
     */
    BashCompletionGeneratorCommand() = default;

    /*! \brief Construct a command
     *
     * \pre \a name must not be empty
     */
    BashCompletionGeneratorCommand(const QString & name)
     : mName(name)
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Copy construct a command from \a other
     */
    BashCompletionGeneratorCommand(const BashCompletionGeneratorCommand & other) = default;

    /*! \brief Copy assign \a other to this command
     */
    BashCompletionGeneratorCommand & operator=(BashCompletionGeneratorCommand && other) = default;

    /*! \brief Move construct a command from \a other
     */
    BashCompletionGeneratorCommand(BashCompletionGeneratorCommand && other) = default;

    /*! \brief Move assign \a other to this command
     */
    BashCompletionGeneratorCommand & operator=(const BashCompletionGeneratorCommand & other) = default;

    /*! \brief Check if this command has a name
     */
    bool hasName() const noexcept
    {
      return !mName.isEmpty();
    }

    /*! \brief Get the name of this command
     */
    const QString & name() const noexcept
    {
      return mName;
    }

    /*! \brief Add a positional argument
     *
     * \pre \a name must not be empty
     */
    void addPositionalArgument(const QString & name)
    {
      assert( !name.trimmed().isEmpty() );

      mArguments.emplace_back(name);
    }

    /*! \brief Add a positional argument
     */
    void addPositionalArgument(const BashCompletionGeneratorPositionalArgument & argument)
    {
      mArguments.push_back(argument);
    }

    /*! \brief Check if this command has positional arguments
     */
    bool hasPositionalArguments() const noexcept
    {
      return !mArguments.empty();
    }

    /*! \brief Get the count of arguments of this command
     */
    int positionalArgumentCount() const noexcept
    {
      return static_cast<int>( mArguments.size() );
    }

    /*! \brief Get the positional argument at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < argumentCount() )
     */
    const BashCompletionGeneratorPositionalArgument & positionalArgumentAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < positionalArgumentCount() );

      return mArguments[static_cast<size_t>(index)];
    }

    /*! \brief Get the list of arguments of this command
     */
    const std::vector<BashCompletionGeneratorPositionalArgument> & positionalArguments() const noexcept
    {
      return mArguments;
    }

    /*! \brief Check if this command is empty
     *
     * Return true if this command has no arguments and no options
     */
    bool isEmpty() const noexcept
    {
      return mArguments.empty() && mOptions.empty();
    }

    /*! \brief Add a command line option
     */
    void addOption(const BashCompletionGeneratorOption & option)
    {
      mOptions.push_back(option);
    }

    /*! \brief Add a command line option
     *
     * \pre \a shortName must be a valid option short name
     * \sa BashCompletionGeneratorOption::isValidShortName()
     * \pre \a name must be a valid option name
     * \sa BashCompletionGeneratorOption::isValidName()
     */
    void addOption(char shortName, const QString & name)
    {
      assert( BashCompletionGeneratorOption::isValidShortName(shortName) );
      assert( BashCompletionGeneratorOption::isValidName(name) );

      addOption( BashCompletionGeneratorOption(shortName, name) );
    }

    /*! \brief Add a command line option
     *
     * \pre \a name must be a valid option name
     * \sa BashCompletionGeneratorOption::isValidName()
     */
    void addOption(const QString & name)
    {
      assert( BashCompletionGeneratorOption::isValidName(name) );

      addOption( BashCompletionGeneratorOption(name) );
    }

    /*! \brief Check if this command has options
     */
    bool hasOptions() const noexcept
    {
      return !mOptions.empty();
    }

    /*! \brief Get the list of options of this command
     */
    const std::vector<BashCompletionGeneratorOption> & options() const noexcept
    {
      return mOptions;
    }

    /*! \brief Get a generator command from \a command
     */
    static
    BashCompletionGeneratorCommand fromParserDefinitionCommand(const ParserDefinitionCommand & command);

  private:

    QString mName;
    std::vector<BashCompletionGeneratorPositionalArgument> mArguments;
    std::vector<BashCompletionGeneratorOption> mOptions;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_COMMAND_H
