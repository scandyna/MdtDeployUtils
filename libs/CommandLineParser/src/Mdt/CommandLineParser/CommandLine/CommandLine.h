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
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

  /*! \brief A representation of a command line
   *
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

    /*! \brief Add a option value to the end of this command line
     *
     * \pre the last insert argument must be a option
     */
    void appendOptionValue(const QString & value)
    {
      assert( !isEmpty() );
      assert( isOption( mArgumentList.back() ) );

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

//     /*! \brief Get the index of the sub-command name in this command line
//      *
//      * Returns the index of of the sub-command name if exists,
//      * otherwise -1.
//      */
//     int subCommandNameIndex() const noexcept
//     {
//       return mSubCommandNameIndex;
//     }

//     /*! \brief Get the value of the argument at \a pos
//      *
//      * \pre \a pos must be at valid range ( 0 <= \a pos < argumentCount() ).
//      */
//     QString valueAt(int pos) const
//     {
//       assert( pos >= 0 );
//       assert( pos < argumentCount() );
//     }

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
     * \sa ParserDefinitionOption::isValidName(name)
     */
    static
    bool isValidOptionName(const QString & name)
    {
      return ParserDefinitionOption::isValidName(name);
    }

   private:

//     int mSubCommandNameIndex;
    ArgumentList mArgumentList;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_COMMAND_LINE_H
