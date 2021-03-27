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
#ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_COMMAND_LINE_H
#define MDT_COMMAND_LINE_PARSER_COMMAND_LINE_COMMAND_LINE_H

#include "Argument.h"
#include "Algorithm.h"
#include "../ParserDefinitionOption.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <vector>
#include <cstddef>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

  /*! \brief A representation of a command line
   *
   * This is a intermediate representation of the command line,
   * that can be used to query about a argument at a specific position.
   * One example is a parser for Bash completion.
   *
   * To handle the command line in a application,
   * ParserResult should be used
   *
   * \sa ParserResult
   * \sa ArgumentListAttributes
   * \sa https://www.gnu.org/software/guile/manual/html_node/Command-Line-Format.html
   * \sa https://www.gnu.org/prep/standards/html_node/Command_002dLine-Interfaces.html
   */
  class MDT_COMMANDLINEPARSER_EXPORT CommandLine
  {
   public:

    /*! \brief Construct a empty command line
     */
    explicit CommandLine() noexcept = default;

    /*! \brief Copy construct a command line from \a other
     */
    CommandLine(const CommandLine & other) = default;

    /*! \brief Copy assign \a other to this command line
     */
    CommandLine & operator=(const CommandLine & other) = default;

    /*! \brief Move construct a command line from \a other
     */
    CommandLine(CommandLine && other) noexcept = default;

    /*! \brief Move assign \a other to this command line
     */
    CommandLine & operator=(CommandLine && other) noexcept = default;

    /*! \brief Set the executable name
     *
     * \pre this command line must be empty
     * \sa isEmpty()
     */
    void setExecutableName(const QString & name)
    {
      assert( isEmpty() );

      mArgumentList.emplace_back( Executable{name} );
    }

    /*! \brief Add a positional argument to the end of this command line
     */
    void appendPositionalArgument(const QString & value)
    {
      mArgumentList.emplace_back( PositionalArgument{value} );
    }

    /*! \brief Add a option to the end of this command line
     *
     * \pre \a name must be a valid option name
     * \sa isValidOptionName()
     */
    void appendOption(const QString & name)
    {
      assert( isValidOptionName(name) );

      mArgumentList.emplace_back( Option{name} );
    }

    /*! \brief Add a unknown option to the end of this command line
     *
     * \pre \a name must be a valid option name
     * \sa isValidOptionName()
     */
    void appendUnknownOption(const QString & name)
    {
      assert( isValidOptionName(name) );

      mArgumentList.emplace_back( UnknownOption{name} );
    }

    /*! \brief Add a option that expects a value to the end of this command line
     *
     * \pre \a name must be a valid option name
     * \sa isValidOptionName()
     */
    void appendOptionExpectingValue(const QString & name)
    {
      assert( isValidOptionName(name) );

      mArgumentList.emplace_back( Option{name,true} );
    }

    /*! \brief Add a list of short options to the end of this command line
     *
     * \pre \a options must have more than 1 option
     * \pre each name in \a options must be a valid short name
     * \sa isValidShortOptionNameList()
     */
    void appendShortOptionList(const std::vector<char> & options) noexcept
    {
      assert( options.size() > 1 );
      assert( isValidShortOptionNameList(options) );

      mArgumentList.emplace_back( ShortOptionList{options,false} );
    }

    /*! \brief Add a list of short options, with the last one expecting a value, to the end of this command line
     *
     * \pre \a options must have more than 1 option
     * \pre each name in \a options must be a valid short name
     * \sa isValidShortOptionNameList()
     */
    void appendShortOptionListWithLastExpectingValue(const std::vector<char> & options) noexcept
    {
      assert( options.size() > 1 );
      assert( isValidShortOptionNameList(options) );

      mArgumentList.emplace_back( ShortOptionList{options,true} );
    }

    /*! \brief Add a option value to the end of this command line
     *
     * \pre the last insert argument must be a option expecting a value,
     *  or a short option list with the last one expecting a value
     */
    void appendOptionValue(const QString & value)
    {
      assert( !isEmpty() );
      assert( isOptionExpectingValue( mArgumentList.back() ) );

      mArgumentList.emplace_back( OptionValue{value} );
    }

    /*! \brief Add a option with a value to the end of this command line
     *
     * \pre \a name must be a valid option name
     * \sa isValidOptionName()
     */
    void appendOptionWithValue(const QString & name, const QString & value)
    {
      assert( isValidOptionName(name) );

      mArgumentList.emplace_back( OptionWithValue{name,value} );
    }

    /*! \brief Add a list of short options, with the last having a value, to this end of this command line
     *
     * \pre \a options must have more than 1 option
     * \pre each name in \a options must be a valid short name
     * \sa isValidShortOptionNameList()
     */
    void appendShortOptionListWithLastHavingValue(const std::vector<char> & options, const QString & value) noexcept
    {
      assert( options.size() > 1 );
      assert( isValidShortOptionNameList(options) );

      mArgumentList.emplace_back( ShortOptionListWithLastHavingValue{options,value} );
    }

    /*! \brief Add a single dash to the end of this command line
     */
    void appendSingleDash() noexcept
    {
      mArgumentList.emplace_back( SingleDash{} );
    }

    /*! \brief Add a double dash to the end of this command line
     */
    void appendDoubleDash() noexcept
    {
      mArgumentList.emplace_back( DoubleDash{} );
    }

    /*! \brief Add the sub-command name to the end of this command line
     */
    void appendSubCommandName(const QString & name)
    {
      mArgumentList.emplace_back( SubCommandName{name} );
    }

    /*! \brief Clear this command line
     */
    void clear() noexcept
    {
      mArgumentList.clear();
    }

    /*! \brief Check if this command line is empty
     *
     * \sa argumentCount()
     */
    bool isEmpty() const noexcept
    {
      return mArgumentList.empty();
    }

    /*! \brief Get the count of arguments in this command line
     *
     * \note The executable name is part of the argument count
     */
    int argumentCount() const noexcept
    {
      return static_cast<int>( mArgumentList.size() );
    }

    /*! \brief Get the executable name
     *
     * \pre the executable name must have been set
     * \sa setExecutableName()
     */
    QString executableName() const noexcept
    {
      assert( !isEmpty() );
      assert( isExecutableName( argumentAt(0) ) );

      return getExecutableName( argumentAt(0) );
    }

    /*! \brief Get the argument at \a index
     *
     * \note Currently, Boost.Variant is used,
     *  but std::variant will be used in the future.
     *  This means that the caller will probably have to rewrite
     *  some pieces, like the visitors.
     *
     * \pre \a index must be in valid range ( 0 <= \a index < argumentCount() ).
     */
    const Argument & argumentAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < argumentCount() );

      return mArgumentList[static_cast<size_t>(index)];
    }

    /*! \brief Check if the argument at \a index is a positional argument
     *
     * \pre \a index must be in valid range ( 0 <= \a index < argumentCount() ).
     */
    bool argumentAtIsPositionalArgument(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < argumentCount() );

      return isPositionalArgument( argumentAt(index) );
    }

    /*! \brief Access the internal argument list
     *
     * Returns a list of variants representing each node in the command-line.
     *
     * \note Currently, Boost.Variant is used,
     *  but std::variant will be used in the future.
     *  This means that the caller will probably have to rewrite
     *  some pieces, like the visitors.
     *
     * \sa Mdt::CommandLineParser::CommandLine namespace
     */
    const ArgumentList & argumentList() const noexcept
    {
      return mArgumentList;
    }

    /*! \brief Check if \a name is a valid name for a option
     *
     * \sa Mdt::CommandLineParser::ParserDefinitionOption::isValidName()
     */
    static
    bool isValidOptionName(const QString & name)
    {
      return ParserDefinitionOption::isValidName(name);
    }

    /*! \brief Check if each name in \a options is a valid short name
     *
     * \sa Mdt::CommandLineParser::ParserDefinitionOption::isValidShortName()
     */
    static
    bool isValidShortOptionNameList(const std::vector<char> & options) noexcept
    {
      for( char c : options ){
        if( !ParserDefinitionOption::isValidShortName(c) ){
          return false;
        }
      }

      return true;
    }

   private:

    ArgumentList mArgumentList;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_COMMAND_LINE_H
