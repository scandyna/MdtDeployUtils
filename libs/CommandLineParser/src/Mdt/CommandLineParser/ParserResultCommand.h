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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_PARSER_RESULT_COMMAND_H

#include "ParserResultOption.h"
#include "ParserDefinitionOption.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <vector>
#include <algorithm>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Command result from a Parser
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserResultCommand
  {
   public:

    /*! \brief Construct a command
     */
    ParserResultCommand() noexcept = default;

    /*! \brief Construct a command with a name
     *
     * \pre \a name must not be empty
     */
    explicit ParserResultCommand(const QString & name)
     : mName( name.trimmed() )
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Copy construct a command from \a other
     */
    ParserResultCommand(const ParserResultCommand & other) = default;

    /*! \brief Copy assign \a other to this command
     */
    ParserResultCommand & operator=(const ParserResultCommand & other) = default;

    /*! \brief Move construct a command from \a other
     */
    ParserResultCommand(ParserResultCommand && other) = default;

    /*! \brief Move assign \a other to this command
     */
    ParserResultCommand & operator=(ParserResultCommand && other) = default;

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

    /*! \brief Add \a option to this result
     */
    void addOption(const ParserResultOption & option)
    {
      mOptions.push_back(option);
    }

    /*! \brief Set \a options to this command
     */
    void setOptions(const std::vector<ParserResultOption> & options)
    {
      mOptions = options;
    }

    /*! \brief Set a value to the last option in this command
     *
     * \pre this command must have at least 1 option
     * \sa hasOptions()
     */
    void setLastOptionValue(const QString & value)
    {
      assert( hasOptions() );

      mOptions.back().setValue(value);
    }

    /*! \brief Check if this result result has any option
     */
    bool hasOptions() const noexcept
    {
      return !mOptions.empty();
    }

    /*! \brief Get the count of options in this result command
     */
    int optionCount() const noexcept
    {
      return static_cast<int>( mOptions.size() );
    }

    /*! \brief Get the options of this result
     */
    const std::vector<ParserResultOption> & options() const noexcept
    {
      return mOptions;
    }

    /*! \brief Check if \a option is set in this result command
     */
    bool isSet(const ParserDefinitionOption & option) const
    {
      return isSet( option.name() );
    }

    /*! \brief Check if \a optionName is set in this result command
     */
    bool isSet(const QString & optionName) const
    {
      const auto pred = [&optionName](const ParserResultOption & option){
        return option.name() == optionName;
      };
      const auto it = std::find_if(mOptions.cbegin(), mOptions.cend(), pred);

      return it != mOptions.cend();
    }

    /*! \brief Check if the help option is set in this result command
     */
    bool isHelpOptionSet() const
    {
      return isSet( QLatin1String("help") );
    }

    /*! \brief Returns a list of option values found for the given \a option
     *
     * For options found by the parser, the list will contain an entry for each time the option was encountered by the parser.
     *
     * If the option wasn't specified on the command line, the default values are returned.
     */
    QStringList getValues(const ParserDefinitionOption & option) const
    {
      QStringList values;

      // We should have something like std::transform_if() to use STL algorithm
      for(const ParserResultOption & resultOption : mOptions){
        if( resultOption.name() == option.name() ){
          values.push_back( resultOption.value() );
        }
      }

      if( values.isEmpty() ){
        values = option.defaultValues();
      }

      return values;
    }

    /*! \brief Check if this result command has any positional argument
     */
    bool hasPositionalArguments() const noexcept
    {
      return !mPositionalArguments.empty();
    }

    /*! \brief Get the count of arguments of this result command
     */
    int positionalArgumentCount() const noexcept
    {
      return mPositionalArguments.count();
    }

    /*! \brief Add \a argument to this result command
     */
    void addPositionalArgument(const QString & argument)
    {
      mPositionalArguments.append(argument);
    }

    /*! \brief Get positional arguments of this result command
     */
    const QStringList & positionalArguments() const noexcept
    {
      return mPositionalArguments;
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
     * \pre \a index must be in valid range ( 0 <= \a index < argumentCount() )
     */
    const QString & positionalArgumentAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < positionalArgumentCount() );

      return mPositionalArguments.at(index);
    }

   private:

    QString mName;
    std::vector<ParserResultOption> mOptions;
    QStringList mPositionalArguments;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_COMMAND_H
