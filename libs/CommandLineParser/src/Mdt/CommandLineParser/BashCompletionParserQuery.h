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
#include "CommandLine/Argument.h"
#include "CommandLine/Algorithm.h"
#include "CommandLine/CommandLineAlgorithm.h"
#include "CommandLine/ArgumentListAttributes.h"
#include "ParserDefinition.h"
#include "mdt_commandlineparser_export.h"
#include <boost/variant.hpp>
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \internal Visitor for isBashCompletionOption()
   */
  class MDT_COMMANDLINEPARSER_EXPORT IsBashCompletionOption : public boost::static_visitor<bool>
  {
   public:

    bool operator()(const CommandLine::Option &) const noexcept
    {
      return true;
    }

    bool operator()(const CommandLine::UnknownOption &) const noexcept
    {
      return true;
    }

    bool operator()(const CommandLine::OptionWithValue &) const noexcept
    {
      return true;
    }

    bool operator()(const CommandLine::SingleDash &) const noexcept
    {
      return true;
    }

    bool operator()(const CommandLine::DoubleDash &) const noexcept
    {
      return true;
    }

    template<typename T>
    bool operator()(const T &) const noexcept
    {
      return false;
    }
  };

  /*! \brief Check if \a argument is a option in the view of a Bash completion
   */
  inline
  bool isBashCompletionOption(const CommandLine::Argument & argument) noexcept
  {
    return boost::apply_visitor(IsBashCompletionOption(), argument);
  }

  /*! \brief Helper class to access a parser reult with Bash completion arguments
   *
   * This class is mostly not usefull to use this library.
   * handleBashCompletion() should be used, which cares about details exposed here.
   *
   * \section findCurrentArgument "findCurrentArgument"
   *
   * The completion script will call the executable as follow:
   * \code
   * "$executable" completion-find-current-argument $COMP_CWORD $COMP_LINE
   * \endcode
   *
   * After parsing, command line will contain at least 3 arguments:
   * - The query argument, findCurrentArgument()
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
   * "myapp" completion-find-current-argument 1 myapp
   * //                                      cursor:  ^
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

    /*! \brief Construct a query from \a commandLine and \a parserDefinition
     *
     * \pre \a commandLine must be a valid Bash completion query
     * \sa isValidBashCompletionQuery()
     * \warning \a commandLine and \a parserDefinition must be valid for the whole lifetime of this BashCompletionParserQuery
     */
    explicit BashCompletionParserQuery(const CommandLine::CommandLine & commandLine, const ParserDefinition & parserDefinition)
     : mParserDefinition(parserDefinition)
    {
      assert( isValidBashCompletionQuery(commandLine, parserDefinition) );

      /*
       * We are interested by the command-line part in the query:
       * "app" completion-find-current-argument 1 app
       *                                          ^
       */
      const auto commandLineArgumentListFirst = commandLine.argumentList().cbegin() + 3;
      mCompLineArgumentListAttributes.setArgumentList( commandLineArgumentListFirst, commandLine.argumentList().cend() );

      mCursorInCompLinePositionIndex = extractCursorInComplinePositionIndex(commandLine);
      assert( mCursorInCompLinePositionIndex >= 0 );

      mInCompLineSubCommandNameIndex = mCompLineArgumentListAttributes.subCommandNameIndex();
      if( compLineHasSubCommand() ){
        const QString subCommandName = Mdt::CommandLineParser::CommandLine::findSubCommandName(commandLine);
        mInDefinitionSubCommandIndex = mParserDefinition.findSubCommandIndexByName(subCommandName);
        assert( mInDefinitionSubCommandIndex >= 0 );
      }
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
    }

    /*! \brief Check if the parser definition has sub-command
     */
    bool parserDefinitionHasSubCommand() const noexcept
    {
      return mParserDefinition.hasSubCommands();
    }

    /*! \brief Check if the COMP_LINE has a sub-command
     *
     * Will return true if the COMP_LINE has a least a sub-command name
     *
     * \note if the command line has a sub-command name,
     * it is a known sub-command
     * (it exists in the parser definition).
     *
     * \sa isValidBashCompletionQuery()
     */
    bool compLineHasSubCommand() const noexcept
    {
      return mInCompLineSubCommandNameIndex > 0;
    }

    /*! \brief Get the sub-command name
     *
     * \pre The COMP_LINE must have a at least a sub-command name
     * \sa compLineHasSubCommand()
     */
    QString subCommandName() const noexcept
    {
      assert( compLineHasSubCommand() );
      assert( mInDefinitionSubCommandIndex >= 0 );

      return mParserDefinition.subCommandAt(mInDefinitionSubCommandIndex).name();
    }

    /*! \brief Check if the cursor is in the main command
     *
     * If the cursor is past the command-line,
     * returns true if the command-line has no sub-command name.
     *
     * If the cursor is in the command-line,
     * return true if it is before the sub-command name.
     */
    bool isCursorInMainCommand() const noexcept
    {
      if( isCursorPastTheCompLine() ){
        return !compLineHasSubCommand();
      }
      return mCompLineArgumentListAttributes.isCommandLineIndexInMainCommand( cursorInCompLinePositionIndex() );
    }

    /*! \brief Check if the cursor is at the sub-command name
     *
     * Returns true if the cursor is exactly at the sub-command name,
     * otherwise false.
     *
     * \note If the command line contains a partially typed sub-command name,
     * it will be a positional argument.
     *
     * Examples:
     * \code
     * completion-find-current-argument 1 app co
     * // return: false
     * // Why: co is not a known sub-command name, but a positional argument
     *
     * completion-find-current-argument 1 app copy
     * // return: true
     * // Why: copy is a known sub-command name
     * \endcode
     */
    bool isCursorAtSubCommandName() const noexcept
    {
      if( isCursorPastTheCompLine() ){
        return false;
      }
      return mCompLineArgumentListAttributes.isCommandLineIndexAtSubCommandName( cursorInCompLinePositionIndex() );
    }

    /*! \brief Check if the cursor is in the sub-command in the command line
     *
     * Return true if the cursor is past the sub-command name,
     * otherwise false.
     */
    bool isCursorInSubCommand() const noexcept
    {
      if( isCursorPastTheCompLine() ){
        return mCompLineArgumentListAttributes.isCommandLineIndexInSubCommand( cursorInCompLinePositionIndex() - 1 );
      }
      return mCompLineArgumentListAttributes.isCommandLineIndexInSubCommand( cursorInCompLinePositionIndex() );
    }

    /*! \brief Get the count of positional arguments in the main command
     */
    int mainCommandPositionalArgumentCount() const noexcept
    {
      return mCompLineArgumentListAttributes.findMainCommandPositionalArgumentCount(CommandLine::isPositionalArgument) - 1;
    }

    /*! \brief Get the count of positional arguments in the main command of the parser definition
     */
    int parserDefinitionMainCommandPositionalArgumentCount() const noexcept
    {
      return mParserDefinition.mainCommand().positionalArgumentCount();
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
    int cursorMainCommandPositionalArgumentIndexInDefinition() const noexcept
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

        const int commandLinePositionalArgumentCount = mainCommandPositionalArgumentCount();
        if( commandLinePositionalArgumentCount >= parserDefinitionMainCommandPositionalArgumentCount() ){
          return -1;
        }
        // We are after the command line
        return commandLinePositionalArgumentCount;
      }

      // The first positional argument in the command line is the executable
      const int index = mCompLineArgumentListAttributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(cursor, CommandLine::isPositionalArgument) - 1;

      return index;
    }

    /*! \brief Get the name of the positional argument the cursor is in the main command
     *
     * \pre the cursor must be at a positional argument in the main command
     *   that exists in the definition
     *  (i.e. cursorMainCommandPositionalArgumentIndexInDefinition() must return a index >= 0)
     */
    QString cursorMainCommandPositionalArgumentName() const noexcept
    {
      const int index = cursorMainCommandPositionalArgumentIndexInDefinition();
      assert( index >= 0 );
      assert( mParserDefinition.mainCommand().hasPositionalArgumentAt(index) );

      return mParserDefinition.mainCommand().positionalArgumentAt(index).name();
    }

    /*! \brief Check if \a compLineIndex refers to a option
     *
     * Examples:
     * \code
     * completion-find-current-argument x app -
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app -h
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app --
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app --overwrite-behavior
     * //                    compLineIndex==1 ^
     * // return: true
     *
     * completion-find-current-argument x app --overwrite-behavior k
     * //                                         compLineIndex==2 ^
     * // return: false
     * // Why: compLineIndex refers to a option value, not a option
     *
     * completion-find-current-argument x app --overwrite-behavior=keep
     * //                    compLineIndex==1 ^
     * // return: true
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
     * \pre \a compLineIndex must be at least at position 0 (the executable in the command-line)
     * \pre \a compLineIndex must not be past de COMP_LINE
     * \sa isCursorAtOption()
     */
    bool isCompLineIndexAtOption(int compLineIndex) const noexcept
    {
      assert( compLineIndex >= 0 );
      assert( !isCompLineIndexPastTheCompLine(compLineIndex) );

      return mCompLineArgumentListAttributes.isCommandLineIndexAtOption(compLineIndex, isBashCompletionOption);
    }

    /*! \brief Check if the cursor is at a option
     *
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     * \pre the cursor must not be past de COMP_LINE
     * \sa isCompLineIndexAtOption()
     * \sa isCursorPastTheCompLine()
     */
    bool isCursorAtOption() const noexcept
    {
      assert( !isCursorPastTheCompLine() );
      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      return isCompLineIndexAtOption(cursor);
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
     * \sa isCursorAtOptionValue()
     */
    bool isCompLineIndexAtOptionValue(int compLineIndex) const noexcept
    {
      assert( compLineIndex >= 0 );

      if(compLineIndex < 1){
        return false;
      }

      return mCompLineArgumentListAttributes.isCommandLineIndexAtOptionExpectingValue(compLineIndex-1);
    }

    /*! \brief Check if the cursor is at a option value
     *
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     * \sa isCompLineIndexAtOptionValue()
     */
    bool isCursorAtOptionValue() const noexcept
    {
      const int cursor = cursorInCompLinePositionIndex();
      assert( cursor >= 0 );

      return isCompLineIndexAtOptionValue(cursor);
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

    /*! \brief Get the count of positional arguments in the sub-command
     *
     * \pre The command line must have a sub-command
     * \sa compLineHasSubCommand()
     */
    int subCommandPositionalArgumentCount() const noexcept
    {
      assert( compLineHasSubCommand() );

      return mCompLineArgumentListAttributes.findSubCommandPositionalArgumentCount(CommandLine::isPositionalArgument);
    }

    /*! \brief Get the count of positional arguments in the sub-command of the parser definition
     *
     * \pre The command line must have a sub-command
     * \sa compLineHasSubCommand()
     */
    int parserDefinitionSubCommandPositionalArgumentCount() const noexcept
    {
      assert( compLineHasSubCommand() );
      assert( mInDefinitionSubCommandIndex >= 0 );

      return mParserDefinition.subCommandAt(mInDefinitionSubCommandIndex).positionalArgumentCount();
    }

    /*! \brief Get the cursor index mapped to the positional argument of the sub-command in the parser definition
     *
     * This can be used to get the positional argument in the parser definition.
     *
     * Note that this index will be past of the positional arguments
     * available in a command line most of the time, except if the user goes back:
     * \code
     * completion-find-current-argument 1 app
     * //                              cursor ^
     *
     * completion-find-current-argument 2 app copy some-file
     * //                                   cursor ^
     * \endcode
     *
     * Examples for a application that can take 1 positional argument,
     * then a sub-command that can take \a source and \a destination as positional arguments:
     * \code
     * completion-find-current-argument 1 app
     * //                              cursor ^
     * // Undefined, see preconditions
     *
     * completion-find-current-argument 2 app copy
     * //                                   cursor ^
     * // return: 0
     *
     * completion-find-current-argument 3 app copy file.txt
     * //                                            cursor ^
     * // return: 1
     *
     * completion-find-current-argument 2 app copy --verbose
     * //                                   cursor ^
     * // return: < 0
     * // Why: cursor is at the index of a option in the command line
     *
     * completion-find-current-argument 3 app copy --overwrite-behavior
     * //                                                        cursor ^
     * // return: < 0
     * // Why: cursor is at the index of a option value in the command line
     * \endcode
     *
     * \note This index should not be used to get a positional argument
     * in the command line, but in a parser definition.
     *
     * \pre The command line must have a sub-command
     * \sa compLineHasSubCommand()
     * \pre cursorInCompLinePositionIndex() must be at least at position 0 (the executable in the command-line)
     */
    int cursorSubCommandPositionalArgumentIndexInDefinition() const noexcept
    {
      assert( compLineHasSubCommand() );
      assert( mInDefinitionSubCommandIndex >= 0 );

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
        if( isCompLineIndexAtOptionValue(cursor) ){
          return -1;
        }

        const int commandLinePositionalArgumentCount = subCommandPositionalArgumentCount();

        if( commandLinePositionalArgumentCount >= parserDefinitionSubCommandPositionalArgumentCount() ){
          return -1;
        }
        // We are after the command line
        return commandLinePositionalArgumentCount;
      }

      const int index = mCompLineArgumentListAttributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(cursor, CommandLine::isPositionalArgument);

      return index;
    }

    /*! \brief Get the name of the positional argument the cursor is in the sub-command
     *
     * \pre The command line must have a sub-command
     * \sa compLineHasSubCommand()
     * \pre the cursor must be at a positional argument in the sub-command
     *  that exists in the definition
     *  (i.e. cursorSubCommandPositionalArgumentIndexInDefinition() must return a index >= 0)
     */
    QString cursorSubCommandPositionalArgumentName() const noexcept
    {
      assert( compLineHasSubCommand() );
      assert( mInDefinitionSubCommandIndex >= 0 );

      const int index = cursorSubCommandPositionalArgumentIndexInDefinition();
      assert( index >= 0 );
      assert( mParserDefinition.subCommandAt(mInDefinitionSubCommandIndex).hasPositionalArgumentAt(index) );

      return mParserDefinition.subCommandAt(mInDefinitionSubCommandIndex).positionalArgumentAt(index).name();
    }

    /*! \brief Check if the cursor is at a positional index of the sub-command in the parser definition
     *
     * \sa cursorSubCommandPositionalArgumentsIndexInDefinition()
     */
    bool isCursorAtSubCommandPositionalArgumentsIndexInDefinition() const noexcept
    {
      return cursorSubCommandPositionalArgumentIndexInDefinition() >= 0;
    }

    /*! \brief Returns the name of the "findCurrentArgument" query
     */
    static
    QString findCurrentArgumentString()
    {
      return QLatin1String("completion-find-current-argument");
    }

    /*! \brief Check if \a commandLine is a valid Bash completion query
     *
     * A valid Bash completion query has at least 4 arguments:
     * - The executable
     * - The query argument, findCurrentPositionalArgumentNameString()
     * - The cursor position in the command-line arguments, result of \a COMP_CWORD
     * - The executable (which comes from \a COMP_LINE)
     *
     * If \a commandLine has a sub-command name,
     * it must exist in \a parserDefinition
     */
    static
    bool isValidBashCompletionQuery(const CommandLine::CommandLine & commandLine, const ParserDefinition & parserDefinition)
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
      if( getPositionalArgumentValue( commandLine.argumentAt(1) ) != findCurrentArgumentString() ){
        return false;
      }
      const int cursor = extractCursorInComplinePositionIndex(commandLine);
      if( cursor < 0 ){
        return false;
      }
      if( cursor > compLineArgumentCount(commandLine) ){
        return false;
      }

      const QString subCommandName = Mdt::CommandLineParser::CommandLine::findSubCommandName(commandLine);
      if( !subCommandName.isEmpty() ){
        if( !parserDefinition.containsSubCommand(subCommandName) ){
          return false;
        }
      }

      return true;
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

   private:

    CommandLine::ArgumentListAttributes mCompLineArgumentListAttributes;
    const ParserDefinition & mParserDefinition;
    int mInCompLineSubCommandNameIndex;
    int mInDefinitionSubCommandIndex = -1;
    int mCursorInCompLinePositionIndex;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_QUERY_H
