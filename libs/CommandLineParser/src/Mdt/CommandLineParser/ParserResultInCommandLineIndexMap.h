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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_IN_COMMAND_LINW_INDEX_MAP_H
#define MDT_COMMAND_LINE_PARSER_PARSER_RESULT_IN_COMMAND_LINW_INDEX_MAP_H

#include "ParserResult.h"
#include "mdt_commandlineparser_export.h"
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Helper class to map command-line index to a index in a result
   *
   * Example for a simple application:
   * \code
   * app -f --verbose arg1 arg2 arg3
   * \endcode
   *
   * In the command-line, \a app is at index 0,
   * options at index 1 and 2,
   * \a arg1 at index 3, \a arg2 at index 4, \a arg3 at index 5
   *
   * Example for a simple application:
   * \code
   *                                    app -f --verbose arg1 arg2 arg3
   * // command-line index:             0   1  2         3    4    5
   * // positional arguments in result:                  0    1    2
   * \endcode
   *
   * Example for a application with a \a copy sub-command:
   * \code
   *                                                 app --verbose arg1 copy -f file.txt /tmp
   * // command-line index:                          0   1         2    3    4  5        6
   * // positional arguments in result main command:               0
   * // positional arguments in result sub-command:                             0        1
   * \endcode
   *
   * \note Current implementation has some limitations.
   * It expects that all options are passed before any positional arguments:
   * \code
   * app -f arg1 --verbose
   * // NOT supported
   * 
   * app --
   * // NOT supported
   * \endcode
   * Options values are currently also not supported:
   * \code
   * app --file file.txt other-argument
   * // NOT supported
   * \endcode
   * Supporting above features will require to write a custom command line parser,
   * which is currently not planned (could be in some future).
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserResultInCommandLineIndexMap
  {
   public:

    ParserResultInCommandLineIndexMap() = delete;

    /*! \brief Construct a index map from \a parserResult
     */
    explicit ParserResultInCommandLineIndexMap(const ParserResult & parserResult)
    {
      mCommandLineArgumentCount = commandLineArgumentCount(parserResult);

      mMainCommandOptionCount = parserResult.mainCommand().optionCount();
      mMainCommandPositionalArgumentCount = parserResult.mainCommand().positionalArgumentCount();

      mSubCommandOptionCount = parserResult.subCommand().optionCount();
    }

    /*! \brief Copy construct a index map from \a other
     */
    ParserResultInCommandLineIndexMap(const ParserResultInCommandLineIndexMap & other) = default;

    /*! \brief Copy assign \a other to this index map
     */
    ParserResultInCommandLineIndexMap & operator=(const ParserResultInCommandLineIndexMap & other) = default;

    /*! \brief Move construct a index map from \a other
     */
    ParserResultInCommandLineIndexMap(ParserResultInCommandLineIndexMap && other) noexcept = default;

    /*! \brief Move assign \a other to this index map
     */
    ParserResultInCommandLineIndexMap & operator=(ParserResultInCommandLineIndexMap && other) noexcept = default;

    /*! \brief Get the count of arguments of the command-line represented by the result
     */
    int commandLineArgumentCount() const noexcept
    {
      return mCommandLineArgumentCount;
    }

    /*! \brief Get the count of options in the main command
     */
    int mainCommandOptionCount() const noexcept
    {
      return mMainCommandOptionCount;
    }

    /*! \brief Check if \a commandLineIndex refers to a option in the main command
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    bool isCommandLineIndexAtMainCommandOption(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      return (commandLineIndex >= 1) && (commandLineIndex <= mMainCommandOptionCount);
    }

    /*! \brief Get the count of positional arguments in the main command
     */
    int mainCommandPositionalArgumentCount() const noexcept
    {
      return mMainCommandPositionalArgumentCount;
    }

    /*! \brief Get the index of a positional argument in the result from \a commandLineIndex
     *
     * Returns the index in the result main command if \a commandLineIndex refers to any,
     * otherwise a value < 0
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    int mainCommandPositionalArgumentIndexFromCommandLineIndex(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      const int index = commandLineIndex - mMainCommandOptionCount - 1;
      if(index >= mMainCommandPositionalArgumentCount){
        return -1;
      }
      return index;
    }

    /*! \brief Check if \a commandLineIndex refers to a option in the sub-command
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    bool isCommandLineIndexAtSubCommandOption(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      const int index = commandLineIndex - 1 - mMainCommandOptionCount - mMainCommandPositionalArgumentCount - 1;
      if(index < 0){
        return false;
      }
      if(index >= mSubCommandOptionCount){
        return false;
      }

      return true;
    }

    /*! \brief Get the index of a positional argument in the sub-command in the result from \a commandLineIndex
     *
     * Returns the index in the result sub-command if \a commandLineIndex refers to any,
     * otherwise a value < 0
     *
     * \pre \a commandLineIndex must be in valid range ( 0 <= \a commandLineIndex < commandLineArgumentCount() )
     */
    int subCommandPositionalArgumentIndexFromCommandLineIndex(int commandLineIndex) const noexcept
    {
      assert( commandLineIndex >= 0 );
      assert( commandLineIndex < commandLineArgumentCount() );

      return commandLineIndex - 1 - mMainCommandOptionCount - mMainCommandPositionalArgumentCount - 1 - mSubCommandOptionCount;
    }

    /*! \brief Get the count of arguments of the command-line represented by \a parserResult
     */
    static
    int commandLineArgumentCount(const ParserResult & parserResult) noexcept
    {
      int count = 1 + parserResult.mainCommand().optionCount() + parserResult.mainCommand().positionalArgumentCount();
      if( parserResult.hasSubCommand() ){
        count += 1 + parserResult.subCommand().optionCount() + parserResult.subCommand().positionalArgumentCount();
      }
      return count;
    }

   private:

    int mCommandLineArgumentCount;
    int mMainCommandOptionCount;
    int mMainCommandPositionalArgumentCount;
    int mSubCommandOptionCount;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_IN_COMMAND_LINW_INDEX_MAP_H
