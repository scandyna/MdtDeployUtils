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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H

#include "ParserDefinitionOption.h"
#include "ParserDefinitionPositionalArgument.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <QCoreApplication>
#include <vector>
#include <cstddef>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Representation of a command in a ParserDefinition
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserDefinitionCommand
  {
    Q_DECLARE_TR_FUNCTIONS(ParserDefinitionCommand)

   public:

    /*! \brief Construct a command
     */
    ParserDefinitionCommand() noexcept = default;

    /*! \brief Construct a command with a name
     *
     * \pre \a name must not be empty
     */
    explicit ParserDefinitionCommand(const QString & name)
     : mName( name.trimmed() )
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Copy construct a command from \a other
     */
    ParserDefinitionCommand(const ParserDefinitionCommand & other) = default;

    /*! \brief Copy assign \a other to this command
     */
    ParserDefinitionCommand & operator=(const ParserDefinitionCommand & other) = default;

    /*! \brief Move construct a command from \a other
     */
    ParserDefinitionCommand(ParserDefinitionCommand && other) = default;

    /*! \brief Move assign \a other to this command
     */
    ParserDefinitionCommand & operator=(ParserDefinitionCommand && other) = default;

    /*! \brief Set the name of this command
     *
     * \pre \a name must not be empty
     */
    void setName(const QString & name)
    {
      assert( !name.trimmed().isEmpty() );

      mName = name.trimmed();
    }

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

    /*! \brief Get the length of the name
     */
    int nameLength() const noexcept
    {
      return mName.length();
    }

    /*! \brief Set the description of this command
     */
    void setDescription(const QString & description)
    {
      mDescription = description;
    }

    /*! \brief Check if this command has a description
     */
    bool hasDescription() const noexcept
    {
      return !mDescription.isEmpty();
    }

    /*! \brief Get the description of this command
     */
    const QString & description() const noexcept
    {
      return mDescription;
    }

    /*! \brief Add a option
     */
    void addOption(const ParserDefinitionOption & option)
    {
      mOptions.push_back(option);
    }

    /*! \brief Add a option
     *
     * \pre \a name must be a valid option name
     * \sa ParserDefinitionOption::isValidName()
     */
    void addOption(const QString & name, const QString & description)
    {
      assert( ParserDefinitionOption::isValidName(name) );

      addOption( ParserDefinitionOption(name, description) );
    }

    /*! \brief Add a option
     *
     * \pre \a shortName must be a valid option short name
     * \pre \a name must be a valid option name
     * \sa ParserDefinitionOption::isValidShortName()
     * \sa ParserDefinitionOption::isValidName()
     */
    void addOption(char shortName, const QString & name, const QString & description)
    {
      assert( ParserDefinitionOption::isValidShortName(shortName) );
      assert( ParserDefinitionOption::isValidName(name) );

      addOption( ParserDefinitionOption(shortName, name, description) );
    }

    /*! \brief Add the help option
     */
    void addHelpOption()
    {
      addOption( 'h', QLatin1String("help"), tr("Displays help on commandline options.") );
    }

    /*! \brief Check if this command has at least 1 option
     */
    bool hasOptions() const noexcept
    {
      return !mOptions.empty();
    }

    /*! \brief Get the count of options of this result command
     */
    int optionCount() const noexcept
    {
      return static_cast<int>( mOptions.size() );
    }

    /*! \brief Get the option at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < optionCount() )
     */
    const ParserDefinitionOption & optionAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < optionCount() );

      return mOptions[static_cast<size_t>(index)];
    }

    /*! \brief Get the options of this command
     */
    const std::vector<ParserDefinitionOption> & options() const noexcept
    {
      return mOptions;
    }

    /*! \brief Add a positional argument
     *
     * \pre \a name must not be empty
     */
    void addPositionalArgument(const QString & name, const QString & description, const QString & syntax = QString())
    {
      assert( !name.trimmed().isEmpty() );

      addPositionalArgument(ValueType::Unspecified, name, description, syntax);
    }

    /*! \brief Add a positional argument
     *
     * \pre \a name must not be empty
     */
    void addPositionalArgument(ValueType valueType, const QString & name, const QString & description, const QString & syntax = QString())
    {
      assert( !name.trimmed().isEmpty() );

      mPositionalArguments.emplace_back(valueType, name, description, syntax);
    }

    /*! \brief Add a positional argument
     */
    void addPositionalArgument(const ParserDefinitionPositionalArgument & argument)
    {
      mPositionalArguments.push_back(argument);
    }

    /*! \brief Check if this command has positional arguments
     */
    bool hasPositionalArguments() const noexcept
    {
      return !mPositionalArguments.empty();
    }

    /*! \brief Get the count of arguments of this result command
     */
    int positionalArgumentCount() const noexcept
    {
      return static_cast<int>( mPositionalArguments.size() );
    }

    /*! \brief Check if this command has a positional argument at \a index
     *
     * \pre \a index must be >= 0
     */
    bool hasPositionalArgumentAt(int index) const noexcept
    {
      assert( index >= 0 );

      return index < positionalArgumentCount();
    }

    /*! \brief Get the positional argument at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < positionalArgumentCount() )
     */
    const ParserDefinitionPositionalArgument & positionalArgumentAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < positionalArgumentCount() );

      return mPositionalArguments[static_cast<size_t>(index)];
    }

    /*! \brief Get the positional arguments of this command
     */
    const std::vector<ParserDefinitionPositionalArgument> & positionalArguments() const noexcept
    {
      return mPositionalArguments;
    }

    /*! \brief Get the help text for this command
     *
     * \sa ParserDefinition::getSubCommandHelpText()
     */
    QString getHelpText(const QString & applicationName) const;

   private:

    QString getUsageText(const QString & applicationName) const;
    QString getOptionsHelpText() const;
    QString getPositionalArgumentsHelpText() const;

    QString mName;
    QString mDescription;
    std::vector<ParserDefinitionOption> mOptions;
    std::vector<ParserDefinitionPositionalArgument> mPositionalArguments;
    int mHelpTextLineMaxLength = 80;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H
