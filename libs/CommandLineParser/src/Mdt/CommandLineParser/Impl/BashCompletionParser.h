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
#ifndef MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_PARSER_H
#define MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_PARSER_H

#include "../CommandLine/CommandLine.h"
#include "../ParserDefinition.h"
#include "../BashCompletionParserQuery.h"
#include "../BashCompletionParserCurrentArgument.h"
#include "../BashCompletionParserCurrentArgumentType.h"
#include <QLatin1String>
#include <QString>
#include <QLatin1Char>
#include <QStringBuilder>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace Impl{

  /*! \internal Find the current argument type in the command line for completion
   *
   * Examples:
   * \code
   * <...> 1 app -
   * //  cursor: ^
   * // return: Options
   *
   * <...> 1 app --
   * //  cursor: ^
   * // return: Options
   *
   * <...> 1 app -h
   * //  cursor: ^
   * // return: Options
   *
   * <...> 1 app --verbose
   * //  cursor: ^
   * // return: Options
   *
   * <...> 2 app --overwrite-behavior k
   * //                       cursor: ^
   * // return: OptionNameValue
   *
   * <...> 1 app copy
   * //  cursor: ^
   * // return: Commands
   * // Why: the cursor is at the sub-command name (no ambiguity)
   *
   * <...> 2 app copy -h
   * //       cursor: ^
   * // return: Options
   *
   * <...> 1 app fi
   * //  cursor: ^
   * // return:
   * // - PositionalArgumentName if the definition has at least 1 positional argument in its main command
   * // - Commands if the definition has no positional argument, but has sub-commands
   * // - Undefined if none of above is true
   *
   * <...> 2 app file.conf c
   * //            cursor: ^
   * // return:
   * // - PositionalArgumentName if the definition has at least 2 positional arguments in its main command
   * // - Commands if the definition has less than 2 positional argument, but has sub-commands
   * // - Undefined if none of above is true
   *
   * <...> 2 app file.txt /
   * //           cursor: ^
   * // return:
   * // - PositionalArgumentName if the definition has at least 2 positional arguments in its main command
   * // - Commands if the definition has less than 2 positional argument, but has sub-commands
   * // - Undefined if none of above is true
   *
   * <...> 2 app copy fi
   * //       cursor: ^
   * // return:
   * // - PositionalArgumentName if the definition has at least 1 positional argument in its copy sub-command
   * // - Undefined if none of above is true
   * \endcode
   *
   * \pre the cursor must be in the COMP_LINE, not past
   */
  static
  BashCompletionParserCurrentArgumentType completionFindCurrentArgumentTypeForCursorInTheCompLine(const BashCompletionParserQuery & query)
  {
    assert( !query.isCursorPastTheCompLine() );

    if( query.isCursorAtOption() ){
      return BashCompletionParserCurrentArgumentType::Options;
    }
    if( query.isCursorAtOptionValue() ){
      return BashCompletionParserCurrentArgumentType::OptionNameValue;
    }

    if( query.isCursorInMainCommand() ){
      const int commandLinePositionalArgumentCount = query.mainCommandPositionalArgumentCount();
      const int definitionPositionalArgumentCount = query.parserDefinitionMainCommandPositionalArgumentCount();

      if( commandLinePositionalArgumentCount > definitionPositionalArgumentCount ){
        if( query.parserDefinitionHasSubCommand() ){
          return BashCompletionParserCurrentArgumentType::Commands;
        }
        return BashCompletionParserCurrentArgumentType::Undefined;
      }
      return BashCompletionParserCurrentArgumentType::PositionalArgumentName;
    }

    if( query.isCursorAtSubCommandName() ){
      return BashCompletionParserCurrentArgumentType::Commands;
    }

    if( query.isCursorInSubCommand() ){
      const int commandLinePositionalArgumentCount = query.subCommandPositionalArgumentCount();
      const int definitionPositionalArgumentCount = query.parserDefinitionSubCommandPositionalArgumentCount();

      if( commandLinePositionalArgumentCount <= definitionPositionalArgumentCount ){
        return BashCompletionParserCurrentArgumentType::PositionalArgumentName;
      }
    }

    return BashCompletionParserCurrentArgumentType::Undefined;
  }

  /*! \internal Find the current argument in the command line for completion
   *
   * \pre the cursor must be in the COMP_LINE, not past
   */
  static
  BashCompletionParserCurrentArgument completionFindCurrentArgumentForCursorInTheCompLine(const BashCompletionParserQuery & query)
  {
    assert( !query.isCursorPastTheCompLine() );

    BashCompletionParserCurrentArgument currentArgument;

    currentArgument.setType( completionFindCurrentArgumentTypeForCursorInTheCompLine(query) );
    currentArgument.setInSubCommand( query.isCursorInSubCommand() );

    return currentArgument;
  }

  /*! \internal Find the current argument in the command line for completion
   *
   * Examples:
   * \code
   * <...> 1 app
   * //  cursor: ^
   * // return:
   * // - OptionsOrPositionalArgumentName if the definition has at least 1 positional argument in its main command
   * //    (if more positional arguments are available in the main command of the definition)
   * // - Options if the definition has no positional argument and no sub-commands
   * //    (if the cursor is past the available positional arguments in the main command of the definition)
   * // - OptionsOrCommands if the definition has no positional argument, but has sub-commands
   * //    (if the cursor is past the available positional arguments in the main command of the definition
   * //     and the definition has sub-commands)
   * // - Undefined if none of above is true
   *
   * <...> 2 app --verbose
   * //            cursor: ^
   * // return:
   * // - OptionsOrPositionalArgumentName if the definition has at least 1 positional argument in its main command
   * //    (if more positional arguments are available in the main command of the definition)
   * // - Options if the definition has no positional argument and no sub-commands
   * //    (if the cursor is past the available positional arguments in the main command of the definition)
   * // - OptionsOrCommands if the definition has no positional argument, but has sub-commands
   * //    (if the cursor is past the available positional arguments in the main command of the definition
   * //     and the definition has sub-commands)
   * // - Undefined if none of above is true
   *
   * <...> 2 app --overwrite-behavior
   * //                       cursor: ^
   * // return: OptionNameValue
   *
   * <...> 2 app file.txt
   * //           cursor: ^
   * // return:
   * // - OptionsOrPositionalArgumentName if the definition has at least 2 positional arguments in its main command
   * //    (if more positional arguments are available in the main command of the definition)
   * // - Options if the definition has 1 positional argument and no sub-commands
   * //    (if the cursor is past the available positional arguments in the main command of the definition)
   * // - OptionsOrCommands if the definition has 1 positional argument and has sub-commands
   * //    (if the cursor is past the available positional arguments in the main command of the definition
   * //     and the definition has sub-commands)
   * // - Undefined if none of above is true
   *
   * <...> 2 app co
   * //     cursor: ^
   * // return:
   * // - OptionsOrPositionalArgumentName if the definition has at least 2 positional arguments in its main command
   * //    (if more positional arguments are available in the main command of the definition)
   * // - Options if the definition has 1 positional argument and no sub-commands
   * //    (if the cursor is past the available positional arguments in the main command of the definition)
   * // - OptionsOrCommands if the definition has 1 positional argument and has sub-commands
   * //    (if the cursor is past the available positional arguments in the main command of the definition
   * //     and the definition has sub-commands)
   * // - Undefined if none of above is true
   *
   * <...> 2 app copy
   * //       cursor: ^
   * // return:
   * // - OptionsOrPositionalArgumentName if the definition has at least 1 positional argument in its sub-command
   * // - Options if the definition has no positional argument in its sub-command
   * // - Undefined if none of above is true
   * \endcode
   *
   * \pre the cursor must be past the COMP_LINE
   */
  static
  BashCompletionParserCurrentArgumentType completionFindCurrentArgumentTypeForCursorPastTheCompLine(const BashCompletionParserQuery & query)
  {
    assert( query.isCursorPastTheCompLine() );

    if( query.isCursorAtOptionValue() ){
      return BashCompletionParserCurrentArgumentType::OptionNameValue;
    }

    if( query.isCursorInMainCommand() ){
      const int commandLinePositionalArgumentCount = query.mainCommandPositionalArgumentCount() + 1;
      const int definitionPositionalArgumentCount = query.parserDefinitionMainCommandPositionalArgumentCount();

      if( commandLinePositionalArgumentCount > (definitionPositionalArgumentCount+1) ){
        return BashCompletionParserCurrentArgumentType::Undefined;
      }

      if( commandLinePositionalArgumentCount > definitionPositionalArgumentCount ){
        if( query.parserDefinitionHasSubCommand() ){
          return BashCompletionParserCurrentArgumentType::OptionsOrCommands;
        }
        return BashCompletionParserCurrentArgumentType::Options;
      }
      return BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName;
    }

    if( query.compLineHasSubCommand() ){
      const int commandLinePositionalArgumentCount = query.subCommandPositionalArgumentCount() + 1;
      const int definitionPositionalArgumentCount = query.parserDefinitionSubCommandPositionalArgumentCount();

      if( commandLinePositionalArgumentCount <= definitionPositionalArgumentCount ){
        return BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName;
      }
    }

    return BashCompletionParserCurrentArgumentType::Undefined;
  }

  /*! \internal Find the current argument in the command line for completion
   *
   * \pre the cursor must be past the COMP_LINE
   */
  static
  BashCompletionParserCurrentArgument completionFindCurrentArgumentForCursorPastTheCompLine(const BashCompletionParserQuery & query)
  {
    assert( query.isCursorPastTheCompLine() );

    BashCompletionParserCurrentArgument currentArgument;

    currentArgument.setType( completionFindCurrentArgumentTypeForCursorPastTheCompLine(query) );
    currentArgument.setInSubCommand( query.compLineHasSubCommand() );

    return currentArgument;
  }

  /*! \internal Find the current argument in the command line for completion
   *
   * \todo Should document in the public BashCompletionParser class
   *
   * The completion script will call the executable as follow:
   * \code
   * "$executable" completion-find-current-argument $COMP_CWORD $COMP_LINE
   * \endcode
   * Where:
   * - COMP_LINE: the current command line
   * - COMP_CWORD: the cursor in the command line (could be past the command line)
   *
   * \sa https://www.gnu.org/software/bash/manual/html_node/Bash-Variables.html
   *
   * After parsing, \a commandLine will contain at least 4 arguments:
   * - The executable name
   * - The query argument, BashCompletionParserQuery::findCurrentArgumentString()
   * - The cursor position in the command-line arguments, result of \a COMP_CWORD
   * - The executable name in \a COMP_LINE (which is this time of a positional argument type)
   *
   * For example, if the user types:
   * \code
   * app <TAB>
   * \endcode
   * the executable \a app will be called with those arguments:
   * \code
   * "app" completion-find-current-argument 1 app
   * //                                  cursor:  ^
   * \endcode
   *  Note that the cursor is at position 1, which is just after \a app .
   *
   * \section simpleApp Simple app
   *
   * Lets take a example of a application with this definition:
   * \code
   * app [options] <source> <destination>
   * \endcode
   * the possible options are:
   * - -h, --help : print the help
   * - --verbose
   * - --overwrite-behavior <behavior> : behavior can be keep, fail, overwrite
   *
   * Here are some possible completion cases:
   * \code
   * app completion-find-current-argument 1 app
   * //                                cursor:  ^
   * // return: options-or-source
   * // related in script: compgen -A file -W "-h --help --verbose --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 1 app -
   * //                                cursor:  ^
   * // return: options
   * // related in script: compgen -W "-h --help --verbose --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 1 app --
   * //                                cursor:  ^
   * // return: options
   * // related in script: compgen -W "-h --help --verbose --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 2 app --verbose
   * //                                          cursor:  ^
   * // return: options-or-source
   * // related in script: compgen -A file -W "-h --help --verbose --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 2 app --verbose fi
   * //                                          cursor:  ^
   * // return: source
   * // related in script: compgen -A file -- "$cur"
   *
   * app completion-find-current-argument 2 app --verbose -
   * //                                          cursor:  ^
   * // return: options
   * // related in script: compgen -W "-h --help --verbose --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 3 app --verbose --overwrite-behavior
   * //                                                               cursor:  ^
   * // return: option-overwrite-behavior-value
   * // related in script: compgen -W "keep fail overwrite" -- "$cur"
   *
   * app completion-find-current-argument 4 app --verbose --overwrite-behavior keep
   * //                                                                    cursor:  ^
   * // return: options-or-source
   * // related in script: compgen -A file -W "-h --help --verbose --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 5 app --verbose --overwrite-behavior keep file.txt
   * //                                                                             cursor:  ^
   * // return: options-or-destination
   * // related in script: compgen -A directory -W "-h --help --verbose --overwrite-behavior" -- "$cur"
   * \endcode
   *
   * \subsection note_About_ShortOptionsValues Note about short options with values
   *
   * Taking a option like:
   * - -f, --file
   *
   * if the user types the short option,
   * the short option name is returned.
   * This means that the completion script has to handle
   * both short and long option names:
   * \code
   * app completion-find-current-argument 1 app --file
   * //                                       cursor:  ^
   * // return: option-file-value
   * // related in script: compgen -A file -- "$cur"
   *
   * app completion-find-current-argument 1 app -f
   * //                                   cursor:  ^
   * // return: option-f-value
   * // related in script: compgen -A file -- "$cur"
   * \endcode
   *
   * \section appWithSubCommand App with sub-command
   *
   * Lets take a example of a application with this definition:
   * \code
   * app [options] <command> [options] <args>
   * \endcode
   *
   * The options in the scope of \a app are:
   * - -h, --help
   * - --verbose
   *
   * The possible commands are:
   * - copy
   * - cut
   *
   * For the example, we only care about the \a copy sub-command:
   * \code
   * app [options] copy [options] <source> <destination>
   * \endcode
   *
   * The options for the \a copy sub-command are:
   * - -h, --help
   * - --dereference
   * - --overwrite-behavior <behavior> : behavior can be keep, fail, overwrite
   *
   * Here are some possible completion cases:
   * \code
   * app completion-find-current-argument 1 app
   * //                                cursor:  ^
   * // return: options-or-commands
   * // related in script: compgen -W "-h --help --verbose copy cut" -- "$cur"
   *
   * app completion-find-current-argument 1 app -
   * //                                cursor:  ^
   * // return: options
   * // related in script: compgen -W "-h --help --verbose" -- "$cur"
   *
   * app completion-find-current-argument 2 app --verbose
   * //                                          cursor:  ^
   * // return: options-or-commands
   * // related in script: compgen -W "-h --help --verbose copy cut" -- "$cur"
   *
   * app completion-find-current-argument 1 app c
   * //                                cursor:  ^
   * // return: commands
   * // related in script: compgen -W "copy cut" -- "$cur"
   *
   * app completion-find-current-argument 1 app fi
   * //                                cursor:  ^
   * // return: commands
   * // related in script: compgen -W "copy cut" -- "$cur"
   *
   * app completion-find-current-argument 2 app copy
   * //                                     cursor:  ^
   * // return: copy-options-or-source
   * // related in script: compgen -A file -W "-h --help --dereference --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 2 app copy -
   * //                                     cursor:  ^
   * // return: copy-options
   * // related in script: compgen -W "-h --help --dereference --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 3 app copy --dereference
   * //                                                   cursor:  ^
   * // return: copy-options-or-source
   * // related in script: compgen -A file -W "-h --help --dereference --overwrite-behavior" -- "$cur"
   *
   * app completion-find-current-argument 2 app copy fi
   * //                                     cursor:  ^
   * // return: copy-source
   * // related in script: compgen -A file -- "$cur"
   *
   * app completion-find-current-argument 3 app copy file.txt
   * //                                              cursor:  ^
   * // return: copy-options-or-destination
   * // related in script: compgen -A directory -W "-h --help --dereference --overwrite-behavior" -- "$cur"
   * \endcode
   *
   *
   * \subsection appWithPositionalArgumentAndSubCommand App with positional arguments and sub-command
   *
   * Here is a other example that take a positional argument before the sub-command:
   * \code
   * app [options] <config> <command> [options] <args>
   * \endcode
   *
   * Note that \a config is mandatory (it's a positional argument, not a option).
   *
   * Here are some possible completion cases:
   * \code
   * app completion-find-current-argument 1 app
   * //                                cursor:  ^
   * // return: options-or-config
   * // related in script: compgen -A file -W "-h --help --verbose" -- "$cur"
   *
   * app completion-find-current-argument 1 app -
   * //                                cursor:  ^
   * // return: options
   * // related in script: compgen -W "-h --help --verbose" -- "$cur"
   *
   * app completion-find-current-argument 1 app c
   * //                                cursor:  ^
   * // return: config
   * // related in script: compgen -A file -- "$cur"
   *
   * app completion-find-current-argument 1 app fi
   * //                                cursor:  ^
   * // return: config
   * // related in script: compgen -A file -- "$cur"
   *
   * app completion-find-current-argument 2 app file.conf
   * //                                          cursor:  ^
   * // return: options-or-commands
   * // related in script: compgen -W "-h --help --verbose copy cut" -- "$cur"
   *
   * app completion-find-current-argument 2 app file.conf c
   * //                                          cursor:  ^
   * // return: commands
   * // related in script: compgen -W "copy cut" -- "$cur"
   * \endcode
   *
   * \note Current version will not try to find out if the command line is allready complete.
   * This would requiere to know how many positional arguments are possible (maybe more than in the parser definition ?).
   * For options, it would also be required to know how many times a specific option can be passed.
   *
   * \note The fact that any options or sub-commands are available is not handled here,
   * but in the generated completion script.
   * If this function returns, for example \a "options", and the parser definition has no options,
   * the *) (\a else) case will be triggered in the script.
   *
   * \pre \a commandLine must be a valid Bash completion query
   * \sa BashCompletionParserQuery::isValidBashCompletionQuery()
   *
   * \section references References
   *
   * \sa https://iridakos.com/programming/2018/03/01/bash-programmable-completion-tutorial
   * \sa https://www.gnu.org/software/bash/manual/html_node/Programmable-Completion.html#Programmable-Completion
   * \sa https://www.gnu.org/software/bash/manual/html_node/Bash-Variables.html
   */
  static
  QString completionFindCurrentArgument(const CommandLine::CommandLine & commandLine, const ParserDefinition & parserDefinition)
  {
    assert( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );

    BashCompletionParserQuery query(commandLine, parserDefinition);
    BashCompletionParserCurrentArgument currentArgument;

    if( query.isCursorPastTheCompLine() ){
      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(query);
    }else{
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(query);
    }

    if( currentArgument.isInSubCommand() ){
      const QString subCommandName = query.subCommandName();
      switch( currentArgument.type() ){
        case BashCompletionParserCurrentArgumentType::Options:
          return subCommandName % QLatin1String("-options");
        case BashCompletionParserCurrentArgumentType::OptionNameValue:
          return subCommandName % QLatin1String("-option-") % query.optionNameRelatedToCurrentOptionValue() % QLatin1String("-value");
        case BashCompletionParserCurrentArgumentType::PositionalArgumentName:
          return subCommandName % QLatin1Char('-') % query.cursorSubCommandPositionalArgumentName();
        case BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName:
          return subCommandName % QLatin1String("-options-or-") % query.cursorSubCommandPositionalArgumentName();
        case BashCompletionParserCurrentArgumentType::Commands:
        case BashCompletionParserCurrentArgumentType::OptionsOrCommands:
        case BashCompletionParserCurrentArgumentType::Undefined:
          return QString();
      }
    }else{
      switch( currentArgument.type() ){
        case BashCompletionParserCurrentArgumentType::Options:
          return QLatin1String("options");
        case BashCompletionParserCurrentArgumentType::OptionNameValue:
          return QLatin1String("option-") % query.optionNameRelatedToCurrentOptionValue() % QLatin1String("-value");
        case BashCompletionParserCurrentArgumentType::PositionalArgumentName:
          return query.cursorMainCommandPositionalArgumentName();
        case BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName:
          return QLatin1String("options-or-") % query.cursorMainCommandPositionalArgumentName();
        case BashCompletionParserCurrentArgumentType::Commands:
          return QLatin1String("commands");
        case BashCompletionParserCurrentArgumentType::OptionsOrCommands:
          return QLatin1String("options-or-commands");
        case BashCompletionParserCurrentArgumentType::Undefined:
          return QString();
      }
    }

    return QString();
  }

}}} // namespace Mdt{ namespace CommandLineParser{ namespace Impl{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_PARSER_H
