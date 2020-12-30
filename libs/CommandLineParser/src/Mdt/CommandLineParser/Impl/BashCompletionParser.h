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
#ifndef MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_PARSER_H
#define MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_PARSER_H

#include "../ParserResult.h"
#include "../ParserDefinition.h"
#include <QLatin1String>
#include <QString>
#include <QLatin1Char>
#include <QStringBuilder>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace Impl{

  static
  QString completionFindCurrentPositionalArgumentNameString()
  {
    return QLatin1String("completion-find-current-positional-argument-name");
  }

  /*! \internal Find the name of the current positional argument in \a result
   *
   * Possible commands.
   *
   * For a simple application like:
   * \code
   * myapp <source> <destination>
   * \endcode
   *
   * we could have:
   * \code
   * completion-find-current-positional-argument-name
   * // return source
   *
   * completion-find-current-positional-argument-name some-file
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
   * completion-find-current-positional-argument-name
   * // return command
   *
   * completion-find-current-positional-argument-name copy
   * // return copy-source
   *
   * completion-find-current-positional-argument-name copy some-file
   * // return copy-destination
   * \endcode
   *
   * \pre \a result must at least contain the completionFindCurrentPositionalArgumentNameString()
   */
  static
  QString findCurrentPositionalArgumentName(const ParserResult & result, const ParserDefinition & parserDefinition)
  {
    assert( result.hasPositionalArgumentAt(0) );
    assert( result.positionalArgumentAt(0) == completionFindCurrentPositionalArgumentNameString() );

    if( result.hasSubCommand() ){
      const auto subCommandDefinition = parserDefinition.findSubCommandByName( result.subCommand().name() );
      if( !subCommandDefinition ){
        return QString();
      }

      const int positionalArgumentCount = result.subCommand().positionalArgumentCount();

      // If the command is complete, return nothing
      if( positionalArgumentCount == subCommandDefinition->positionalArgumentCount() ){
        return QString();
      }

      // We search the next argument name
      const int lastPositionalArgumentIndex = positionalArgumentCount;

      if(lastPositionalArgumentIndex < 0){
        return QString();
      }
      if( subCommandDefinition->hasPositionalArgumentAt(lastPositionalArgumentIndex) ){
        return subCommandDefinition->name() % QLatin1Char('-') % subCommandDefinition->positionalArgumentAt(lastPositionalArgumentIndex).name();
      }

    }else{
      // We have the query string as first argument, so substract it
      const int positionalArgumentCount = result.positionalArgumentCount()-1;
      assert(positionalArgumentCount >= 0);

      // If the (main) command is complete
      if( positionalArgumentCount == parserDefinition.mainCommand().positionalArgumentCount() ){
        if( parserDefinition.hasSubCommands() ){
          return QLatin1String("command");
        }
        return QString();
      }

      // We search the next argument name
      const int lastPositionalArgumentIndex = positionalArgumentCount;
      if(lastPositionalArgumentIndex < 0){
        return QString();
      }
      if( parserDefinition.mainCommand().hasPositionalArgumentAt(lastPositionalArgumentIndex) ){
        return parserDefinition.mainCommand().positionalArgumentAt(lastPositionalArgumentIndex).name();
      }
    }

    return QString();
  }

}}} // namespace Mdt{ namespace CommandLineParser{ namespace Impl{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_PARSER_H
