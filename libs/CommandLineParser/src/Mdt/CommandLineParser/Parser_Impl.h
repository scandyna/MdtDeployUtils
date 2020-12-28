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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_IMPL_H
#define MDT_COMMAND_LINE_PARSER_PARSER_IMPL_H

#include "ParserDefinitionCommand.h"
#include "ParserDefinitionOption.h"
#include <QString>
#include <QChar>
#include <QStringList>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <algorithm>
#include <vector>
#include <iterator>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  namespace Impl{

    /*! \internal Check if \a subCommands contains a command named \a subCommandName
     */
    static
    bool containsSubCommand(const std::vector<ParserDefinitionCommand> & subCommands, const QString & subCommandName)
    {
      const auto pred = [&subCommandName](const ParserDefinitionCommand & command){
        return command.name() == subCommandName;
      };

      const auto it = std::find_if(subCommands.cbegin(), subCommands.cend(), pred);

      return it != subCommands.cend();
    }

    /*! \internal Split \a arguments to part that is before a sub-command and the other after
     *
     * \code
     * QStringList mainArguments;
     * QStringList subCommandArguments;
     *
     * const auto subCommands = parserDefinition.subCommands();
     * // subCommands contains the copy sub-command
     *
     * splitToMainAndSubCommandArguments({"myapp"}, subCommands, mainArguments, subCommandArguments);
     * // mainArguments == {"myapp"}
     * // subCommandArguments will be empty
     *
     * splitToMainAndSubCommandArguments({"myapp","-f","file.txt","/tmp"}, subCommands, mainArguments, subCommandArguments);
     * // mainArguments == {"myapp","-f","file.txt","/tmp"}
     * // subCommandArguments will be empty
     *
     * splitToMainAndSubCommandArguments({"myapp","-f","copy","file.txt","/tmp"}, subCommands, mainArguments, subCommandArguments);
     * // mainArguments == {"myapp","-f"}
     * // subCommandArguments == {"myapp","copy","file.txt","/tmp"}
     * \endcode
     *
     * Notice: if a known sub-command have beend found,
     * \a subCommandArguments will contain the application name (first argument of \a arguments),
     *  and the command name as second argument.
     * The application name is expected by QCommandLineParser,
     * and the sub-command name can be used in the parser result.
     *
     * \pre \a arguments must have at least 1 argument, which is the executable name
     */
    static
    void splitToMainAndSubCommandArguments(const QStringList & arguments, std::vector<ParserDefinitionCommand> subCommands,
                                           QStringList & mainArguments, QStringList & subCommandArguments)
    {
      assert( !arguments.isEmpty() );

      const auto isSubCommand = [&subCommands](const QString & argument){
        return containsSubCommand(subCommands, argument);
      };
      const auto it = std::find_if(arguments.cbegin(), arguments.cend(), isSubCommand);

      std::copy( arguments.cbegin(), it, std::back_inserter(mainArguments) );

      if( it != arguments.cend() ){
        subCommandArguments.push_back( arguments.at(0) );
        std::copy( it, arguments.cend(), std::back_inserter(subCommandArguments) );
      }
    }

    /*! \internal Get a QCommandLineOption from \a option
     */
    static
    QCommandLineOption qtParserOptionFromOptionDefinition(const ParserDefinitionOption & option)
    {
      QStringList names;
      if( option.hasShortName() ){
        names.append( QChar::fromLatin1( option.shortName() ) );
      }
      names.append( option.name() );

      return QCommandLineOption(names);
    }

    /*! \internal Setup \a qtParser based on \a commandDefinition
     */
    static
    void setupQtParser(QCommandLineParser & qtParser, const ParserDefinitionCommand & commandDefinition)
    {
      for( const auto & option : commandDefinition.options() ){
        qtParser.addOption( qtParserOptionFromOptionDefinition(option) );
      }
    }

  } // namespace Impl{

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_IMPL_H
