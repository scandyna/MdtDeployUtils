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
#ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ARGUMENT_LIST_ATTRIBUTES_H
#define MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ARGUMENT_LIST_ATTRIBUTES_H

#include "Argument.h"
#include "Algorithm.h"
#include "mdt_commandlineparser_export.h"
#include <cassert>
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

  /*! \brief Helper class to get some attributes of a command line arguments list
   *
   * It can be useful to map some attributes, based on a position in the command line,
   * to other data structures, like ParserResult or ParserDefinition.
   * This is for example the case while handling Bash completion.
   *
   * This class help to answer questions like
   * 'how many positional arguments are there in the main command of this command line',
   * or 'is this the first, second, ... positional argument in the sub-command'.
   *
   * Example for a simple application:
   * \code
   *                                    app -f --verbose arg1 arg2 arg3
   * // command-line index:             0   1  2         3    4    5
   * // positional arguments index:                      0    1    2
   * \endcode
   *
   * Example for a application with a \a copy sub-command, positional arguments, option and option value:
   * \code
   *                                                 app --verbose arg1 copy -f --overwrite-behavior keep file.txt /tmp
   * // command-line index:                          0   1         2    3    4  5                    6    7        8
   * // positional arguments index in main command:                0
   * // positional arguments index in sub-command:                                                        0        1
   * \endcode
   *
   * Same as before, but using \a option=value syntax:
   * \code
   *                                                 app --verbose arg1 copy -f --overwrite-behavior=keep file.txt /tmp
   * // command-line index:                          0   1         2    3    4  5                         6        7
   * // positional arguments index in main command:                0
   * // positional arguments index in sub-command:                                                        0        1
   * \endcode
   */
  class MDT_COMMANDLINEPARSER_EXPORT ArgumentListAttributes
  {
    using const_iterator = ArgumentList::const_iterator;
    using difference_type = const_iterator::difference_type;

   public:

    /*! \brief Construct a command line argument list attributes to no range
     *
     * \sa setArgumentList()
     */
    explicit ArgumentListAttributes() noexcept = default;

    /*! \brief Construct a command line argument list attributes from \a first and \a last iterators
     */
    explicit ArgumentListAttributes(const_iterator first, const_iterator last) noexcept
     : mBegin(first),
       mEnd(last)
    {
      setSubCommandNameIterator();
    }

    /*! \brief Copy construct a command line argument list attributes from \a other
     */
    ArgumentListAttributes(const ArgumentListAttributes & other) noexcept = default;

    /*! \brief Copy assign \a other to this command line argument list attributes
     */
    ArgumentListAttributes & operator=(const ArgumentListAttributes & other) noexcept = default;

    /*! \brief Move construct a command line argument list attributes from \a other
     */
    ArgumentListAttributes(ArgumentListAttributes && other) noexcept = default;

    /*! \brief Move assign \a other to this command line argument list attributes
     */
    ArgumentListAttributes & operator=(ArgumentListAttributes && other) noexcept = default;

    /*! \brief Set the argument list defined by \a first and \a last iterators
     */
    void setArgumentList(const_iterator first, const_iterator last) noexcept
    {
      mBegin = first;
      mEnd = last;
      setSubCommandNameIterator();
    }

    /*! \brief Set the argument list
     */
    void setArgumentList(const ArgumentList & argumentList) noexcept
    {
      setArgumentList( argumentList.cbegin(), argumentList.cend() );
    }

    /*! \brief Get the count of arguments of the command-line
     */
    int commandLineArgumentCount() const noexcept
    {
      return static_cast<int>( std::distance(mBegin, mEnd) );
    }

    /*! \brief Get the argument at \a commandLineIndex
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    const Argument & argumentAtCommandLineIndex(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      return *iteratorFromCommandLineIndex(commandLineIndex);
    }

    /*! \brief Check if \a commandLineIndex refers to a option in the main command
     *
     * \todo Must clarify what we consider a option (option name, option value, option with value ?)
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    bool isCommandLineIndexAtMainCommandOption(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      if( !isCommandLineIndexInMainCommand(commandLineIndex) ){
        return false;
      }

      return isOption( argumentAtCommandLineIndex(commandLineIndex) );
    }

    /*! \brief Get the index of a positional argument in the main command from \a commandLineIndex
     *
     * Returns the positional argument index in the main command if \a commandLineIndex refers to any,
     * otherwise a value < 0
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    int findMainCommandPositionalArgumentIndexFromCommandLineIndex(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      auto it = iteratorFromCommandLineIndex(commandLineIndex);
      if( !isCommandLineIteratorInMainCommand(it) ){
        return -1;
      }
      ++it; // Act like a past-the-end iterator

      return static_cast<int>( std::count_if( mBegin, it, isPositionalArgument ) ) - 1;
    }

    /*! \brief Get the index of the sub-command name in this command line
     *
     * Returns the index of the sub-command name in this command line if exists,
     * otherwise a value < 0
     */
    int subCommandNameIndex() const noexcept
    {
      if(mSubCommandNameIt == mEnd){
        return -1;
      }

      return static_cast<int>( std::distance(mBegin, mSubCommandNameIt) );
    }

    /*! \brief Check if \a commandLineIt is in the main command
     */
    bool isCommandLineIteratorInMainCommand(const_iterator commandLineIt) const noexcept
    {
      if(mSubCommandNameIt == mEnd){
        return true;
      }

      return commandLineIt < mSubCommandNameIt;
    }

    /*! \brief Check if \a commandLineIndex is in the main command
     */
    inline
    bool isCommandLineIndexInMainCommand(int commandLineIndex) const noexcept
    {
      return isCommandLineIteratorInMainCommand( iteratorFromCommandLineIndex(commandLineIndex) );
    }

    /*! \brief Check if \a commandLineIt is in the sub-command
     *
     * Return true if \a commandLineIt is past the sub-command name,
     * otherwise false.
     */
    bool isCommandLineIteratorInSubCommand(const_iterator commandLineIt) const noexcept
    {
      return commandLineIt > mSubCommandNameIt;
    }

    /*! \brief Check if \a commandLineIndex is in the sub-command
     *
     * Return true if \a commandLineIndex is past the sub-command name,
     * otherwise false.
     */
    bool isCommandLineIndexInSubCommand(int commandLineIndex) const noexcept
    {
      return isCommandLineIteratorInSubCommand( iteratorFromCommandLineIndex(commandLineIndex) );
    }

    /*! \brief Check if \a commandLineIndex refers to a option in the sub-command
     *
     * \todo Must clarify what we consider a option (option name, option value, option with value ?)
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    bool isCommandLineIndexAtSubCommandOption(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      if( !isCommandLineIndexInSubCommand(commandLineIndex) ){
        return false;
      }

      return isOption( argumentAtCommandLineIndex(commandLineIndex) );
    }

    /*! \brief Get the index of a positional argument in the sub-command in the result from \a commandLineIndex
     *
     * Returns the positional argument index in the sub-command if \a commandLineIndex refers to any,
     * otherwise a value < 0
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    int findSubCommandPositionalArgumentIndexFromCommandLineIndex(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      auto it = iteratorFromCommandLineIndex(commandLineIndex);
      if( !isCommandLineIteratorInSubCommand(it) ){
        return -1;
      }
      ++it; // Act like a past-the-end iterator

      return static_cast<int>( std::count_if( mSubCommandNameIt+1, it, isPositionalArgument ) ) - 1;
    }

   private:

    void setSubCommandNameIterator() noexcept
    {
      mSubCommandNameIt = findSubCommandNameArgument(mBegin, mEnd);
    }

    inline
    const_iterator iteratorFromCommandLineIndex(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      return std::next( mBegin, static_cast<difference_type>(commandLineIndex) );
    }

    const_iterator mBegin = const_iterator{};
    const_iterator mEnd = const_iterator{};
    const_iterator mSubCommandNameIt = const_iterator{};
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ARGUMENT_LIST_ATTRIBUTES_H
