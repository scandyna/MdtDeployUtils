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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_QUERY_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_QUERY_H

#include "ParserResult.h"
#include "ParserDefinition.h"
#include "ParserResultInCommandLineIndexMap.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <cassert>

#include <QDebug>
#include "ParserResultDebug.h"
#include <iostream>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Helper class to access a parser reult with Bash completion arguments
   *
   * This class is mostly not usefull to use this library.
   * handleBashCompletion() should be used, which cares about details exposed here.
   *
   * \section findCurrentPositionalArgumentName "findCurrentPositionalArgumentName"
   *
   * The completion script will call the executable as follow:
   * \code
   * "$executable" completion-find-current-positional-argument-name $COMP_CWORD $COMP_LINE
   * \endcode
   *
   * After parsing, the parser result will contain at least 3 positional arguments:
   * - The query argument, findCurrentPositionalArgumentNameString()
   * - The cursor position in the command-line arguments, result of \a COMP_CWORD
   * - The executable in \a COMP_LINE
   *
   * The \a COMP_LINE will again contain the executable name,
   * then the positional arguments we have to parse.
   * 
   * For example, if the user types:
   * \code
   * myapp <TAB>
   * \endcode
   * the executable \a myapp will be called with those arguments:
   * \code
   * "myapp" completion-find-current-positional-argument-name 1 myapp
   * //                                                      cursor:  ^
   * \endcode
   * Note that the cursor is at position 1,
   * which is just after \a myapp .
   *
   * \section references References
   *
   * \sa https://iridakos.com/programming/2018/03/01/bash-programmable-completion-tutorial
   * \sa https://www.gnu.org/software/bash/manual/html_node/Programmable-Completion.html#Programmable-Completion
   * \sa https://www.gnu.org/software/bash/manual/html_node/Bash-Variables.html
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionParserQuery
  {
   public:

    BashCompletionParserQuery() = delete;

    /*! \brief Construct a query from \a parserResult
     *
     * \pre \a parserResult must be a valid Bash completion query
     * \sa isValidBashCompletionQuery()
     */
    explicit BashCompletionParserQuery(const ParserResult & parserResult, const ParserDefinition & parserDefinition)
     : mParserResultInCommandLineIndexMap( parserResultWithoutQueryArguments(parserResult) )
    {
      
      std::cout << toDebugString(parserResult).toStdString() << std::endl;
      
      assert( isValidBashCompletionQuery(parserResult) );

      mCursorInCompLinePositionIndex = extractCursorInComplinePositionIndex(parserResult);
      assert( mCursorInCompLinePositionIndex >= 0 );

      mMainCommandPositionalArgumentsCount = parserResult.positionalArgumentCount()-3;
      assert( mMainCommandPositionalArgumentsCount >= 0 );

      mMainCommandOptionsCount = parserResult.mainCommand().options().size();

      mIsCursorAtMainCommandOption = isCursorAtMainCommandOption(parserResult);

      mParserDefinitionHasSubCommand = parserDefinition.hasSubCommands();

      mResultHasSubCommand = parserResult.hasSubCommand();

      mSubCommandPositionalArgumentsCount = parserResult.subCommand().positionalArgumentCount();

      mSubCommandOptionsCount = parserResult.subCommand().options().size();

      mIsCursorAtSubCommandOption = isCursorAtSubCommandOption(parserResult);
    }

    /*! \brief Copy construct a query from \a other
     */
    BashCompletionParserQuery(const BashCompletionParserQuery & other) = default;

    /*! \brief Copy assign \a other to this query
     */
    BashCompletionParserQuery & operator=(const BashCompletionParserQuery & other) = default;

    /*! \brief Move construct a query from \a other
     */
    BashCompletionParserQuery(BashCompletionParserQuery && other) noexcept = default;

    /*! \brief Move assign \a other to this query
     */
    BashCompletionParserQuery & operator=(BashCompletionParserQuery && other) noexcept = default;

    /*! \brief Get the cursor position in the command line
     */
    int cursorInCompLinePositionIndex() const noexcept
    {
      return mCursorInCompLinePositionIndex;
    }

    /*! \brief Get the index for which the cursor would be at the sub-command name position in the command-line
     *
     * If the parser definition has no sub-command,
     * this method will allways return invalid index (value < 0).
     *
     * Examples for a application that can take 1 argument for the main command,
     * and has a copy sub-command that can take \a source and \a destination as positional arguments:
     * \code
     * completion-find-current-positional-argument-name x myapp
     * // return: 1
     * // Why: after myapp, we could have a positional argument or a sub-command
     * //      choice: 1 will be the index of the sub-command name
     *
     * completion-find-current-positional-argument-name x myapp copy
     * // return: 1
     * // Why: 1 is the index of a known sub-command name
     *
     * completion-find-current-positional-argument-name x myapp -h copy
     * // return: 2
     * // Why: 2 is the index of a known sub-command name
     *
     * completion-find-current-positional-argument-name x myapp arg1 copy
     * // return: 2
     * // Why: 2 is the index of a known sub-command name
     *
     * completion-find-current-positional-argument-name x myapp arg1 copy file.txt
     * // return: 2
     * // Why: 2 is the index of a known sub-command name
     * \endcode
     */
    int compLineSubCommandNamePositionIndex() const noexcept
    {
      if( !mParserDefinitionHasSubCommand ){
        return -1;
      }
      return 1 + mMainCommandPositionalArgumentsCount + mMainCommandOptionsCount;
    }

    /*! \brief Check if the cursor is in the sub-command in the command line
     *
     * \sa compLineSubCommandNamePositionIndex()
     */
    bool isCursorInSubCommand() const noexcept
    {
      const int subCommandIndex = compLineSubCommandNamePositionIndex();
      if(subCommandIndex < 0){
        return false;
      }
      return cursorInCompLinePositionIndex() >= subCommandIndex;
    }

    /*! \brief Get the count of positional arguments in the main command
     */
    int mainCommandPositionalArgumentsCount() const noexcept
    {
      return mMainCommandPositionalArgumentsCount;
    }

    /*! \brief Get the cursor index mapped to the positional argument of the main command
     *
     * This can be used to get the positional argument in the parser definition.
     *
     * Note that this index will be past of the positional arguments
     * available in a result most of the time, except if the user goes back:
     * \code
     * completion-find-current-positional-argument-name 1 myapp
     * //                                                cursor ^
     * // return: 0
     *
     * completion-find-current-positional-argument-name 2 myapp some-file
     * //                                                          cursor ^
     * // return: 1
     * \endcode
     *
     * If the parser definition supports sub-commands,
     * this method can return a invalid index (value < 0)
     * for some cases:
     * \code
     * completion-find-current-positional-argument-name 1 myapp
     * //                                                cursor ^
     * // return: value < 0
     * // Why: cursor is at sub-command name index
     *
     * completion-find-current-positional-argument-name 2 myapp copy
     * //                                                     cursor ^
     * // return: value < 0
     * // Why: cursor is after sub-command name index
     *
     * completion-find-current-positional-argument-name 1 myapp copy
     * //                                                cursor ^
     * // return: value < 0
     * // Why: cursor is at sub-command name index
     *
     * completion-find-current-positional-argument-name 1 myapp arg1 copy
     * //                                                cursor ^
     * // return: 0
     * // Why: cursor is before sub-command name index
     *
     * completion-find-current-positional-argument-name 2 myapp arg1 copy
     * //                                                     cursor ^
     * // return: value < 0
     * // Why: cursor is at sub-command name index
     *
     * completion-find-current-positional-argument-name 3 myapp arg1 copy
     * //                                                          cursor ^
     * // return: value < 0
     * // Why: cursor is after sub-command name index
     * \endcode
     *
     * \note This index should not be used to get a positional argument
     * in a result, but in a parser definition.
     *
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    int cursorMainCommandPositionalArgumentsIndexInDefinition() const noexcept
    {
      if( isCursorInSubCommand() ){
        return -1;
      }

      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      if(cursor == 0){
        return -1;
      }
      
      /*
       * If cursor is past the last argument in the command-line,
       * ................
       */
//       assert( index >= 1 );

      /*
       * The cursor in the command-line could be after the parser result.
       * This is maybe valid in the parser definition, so
       * get the positional argument index from the map by passing cursor-1,
       * then return index+1
       */
      return mParserResultInCommandLineIndexMap.mainCommandPositionalArgumentIndexFromCommandLineIndex(cursor-1) + 1;
//       qDebug() << "cursorMainCommandPositionalArgumentsIndexInDefinition(), cursor: " << cursorInCompLinePositionIndex();
//       
//       return mParserResultInCommandLineIndexMap.mainCommandPositionalArgumentIndexFromCommandLineIndex( cursorInCompLinePositionIndex() );
//       if( isCursorAtMainCommandOption() ){
//         return mCursorInCompLinePositionIndex - 1;
//       }

      /*
      if(mParserDefinitionHasSubCommand){
        if( cursorInCompLinePositionIndex() >= compLineSubCommandNamePositionIndex() ){
          return -1;
        }
      }
      */
//       if(mParserDefinitionHasSubCommand){
//         return -1;
//       }
//       int index = mCursorInCompLinePositionIndex - 1 - mMainCommandOptionsCount;
//       if(mResultHasSubCommand){
//         --index;
//       }
// 
//       return index;

//       return mCursorInCompLinePositionIndex - 1 - mMainCommandOptionsCount;
    }

    /*! \brief Check if the cursor is at a positional index of the main command in the parser definition
     *
     * Examples for a simple copy app that can take \a source and \a destination
     * as positional arguments:
     * \code
     * completion-find-current-positional-argument-name 1 myapp
     * //                                                cursor ^
     * // return: true
     * \endcode
     *
     * Examples for a app that has a \a copy sub-command
     * that can take \a source and \a destination as positional arguments:
     * \code
     * completion-find-current-positional-argument-name 1 myapp
     * //                                                cursor ^
     * // return: false
     * \endcode
     *
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    bool isCursorAtMainCommandPositionalArgumentsIndexInDefinition() const noexcept
    {
      return cursorMainCommandPositionalArgumentsIndexInDefinition() >= 0;
    }

    /*! \brief Check if the cursor is at a option of the main command
     *
     * Examples:
     * \code
     * completion-find-current-positional-argument-name 1 myapp
     * //                                                cursor ^
     * // return: false
     *
     * completion-find-current-positional-argument-name 1 myapp -
     * //                                                cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 1 myapp -h
     * //                                                cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 1 myapp --
     * //                                                cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 2 myapp -
     * //                                                  cursor ^
     * // return: false
     *
     * completion-find-current-positional-argument-name 1 myapp file
     * //                                                cursor ^
     * // return: false
     *
     * completion-find-current-positional-argument-name 2 myapp -h
     * //                                                   cursor ^
     * // return: false
     * \endcode
     *
     * For application with sub-command,
     * this function returns false if the cursor is in the sub-command.
     *
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    bool isCursorAtMainCommandOption() const noexcept
    {
      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      if( cursor >= mParserResultInCommandLineIndexMap.commandLineArgumentCount() ){
        return false;
      }

      return mParserResultInCommandLineIndexMap.isCommandLineIndexAtMainCommandOption(cursor);

//       return mIsCursorAtMainCommandOption;
    }

    /*! \brief Check if the cursor is at the position of the sub-command name in the definition
     *
     * If the parser definition does not contain any sub-command,
     * this methods will allways return false.
     *
     * Examples for a application that can take 1 argument for the main command,
     * and has a copy sub-command that can take \a source and \a destination as positional arguments:
     * \code
     * completion-find-current-positional-argument-name 1 myapp
     * //                                                cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 2 myapp copy
     * //                                                     cursor ^
     * // return: false
     *
     * completion-find-current-positional-argument-name 1 myapp copy
     * //                                                cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 2 myapp arg1
     * //                                                     cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 1 myapp arg1
     * //                                                cursor ^
     * // return: false
     * \endcode
     *
     * \sa compLineSubCommandNamePositionIndex()
     */
    bool isCursorAtSubCommandNameIndexInDefinition() const noexcept
    {
      return cursorInCompLinePositionIndex() == compLineSubCommandNamePositionIndex();
    }

    /*! \brief Get the count of positional arguments in the sub-command
     */
    int subCommandPositionalArgumentsCount() const noexcept
    {
      return mSubCommandPositionalArgumentsCount;
    }

    /*! \brief Get the cursor index mapped to the positional argument of the sub-command in the parser definition
     *
     * This can be used to get the positional argument in the parser definition.
     *
     * Note that this index will be past of the positional arguments
     * available in a result most of the time, except if the user goes back:
     * \code
     * completion-find-current-positional-argument-name 1 myapp
     * //                                                cursor ^
     *
     * completion-find-current-positional-argument-name 2 myapp copy some-file
     * //                                                               cursor ^
     * \endcode
     *
     * Examples for a application that can take 1 positional argument,
     * then a sub-command that can take \a source and \a destination as positional arguments:
     * \code
     * completion-find-current-positional-argument-name 1 myapp
     * //                                                cursor ^
     * // return: value < 0
     *
     * completion-find-current-positional-argument-name 2 myapp arg1
     * //                                                     cursor ^
     * // return: value < 0
     *
     * completion-find-current-positional-argument-name 2 myapp copy
     * //                                                     cursor ^
     * // return: 0
     *
     * completion-find-current-positional-argument-name 3 myapp copy file.txt
     * //                                                              cursor ^
     * // return: 1
     * \endcode
     *
     * \note This index should not be used to get a positional argument
     * in a result, but in a parser definition.
     *
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    int cursorSubCommandPositionalArgumentsIndexInDefinition() const noexcept
    {
      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      if(cursor == 0){
        return -1;
      }

      /*
       * The cursor in the command-line could be after the parser result.
       * This is maybe valid in the parser definition, so
       * get the positional argument index from the map by passing cursor-1,
       * then return index+1
       */
      return mParserResultInCommandLineIndexMap.subCommandPositionalArgumentIndexFromCommandLineIndex(cursor-1) + 1;

      // -1 for executable and -1 for command
//       return mCursorInCompLinePositionIndex - 1 - mMainCommandPositionalArgumentsCount - 1 - mMainCommandOptionsCount - mSubCommandOptionsCount;
    }

    /*! \brief Check if the cursor is at a positional index of the sub-command in the parser definition
     *
     * \sa cursorSubCommandPositionalArgumentsIndexInDefinition()
     */
    bool isCursorAtSubCommandPositionalArgumentsIndexInDefinition() const noexcept
    {
      return cursorSubCommandPositionalArgumentsIndexInDefinition() >= 0;
    }

    /*! \brief Check if the cursor is at a option of the sub-command
     *
     * Examples:
     * \code
     * completion-find-current-positional-argument-name 1 myapp -h
     * //                                                cursor ^
     * // return: false
     *
     * completion-find-current-positional-argument-name 2 myapp copy
     * //                                                     cursor ^
     * // return: false
     *
     * completion-find-current-positional-argument-name 2 myapp copy -
     * //                                                     cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 2 myapp copy -h
     * //                                                     cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 2 myapp copy --
     * //                                                     cursor ^
     * // return: true
     *
     * completion-find-current-positional-argument-name 3 myapp copy -
     * //                                                       cursor ^
     * // return: false
     *
     * completion-find-current-positional-argument-name 2 myapp copy file
     * //                                                     cursor ^
     * // return: false
     *
     * completion-find-current-positional-argument-name 3 myapp copy -h
     * //                                                        cursor ^
     * // return: false
     * \endcode
     *
     * This function returns false if the cursor is not in the sub-command.
     *
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    bool isCursorAtSubCommandOption() const noexcept
    {
      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      if( cursor >= mParserResultInCommandLineIndexMap.commandLineArgumentCount() ){
        return false;
      }

      return mParserResultInCommandLineIndexMap.isCommandLineIndexAtSubCommandOption(cursor);

//       return mIsCursorAtSubCommandOption;
    }

    /*! \brief Returns the name of the "findCurrentPositionalArgumentName" query
     */
    static
    QString findCurrentPositionalArgumentNameString()
    {
      return QLatin1String("completion-find-current-positional-argument-name");
    }

    /*! \brief Check if \a parserResult is a valid Bash completion query
     *
     * A valid Bash completion query has at least 3 arguments:
     * - The query argument, findCurrentPositionalArgumentNameString()
     * - The cursor position in the command-line arguments, result of \a COMP_CWORD
     * - The executable (which comes from \a COMP_LINE)
     */
    static
    bool isValidBashCompletionQuery(const ParserResult & parserResult)
    {
      if( parserResult.positionalArgumentCount() < 3 ){
        return false;
      }
      if( parserResult.positionalArgumentAt(0) != findCurrentPositionalArgumentNameString() ){
        return false;
      }
      if( extractCursorInComplinePositionIndex(parserResult) < 0 ){
        return false;
      }

      return true;
    }

    /*! \brief Extract cursor position from \a parserResult
     *
     * Return -1 on failure
     *
     * \pre \a parserResult must have at least 2 positional arguments
     */
    static
    int extractCursorInComplinePositionIndex(const ParserResult & parserResult)
    {
      assert( parserResult.positionalArgumentCount() >= 2 );

      bool ok = false;
      const int index = parserResult.positionalArgumentAt(1).toInt(&ok);
      if( !ok ){
        return -1;
      }

      return index;
    }

    /*! \brief Get a parser result with from \a queryResult with the query command, cursor position and executable removed
     *
     * For example:
     * \code
     * completion-find-current-positional-argument-name 2 myapp some-file
     * \endcode
     * will return:
     * \code
     * some-file
     * \endcode
     *
     * All the rest (options, sub-command) are the same as \a queryResult
     *
     * \pre \a queryResult must have at least 3 positional arguments (for its main command)
     */
    static
    ParserResult parserResultWithoutQueryArguments(const ParserResult & queryResult)
    {
      assert( queryResult.mainCommand().positionalArgumentCount() >= 3 );
      ParserResult result;

      ParserResultCommand mainCommand;
      for( const auto & option : queryResult.mainCommand().options() ){
        mainCommand.addOption(option);
      }
      for( int i=3; i < queryResult.mainCommand().positionalArgumentCount(); ++i ){
        mainCommand.addPositionalArgument( queryResult.mainCommand().positionalArgumentAt(i) );
      }
      result.setMainCommand(mainCommand);

      result.setSubCommand( queryResult.subCommand() );

      std::cout << "\nResult without query arguments " << toDebugString(result).toStdString() << std::endl;
      return result;
    }

    /*! \brief Check if the cursor is at the position of a option in \a result
     */
    [[deprecated]]
    bool isCursorAtOption(const ParserResultCommand & command) const noexcept
    {
      const int cursor = mCursorInCompLinePositionIndex + 2;
      assert( cursor >= 0 );

      qDebug() << "cursor: " << cursor;
      
      if( cursor < command.positionalArgumentCount() ){
        qDebug() << "arg: " << command.positionalArgumentAt(cursor);
        return command.positionalArgumentAt(cursor).startsWith( QChar::fromLatin1('-') );
      }
      
      const int optionCursor = mCursorInCompLinePositionIndex - 1;
      
      qDebug() << "optionCursor: " << optionCursor;
      
      if(optionCursor < 0){
        return false;
      }
      if( optionCursor < command.optionCount() ){
        return true;
      }

      return false;
    }

   private:

    bool isCursorAtMainCommandOption(const ParserResult & result) const noexcept
    {
//       if(mParserDefinitionHasSubCommand){
//         if( cursorInCompLinePositionIndex() >= compLineSubCommandNamePositionIndex() ){
//           return -1;
//         }
//       }

      return isCursorAtOption( result.mainCommand() );
    }

    bool isCursorAtSubCommandOption(const ParserResult & result) const noexcept
    {
      return isCursorAtOption( result.subCommand() );
    }

    ParserResultInCommandLineIndexMap mParserResultInCommandLineIndexMap;
    int mCursorInCompLinePositionIndex;
    int mMainCommandPositionalArgumentsCount;
    int mMainCommandOptionsCount;
    bool mIsCursorAtMainCommandOption;
    bool mParserDefinitionHasSubCommand;
    bool mResultHasSubCommand;
    bool mIsCursorAtSubCommandOption;
    int mSubCommandPositionalArgumentsCount;
    int mSubCommandOptionsCount;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_QUERY_H
