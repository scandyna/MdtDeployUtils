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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_QUERY_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_QUERY_H

#include "CommandLine/CommandLine.h"
#include "CommandLine/Algorithm.h"
#include "CommandLine/CommandLineAlgorithm.h"
#include "CommandLine/ArgumentListAttributes.h"

#include "ParserResult.h"
#include "ParserDefinition.h"

#include "ParserResultInCommandLineIndexMap.h"

#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <cassert>

#include <QDebug>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Helper class to access a parser reult with Bash completion arguments
   *
   * This class is mostly not usefull to use this library.
   * handleBashCompletion() should be used, which cares about details exposed here.
   *
   * \section findCurrentPositionalArgumentName "findCurrentPositionalArgumentName"
   *
   * \todo Should be renamed like: findCurrentArgument or findCurrentArgumentTypeOrName
   * \code
   * completion-find-current-argument
   *
   * completion-find-current-argument-action
   *
   * // maybe:
   * completion-find-current-argument-type-or-name
   * \endcode
   *
   * Also, once a good name has been found,
   * rename this class.
   *
   * In script:
   * \code
   * local currentArgument=$("$executable" completion-find-current-argument $COMP_CWORD $COMP_LINE)
   *
   * case $currentArgument in
   *
   *   options)
   *     ...
   *
   *   command)
   *     ...
   *
   *   copy-shared-libraries-target-depends-on-options)
   *     ...
   *
   *   copy-shared-libraries-target-depends-on-destination)
   *     ...
   * \endcode
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
      assert( isValidBashCompletionQuery(parserResult) );

      mCursorInCompLinePositionIndex = extractCursorInComplinePositionIndex(parserResult);
      assert( mCursorInCompLinePositionIndex >= 0 );

      mParserDefinitionHasSubCommand = parserDefinition.hasSubCommands();
      mParserDefinitionMainCommandPositionalArgumentsCount = parserDefinition.mainCommand().positionalArgumentCount();

      mResultHasSubCommand = parserResult.hasSubCommand();

      mSubCommandPositionalArgumentsCount = parserResult.subCommand().positionalArgumentCount();
    }

    /*! \brief Construct a query from \a commandLine and \a parserDefinition
     *
     * \pre \a commandLine must be a valid Bash completion query
     * \sa isValidBashCompletionQuery()
     */
    explicit BashCompletionParserQuery(const CommandLine::CommandLine & commandLine, const ParserDefinition & parserDefinition)
     : mParserResultInCommandLineIndexMap( ParserResult() )
    {
      assert( isValidBashCompletionQuery(commandLine) );

      /*
       * We are interested by the command-line part in the query:
       * "myapp" completion-find-current-positional-argument-name 1 myapp
       *                                                            ^
       */
      const auto commandLineArgumentListFirst = commandLine.argumentList().cbegin() + 3;
        mCompLineArgumentListAttributes.setArgumentList( commandLineArgumentListFirst, commandLine.argumentList().cend() );

      
      mCursorInCompLinePositionIndex = extractCursorInComplinePositionIndex(commandLine);
      assert( mCursorInCompLinePositionIndex >= 0 );

      mParserDefinitionHasSubCommand = parserDefinition.hasSubCommands();
      mParserDefinitionMainCommandPositionalArgumentsCount = parserDefinition.mainCommand().positionalArgumentCount();
      
      mInCompLineSubCommandNameIndex = mCompLineArgumentListAttributes.subCommandNameIndex();
//       mInCompLineSubCommandNameIndex = CommandLine::findSubCommandNameArgumentIndex(commandLine) - 3;
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

    /*! \brief Get the count of arguments in the command line
     *
     * Returns the count of arguments in the command line, \a COMP_LINE .
     */
    int compLineArgumentCount() const noexcept
    {
      return mCompLineArgumentListAttributes.commandLineArgumentCount();
    }

    /*! \brief Get the cursor position in the command line
     */
    int cursorInCompLinePositionIndex() const noexcept
    {
      return mCursorInCompLinePositionIndex;
    }

    /*! \brief Check if \a compLineIndex is past the command line
     *
     * Returns true if \a compLineIndex
     * is after the command line, \a COMP_LINE ,
     * otherwise false.
     */
    bool isCompLineIndexPastTheCompLine(int compLineIndex) const noexcept
    {
      return compLineIndex >= compLineArgumentCount();
    }

    /*! \brief Check if the cursor is past the command line
     *
     * Return true if the cursor, \a COMP_CWORD ,
     * is after the command line, \a COMP_LINE ,
     * otherwise false.
     */
    bool isCursorPastTheCompLine() const noexcept
    {
      return isCompLineIndexPastTheCompLine( cursorInCompLinePositionIndex() );
//       return cursorInCompLinePositionIndex() >= compLineArgumentCount();
    }

    /*! \brief Check if the COMP_LINE has a sub-command
     *
     * Will return true if the COMP_LINE has a least a sub-command name
     */
    bool compLineHasSubCommand() const noexcept
    {
      return mInCompLineSubCommandNameIndex > 0;
    }

    /*! \brief Check if the command line can be either a main command positional argument or a sub-command name
     *
     * Returns true if the command line is ambiguious
     * because its last, or next, argument could be a positional argument (for the main command),
     * or the sub-command name position.
     *
     * If the parser definition does not have sub-command,
     * this ambiguity does not exist, and this method returns false.
     *
     * If the parser definition does not have positional arguments for its main command,
     * this ambiguity does not exist, and this method returns false.
     *
     * Examples for a application that can take 1 argument for the main command,
     * and has a copy sub-command:
     * \code
     * completion-find-current-argument 1 myapp
     * // return: true
     * // Why: after myapp, we could have a positional argument or a sub-command
     *
     * completion-find-current-argument 1 myapp co
     * // return: true
     * // Why: 'co' could be a positional argument or a partially typed sub-command name
     * // Note: the possible sub-commands the user can type will be handled in the completion script, we not do it here
     *
     * completion-find-current-argument 1 myapp copy
     * // return: false
     * // Why: 'copy' is clearly a known sub-command
     * \endcode
     */
    bool compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() const noexcept
    {
      if( !mParserDefinitionHasSubCommand ){
        return false;
      }
      if( mParserDefinitionMainCommandPositionalArgumentsCount < 1 ){
        return false;
      }
      
      if( compLineHasSubCommand() ){
        return false;
      }
      
      /// \todo has no sense
      if(mResultHasSubCommand){
        return false;
      }

      return true;
    }

    /*! \brief Get the index for which the cursor would be at the sub-command name position in the command-line
     *
     * If the parser definition has no sub-command,
     * this method will allways return invalid index (value < 0).
     *
     * Examples for a application that can take 1 argument for the main command,
     * and has a copy sub-command that can take \a source and \a destination as positional arguments:
     * \todo clarify undefined cases (old behavior is still documented, what is true?)
     * \code
     * completion-find-current-positional-argument-name x myapp
     * // Undefined
     * // Why: after myapp, we could have a positional argument or a sub-command
     * 
     * // return: 1
     * // Why: after myapp, we could have a positional argument or a sub-command
     * //      choice: 1 will be the index of the sub-command name
     *
     * completion-find-current-positional-argument-name x myapp copy
     * // return: 1
     * // Why: 1 is the index of a known sub-command name
     *
     * completion-find-current-positional-argument-name x myapp arg
     * // Undefined
     * // Why: 1 could be the index of a positional argument or a partially typed sub-command name
     * 
     * // return: 1
     * // Why: 1 could be the index of a positional argument or a partially typed sub-command name
     * //      choice: 1 will be the index of the sub-command name
     *
     * completion-find-current-positional-argument-name x myapp -h copy
     * // return: 2
     * // Why: 2 is the index of a known sub-command name
     *
     * completion-find-current-positional-argument-name x myapp arg1 copy
     * // return: 2
     * // Why: 2 is the index of a known sub-command name
     *
     * completion-find-current-positional-argument-name x myapp arg1 unknown
     * // Undefined
     * // Why: 2 could be the index of a known sub-command name while the user is typing
     * 
     * // return: 2
     * // Why: 2 could be the index of a known sub-command name while the user is typing
     * // Note: the possible sub-commands the user can type will be handled in the completion script, we not do it here
     *
     * completion-find-current-positional-argument-name x myapp arg1 copy file.txt
     * // return: 2
     * // Why: 2 is the index of a known sub-command name
     * \endcode
     *
     * \pre The command line must not have the ambiguity about positional arguments or command
     * \sa compLineCouldBeMainCommandPositionalArgumentOrSubCommandName()
     */
    int compLineSubCommandNamePositionIndex() const noexcept
    {
      assert( !compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );

      if( !mParserDefinitionHasSubCommand ){
        return -1;
      }
      return mCompLineArgumentListAttributes.subCommandNameIndex();
//       if( mResultHasSubCommand ){
//         return 1 + mainCommandPositionalArgumentsCount() + mParserResultInCommandLineIndexMap.mainCommandOptionCount();
//       }
//       return 1 + mParserResultInCommandLineIndexMap.mainCommandOptionCount();
    }

    /*! \brief Check if the cursor is in the sub-command in the command line
     *
     * Return true if the cursor is past the sub-command name,
     * otherwise false.
     */
//     bool isCursorInSubCommand() const noexcept
//     {
//     }

    /*! \brief Check if the cursor is in the sub-command in the command line
     *
     * \sa compLineSubCommandNamePositionIndex()
     * \pre The command line must not have the ambiguity about positional arguments or command
     * \sa compLineCouldBeMainCommandPositionalArgumentOrSubCommandName()
     */
    bool isCursorInSubCommand() const noexcept
    {
      assert( !compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );

//       return mCompLineArgumentListAttributes.isCommandLineIndexInSubCommand( cursorInCompLinePositionIndex() );
      const int subCommandIndex = compLineSubCommandNamePositionIndex();
      if(subCommandIndex < 0){
        return false;
      }
      return cursorInCompLinePositionIndex() >= subCommandIndex;
    }

    /*! \brief Get the count of positional arguments in the main command
     */
    [[deprecated]]
    int mainCommandPositionalArgumentsCount() const noexcept
    {
      return mParserResultInCommandLineIndexMap.mainCommandPositionalArgumentCount();
    }

    /*! \brief Get the command line cursor mapped to the positional argument index in the parser definition for the main command
     *
     * Returns the command line cursor mapped to the index of a positional argument
     * in the main command of the parser definition,
     * if it exists (in the definition),
     * otherwise a value < 0.
     *
     * This can be used to get the positional argument in the parser definition.
     *
     * For the following examples, it is assumed that the parser definition
     * has enough positional arguments if not specified else.
     *
     * Note that this index will be past of the positional arguments
     * available in the command line most of the time, except if the user goes back:
     * \code
     * completion-find-current-argument 1 myapp
     * //                                cursor ^
     * // return: 0
     *
     * completion-find-current-argument 2 myapp some-file
     * //                                          cursor ^
     * // return: 1
     * \endcode
     *
     * Here are some examples:
     * \code
     * completion-find-current-argument 1 myapp
     * //                                cursor ^
     * // return: 0
     * // Why: cursor could be at positional argument or at sub-command name index
     *
     * completion-find-current-argument 1 myapp arg1
     * //                                cursor ^
     * // return: 0 if the definition accepts at least 1 positional argument (in the main command), otherwise value < 0
     * // Why: cursor could be at positional argument or at sub-command name index (partially typed sub-command name)
     *
     * completion-find-current-argument 2 myapp arg1
     * //                                     cursor ^
     * // return: 1 if the definition accepts at least 2 positional arguments (in the main command), otherwise value < 0
     * // Why: cursor could be at positional argument or at sub-command name index (partially typed sub-command name)
     *
     * completion-find-current-argument 1 myapp --verbose
     * //                                cursor ^
     * // return: value < 0
     * // Why: cursor is at the index of a option in the command line
     *
     * completion-find-current-argument 2 myapp --verbose
     * //                                          cursor ^
     * // return: 0
     * // Why: cursor could be at positional argument or at sub-command name index
     *
     * completion-find-current-argument 2 myapp --overwrite-behavior
     * //                                                     cursor ^
     * // return: value < 0
     * // Why: cursor is at the index of a option value in the command line
     *
     * completion-find-current-argument 2 myapp copy
     * //                                     cursor ^
     * // return: value < 0
     * // Why: cursor is after sub-command name index
     *
     * completion-find-current-argument 1 myapp copy
     * //                                cursor ^
     * // return: value < 0
     * // Why: cursor is at sub-command name index
     *
     * completion-find-current-argument 1 myapp arg1 copy
     * //                                cursor ^
     * // return: 0
     * // Why: cursor is before sub-command name index
     *
     * completion-find-current-argument 2 myapp arg1 copy
     * //                                     cursor ^
     * // return: value < 0
     * // Why: cursor is at sub-command name index
     *
     * completion-find-current-argument 3 myapp arg1 copy
     * //                                          cursor ^
     * // return: value < 0
     * // Why: cursor is after sub-command name index
     * \endcode
     *
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    int cursorMainCommandPositionalArgumentsIndexInDefinition() const noexcept
    {
      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      if(cursor == 0){
        return -1;
      }

      if( isCursorPastTheCompLine() ){
        /*
         * If the previous argument in the command line is a option,
         * check if this option accepts a value
         */
        if( isCompLineIndexAtMainCommandOptionValue(cursor) ){
          return -1;
        }
        
        qDebug() << "Cursor past the command line";
        // The first positional argument in the command line is the executable
        const int commandLinePositionalArgumentCount = mCompLineArgumentListAttributes.findMainCommandPositionalArgumentCount() - 1;
        
        qDebug() << " def main cmd positional arguments     : " << mParserDefinitionMainCommandPositionalArgumentsCount;
        qDebug() << " cmd line main cmd positional arguments: " << commandLinePositionalArgumentCount;
        
        if( commandLinePositionalArgumentCount >= mParserDefinitionMainCommandPositionalArgumentsCount ){
          return -1;
        }
        // We are after the command line
        return commandLinePositionalArgumentCount;

        
//         return mParserDefinitionMainCommandPositionalArgumentsCount - mCompLineArgumentListAttributes.findMainCommandPositionalArgumentCount();
      }


      /** \todo Here, the first argument of COMP_LINE is the app,
       * which is seen as a positional argument (subset)
       */
      
      
      /*
       * The cursor, COMP_CWORD, could be past the end of the command line.
       * This is maybe valid in the parser definition,
       * so get the positional argument index from the command line argument attributes by passing cursor-1,
       * then return index+1
       */
      
      // The first positional argument in the command line is the executable
      const int index = mCompLineArgumentListAttributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(cursor) - 1;

      /*
       * The cursor in the command-line could be after the parser result.
       * This is maybe valid in the parser definition, so
       * get the positional argument index from the map by passing cursor-1,
       * then return index+1
       */
//       const int index = mParserResultInCommandLineIndexMap.mainCommandPositionalArgumentIndexFromCommandLineIndex(cursor-1) + 1;

//       if( compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() ){
//         if(index >= mParserDefinitionMainCommandPositionalArgumentsCount){
//           return -1;
//         }
//       }else{
//         if( isCursorInSubCommand() ){
//           return -1;
//         }
//       }

      return index;
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
    [[deprecated]]
    bool isCursorAtMainCommandPositionalArgumentsIndexInDefinition() const noexcept
    {
      return cursorMainCommandPositionalArgumentsIndexInDefinition() >= 0;
    }

    /*! \brief Check if \a compLineIndex refers to a option
     *
     * Examples:
     * \code
     * completion-find-current-argument x app
     * //                    compLineIndex==1 ^
     * // return: false
     *
     * completion-find-current-argument x app -
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app -h
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app -h
     * //                       compLineIndex==2 ^
     * // return: false
     *
     * completion-find-current-argument x app --
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app --overwrite-behavior
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app --overwrite-behavior
     * //                                         compLineIndex==2 ^
     * // return: false
     * // Why: compLineIndex refers to a option value, not a option
     *
     * completion-find-current-argument x app --overwrite-behavior=keep
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app -
     * //                      compLineIndex==2 ^
     * // return: false
     *
     * completion-find-current-argument x app file.txt
     * //                    compLineIndex==1 ^
     * // return: false
     *
     * completion-find-current-argument x app copy -h
     * //                         compLineIndex==2 ^
     * // return: true
     * \endcode
     *
     * \sa isCursorAtOption()
     * \pre \a compLineIndex must be at least at position 0 (the executable in the command-line)
     */
    bool isCompLineIndexAtOption(int compLineIndex) const noexcept
    {
      assert( compLineIndex >= 0 );

      if( isCompLineIndexPastTheCompLine(compLineIndex) ){
        return false;
      }

      return mCompLineArgumentListAttributes.isCommandLineIndexAtOption(compLineIndex, CommandLine::isOptionOrOptionWithValueOrAnyDash);
    }

    /*! \brief Check if \a compLineIndex refers to a option value
     *
     * Examples:
     * \code
     * completion-find-current-argument x app --overwrite-behavior
     * //                    compLineIndex==1 ^
     * // return: false
     *
     * completion-find-current-argument x app --overwrite-behavior
     * //                                         compLineIndex==2 ^
     * // return: true
     *
     * completion-find-current-argument x app --overwrite-behavior keep
     * //                                         compLineIndex==2 ^
     * // return: true
     *
     * completion-find-current-argument x app --overwrite-behavior=keep
     * //                    compLineIndex==1 ^
     * // return: false
     * // Why: option=value is one item in the command line.
     * //      By default, Bash uses space as separator
     *
     * completion-find-current-argument x app --overwrite-behavior keep
     * //                                              compLineIndex==3 ^
     * // return: false
     *
     * completion-find-current-argument x app copy --overwrite-behavior
     * //                                              compLineIndex==3 ^
     * // return: true
     * \endcode
     *
     * \pre \a compLineIndex must be at least at position 0 (the executable in the command-line)
     */
    bool isCompLineIndexAtOptionValue(int compLineIndex) const noexcept
    {
      assert( compLineIndex >= 0 );

      if(compLineIndex < 1){
        return false;
      }

      return mCompLineArgumentListAttributes.isCommandLineIndexAtOptionExpectingValue(compLineIndex-1);
    }

    /*! \brief Get the name of the current option expecting a value
     *
     * \pre cursorInCompLinePositionIndex() must be at a option expecting a value,
     *  and the previous argument must be a option
     */
    QString optionNameRelatedToCurrentOptionValue() const
    {
      const int cursor = cursorInCompLinePositionIndex();

      assert( isCompLineIndexAtOptionValue(cursor) );
      assert( cursor >= 2 );
      assert( isCompLineIndexAtOption(cursor-1) );

      return mCompLineArgumentListAttributes.optionNameAtCommandLineIndex(cursor-1);
    }

    /*! \brief Check if \a compLineIndex refers to a option in the main command
     *
     * Returns the same results as isCompLineIndexAtOption()
     * as long as \a compLineIndex refers to the main command,
     * otherwise false.
     *
     * Examples:
     * \code
     * completion-find-current-argument x app -h
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app copy -h
     * //                         compLineIndex==2 ^
     * // return: false
     * \endcode
     *
     * \sa isCursorAtMainCommandOption()
     * \sa isCompLineIndexAtOption()
     * \pre \a compLineIndex must be at least at position 0 (the executable in the command-line)
     */
    bool isCompLineIndexAtMainCommandOption(int compLineIndex) const noexcept
    {
      assert( compLineIndex >= 0 );

      if( isCompLineIndexPastTheCompLine(compLineIndex) ){
        return false;
      }

      if( mCompLineArgumentListAttributes.isCommandLineIndexInMainCommand(compLineIndex) ){
        return isCompLineIndexAtOption(compLineIndex);
      }

      return false;
      
//       if( isCompLineIndexPastTheCompLine(compLineIndex) ){
//         return false;
//       }

//       return mCompLineArgumentListAttributes.isCommandLineIndexAtMainCommandOption(compLineIndex, CommandLine::isOptionOrOptionWithValueOrAnyDash);
    }

    /*! \brief Check if the cursor is at a option of the main command
     *
     * Examples:
     * \code
     * completion-find-current-argument 1 myapp
     * //                                cursor ^
     * // return: false
     *
     * completion-find-current-argument 1 myapp -
     * //                                cursor ^
     * // return: true
     *
     * completion-find-current-argument 1 myapp -h
     * //                                cursor ^
     * // return: true
     *
     * completion-find-current-argument 1 myapp --
     * //                                cursor ^
     * // return: true
     *
     * completion-find-current-argument 2 myapp -
     * //                                  cursor ^
     * // return: false
     *
     * completion-find-current-argument 1 myapp file
     * //                                cursor ^
     * // return: false
     *
     * completion-find-current-argument 2 myapp -h
     * //                                   cursor ^
     * // return: false
     * \endcode
     *
     * For application with sub-command,
     * this function returns false if the cursor is in the sub-command.
     *
     * \sa isCompLineIndexAtMainCommandOption()
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    bool isCursorAtMainCommandOption() const noexcept
    {
      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      return isCompLineIndexAtMainCommandOption(cursor);
//       if( cursor >= mParserResultInCommandLineIndexMap.commandLineArgumentCount() ){
//         return false;
//       }
// 
//       return mParserResultInCommandLineIndexMap.isCommandLineIndexAtMainCommandOption(cursor);
    }

    /*! \brief Check if \a compLineIndex refers to a option value in the main command
     *
     * Returns the same results as isCompLineIndexAtOptionValue()
     * as long as \a compLineIndex refers to the main command,
     * otherwise false.
     *
     * Examples:
     * \code
     * completion-find-current-argument x app --overwrite-behavior
     * //                                         compLineIndex==2 ^
     * // return: true
     *
     * completion-find-current-argument x app copy --overwrite-behavior
     * //                                              compLineIndex==3 ^
     * // return: false
     * // Why: compLineIndex is in the sub-command
     * \endcode
     *
     * \sa isCompLineIndexAtOptionValue()
     * \pre \a compLineIndex must be at least at position 0 (the executable in the command-line)
     */
    bool isCompLineIndexAtMainCommandOptionValue(int compLineIndex) const noexcept
    {
      assert( compLineIndex >= 0 );

      if(compLineIndex < 1){
        return false;
      }

      if( mCompLineArgumentListAttributes.isCommandLineIndexInMainCommand(compLineIndex-1) ){
        return isCompLineIndexAtOptionValue(compLineIndex);
      }

      return false;

//       if(compLineIndex < 1){
//         return false;
//       }
// 
//       return mCompLineArgumentListAttributes.isCommandLineIndexAtMainCommandOptionExpectingValue(compLineIndex-1);
      //if( isCompLineIndexPastTheCompLine(compLineIndex) ){
//         if( !isCompLineIndexAtMainCommandOption(compLineIndex-1) ){
//           return false;
//         }
        //
      //}
    }

    /*! \brief Check if the cursor is at a option value of the main command
     *
     * \sa isCompLineIndexAtMainCommandOptionValue()
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    bool isCursorAtMainCommandOptionValue() const noexcept
    {
      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      return isCompLineIndexAtMainCommandOptionValue(cursor);
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
     * // return: false
     * // Why: at cursor position we can expect a positional argument or a sub-command name
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
     * // Why: at cursor position can be a positional argument or a partially typed sub-command name
     * // Note: the possible sub-commands the user can type will be handled in the completion script, we not do it here
     * \endcode
     *
     * \sa compLineCouldBeMainCommandPositionalArgumentOrSubCommandName()
     * \sa compLineSubCommandNamePositionIndex()
     */
    bool isCursorAtSubCommandNameIndexInDefinition() const noexcept
    {
      if( compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() ){
        return !isCursorAtMainCommandPositionalArgumentsIndexInDefinition();
      }
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
      const int cursor = extractCursorInComplinePositionIndex(parserResult);
      if( cursor < 0 ){
        return false;
      }
      if( cursor > ParserResultInCommandLineIndexMap::commandLineArgumentCount( parserResultWithoutQueryArguments(parserResult) ) ){
        return false;
      }

      return true;
    }

    /*! \brief Check if \a commandLine is a valid Bash completion query
     *
     * A valid Bash completion query has at least 4 arguments:
     * - The executable
     * - The query argument, findCurrentPositionalArgumentNameString()
     * - The cursor position in the command-line arguments, result of \a COMP_CWORD
     * - The executable (which comes from \a COMP_LINE)
     */
    static
    bool isValidBashCompletionQuery(const CommandLine::CommandLine & commandLine)
    {
      using Mdt::CommandLineParser::CommandLine::getPositionalArgumentValue;

      if( commandLine.argumentCount() < 4 ){
        return false;
      }
      if( !commandLine.argumentAtIsPositionalArgument(1) ){
        return false;
      }
      if( !commandLine.argumentAtIsPositionalArgument(2) ){
        return false;
      }
      if( !commandLine.argumentAtIsPositionalArgument(3) ){
        return false;
      }
      if( getPositionalArgumentValue( commandLine.argumentAt(1) ) != findCurrentPositionalArgumentNameString() ){
        return false;
      }
      const int cursor = extractCursorInComplinePositionIndex(commandLine);
      if( cursor < 0 ){
        return false;
      }
      if( cursor > compLineArgumentCount(commandLine) ){
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
      const int cursor = parserResult.positionalArgumentAt(1).toInt(&ok);
      if( !ok ){
        return -1;
      }

      return cursor;
    }

    /*! \brief Extract cursor position from \a commandLine
     *
     * Return -1 on failure
     *
     * \pre \a commandLine must have a positional argument at index 2
     * \code
     * myapp completion-find-current-argument 3 ...
     * \endcode
     */
    static
    int extractCursorInComplinePositionIndex(const CommandLine::CommandLine & commandLine)
    {
      using Mdt::CommandLineParser::CommandLine::getPositionalArgumentValue;

      assert( commandLine.argumentCount() >= 2 );
      assert( commandLine.argumentAtIsPositionalArgument(2) );

      bool ok = false;
      const int cursor = getPositionalArgumentValue( commandLine.argumentAt(2) ).toInt(&ok);
      if( !ok ){
        return -1;
      }

      return cursor;
    }

    /*! \brief Get the count of arguments for the COMP_LINE part of \a commandLine
     *
     * For example:
     * \code
     * myapp completion-find-current-argument 1 myapp file.txt
     * \endcode
     * COMP_LINE has 2 argument (myapp and file.txt)
     */
    static
    int compLineArgumentCount(const CommandLine::CommandLine & commandLine) noexcept
    {
      return commandLine.argumentCount() - 3;
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

      return result;
    }

   private:

    ParserResultInCommandLineIndexMap mParserResultInCommandLineIndexMap;
    
    CommandLine::ArgumentListAttributes mCompLineArgumentListAttributes;
    
    int mInCompLineSubCommandNameIndex;
    int mCursorInCompLinePositionIndex;
    int mParserDefinitionMainCommandPositionalArgumentsCount;
    bool mParserDefinitionHasSubCommand;
    
    bool mResultHasSubCommand;
    
    int mSubCommandPositionalArgumentsCount;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_QUERY_H
