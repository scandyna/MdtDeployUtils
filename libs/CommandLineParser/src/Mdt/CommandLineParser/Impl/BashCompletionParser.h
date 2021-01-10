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

#include "../ParserResult.h"
#include "../ParserDefinition.h"
#include "../BashCompletionParserQuery.h"
#include <QLatin1String>
#include <QString>
#include <QLatin1Char>
#include <QStringBuilder>
#include <cassert>

// #include "../ParserResultDebug.h"
// #include <QDebug>
// #include <iostream>

namespace Mdt{ namespace CommandLineParser{ namespace Impl{

//   [[deprecated]]
//   static
//   QString completionFindCurrentPositionalArgumentNameString()
//   {
//     return QLatin1String("completion-find-current-positional-argument-name");
//   }

  /*! \internal Find the name of the current positional argument in \a result
   *
   * Possible commands.
   *
   * \sa BashCompletionParserQuery
   *
   * For a simple application like:
   * \code
   * myapp <source> <destination>
   * \endcode
   *
   * we could have:
   * \code
   * completion-find-current-positional-argument-name 1 myapp
   * // return source
   *
   * completion-find-current-positional-argument-name 2 myapp some-file
   * // return destination
   * \endcode
   *
   * For a application with subCommands like:
   * \code
   * myapp <command>
   * \endcode
   *
   * we could have:
   * \code
   * completion-find-current-positional-argument-name 1 myapp
   * // return command
   *
   * completion-find-current-positional-argument-name 2 myapp copy
   * // return copy-source
   *
   * completion-find-current-positional-argument-name 3 myapp copy some-file
   * // return copy-destination
   * \endcode
   *
   * \pre \a parserResult must be a valid Bash completion query
   * \sa BashCompletionParserQuery::isValidBashCompletionQuery()
   */
  static
  QString findCurrentPositionalArgumentName(const ParserResult & result, const ParserDefinition & parserDefinition)
  {
    assert( BashCompletionParserQuery::isValidBashCompletionQuery(result) );

    BashCompletionParserQuery query(result, parserDefinition);

    if( query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() ){
      const int index = query.cursorMainCommandPositionalArgumentsIndexInDefinition();
      if(index >= 0){
        if( parserDefinition.mainCommand().hasPositionalArgumentAt(index) ){
          return parserDefinition.mainCommand().positionalArgumentAt(index).name() % QLatin1String("-or-command");
        }
      }
    }

    if( query.isCursorAtMainCommandOption() ){
      return QLatin1String("options");
    }

    if( query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() ){

      // If the (main) command is complete
      if( query.mainCommandPositionalArgumentsCount() > parserDefinition.mainCommand().positionalArgumentCount() ){
        return QString();
      }

      const int index = query.cursorMainCommandPositionalArgumentsIndexInDefinition();
      assert( index >= 0 );
      if( parserDefinition.mainCommand().hasPositionalArgumentAt(index) ){
        return parserDefinition.mainCommand().positionalArgumentAt(index).name();
      }

    }

    if( query.isCursorAtSubCommandNameIndexInDefinition() ){
      return QLatin1String("command");
    }

    const auto subCommandDefinition = parserDefinition.findSubCommandByName( result.subCommand().name() );
    if( !subCommandDefinition ){
      return QString();
    }

    if( query.isCursorAtSubCommandOption() ){
      return subCommandDefinition->name() % QLatin1String("-options");
    }

    if( query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() ){

      // If the command is complete
      if( query.subCommandPositionalArgumentsCount() > subCommandDefinition->positionalArgumentCount() ){
        return QString();
      }

      const int index = query.cursorSubCommandPositionalArgumentsIndexInDefinition();
      assert( index >= 0 );
      if( subCommandDefinition->hasPositionalArgumentAt(index) ){
        return subCommandDefinition->name() % QLatin1Char('-') % subCommandDefinition->positionalArgumentAt(index).name();
      }

    }

    return QString();
  }

}}} // namespace Mdt{ namespace CommandLineParser{ namespace Impl{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_PARSER_H
