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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H

#include "ParserDefinitionOption.h"
#include "ParserDefinitionPositionalArgument.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <QCoreApplication>
#include <vector>
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

    /*! \brief Check if this command is empty
     *
     * Return true if this command has no arguments and no options
     */
    [[deprecated]]
    bool isEmpty() const noexcept
    {
//       return mArguments.empty() && mOptions.empty();
    }

    /*! \brief Set the description of this command
     */
    void setDescription(const QString & description)
    {
      mDescription = description;
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

      addPositionalArgument(ArgumentType::Unspecified, name, description, syntax);
    }

    /*! \brief Add a positional argument
     *
     * \pre \a name must not be empty
     */
    void addPositionalArgument(ArgumentType type, const QString & name, const QString & description, const QString & syntax = QString())
    {
      assert( !name.trimmed().isEmpty() );

      mPositionalArguments.emplace_back(type, name, description, syntax);
    }

    /*! \brief Check if this command has positional arguments
     */
    bool hasPositionalArguments() const noexcept
    {
      return !mPositionalArguments.empty();
    }

    /*! \brief Get the positional arguments of this command
     */
    const std::vector<ParserDefinitionPositionalArgument> & positionalArguments() const noexcept
    {
      return mPositionalArguments;
    }

   private:

    QString mName;
    QString mDescription;
    std::vector<ParserDefinitionOption> mOptions;
    std::vector<ParserDefinitionPositionalArgument> mPositionalArguments;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H
