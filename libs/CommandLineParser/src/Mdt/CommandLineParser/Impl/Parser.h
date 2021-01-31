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
#ifndef MDT_COMMAND_LINE_IMPL_PARSER_PARSER_H
#define MDT_COMMAND_LINE_IMPL_PARSER_PARSER_H

#include "../ParserResult.h"
#include "../ParserResultCommand.h"
#include "../ParserResultOption.h"
#include "../ParserDefinition.h"
#include "../ParserDefinitionCommand.h"
#include "../ParserDefinitionOption.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <QStringList>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <algorithm>
#include <vector>
#include <iterator>
#include <cassert>
#include <cstddef>

namespace Mdt{ namespace CommandLineParser{

  namespace Impl{

    /*! \internal Split \a arguments to part that is before a sub-command and the other after
     *
     * \code
     * QStringList mainArguments;
     * QStringList subCommandArguments;
     *
     * const auto parserDefinition = ...
     * // parserDefinition contains the copy sub-command
     *
     * splitToMainAndSubCommandArguments({"myapp"}, parserDefinition, mainArguments, subCommandArguments);
     * // mainArguments == {"myapp"}
     * // subCommandArguments will be empty
     *
     * splitToMainAndSubCommandArguments({"myapp","-f","file.txt","/tmp"}, parserDefinition, mainArguments, subCommandArguments);
     * // mainArguments == {"myapp","-f","file.txt","/tmp"}
     * // subCommandArguments will be empty
     *
     * splitToMainAndSubCommandArguments({"myapp","-f","copy","file.txt","/tmp"}, parserDefinition, mainArguments, subCommandArguments);
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
     *
     * \todo This will probably not work when a option value or option name, or a positional argument,
     * that has a command name is passed before the command (after is ok)
     * Should parse before with QCommandLineParser and take positional arguments.
     */
    static
    void splitToMainAndSubCommandArguments(const QStringList & arguments, const ParserDefinition & parserDefinition,
                                           QStringList & mainArguments, QStringList & subCommandArguments)
    {
      assert( !arguments.isEmpty() );

      const auto isSubCommand = [&parserDefinition](const QString & argument){
        return parserDefinition.containsSubCommand(argument);
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

      QCommandLineOption qtOption(names);
      qtOption.setValueName( option.valueName() );
      qtOption.setDefaultValues( option.defaultValues() );

      return qtOption;
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

    /*! \internal Get a parser reult option from \a name
     *
     * If \a name is a short name, for example 'h',
     * the corresponding will be get from \a knownOptions
     *
     * \pre \a name must not be empty
     * \pre \a name must refer to a existing option in \a knownOptions
     */
    static
    ParserResultOption resultOptionFromName(const QString & name, const std::vector<ParserDefinitionOption> & knownOptions)
    {
      assert( !name.isEmpty() );

      if( name.length() > 1 ){
        return ParserResultOption(name);
      }

      assert( name.length() == 1 );
      const char shortName = name.at(0).toLatin1();

      const auto pred = [shortName](const ParserDefinitionOption & option){
        return option.shortName() == shortName;
      };
      const auto it = std::find_if(knownOptions.cbegin(), knownOptions.cend(), pred);
      assert( it != knownOptions.cend() );

      return ParserResultOption( it->name() );
    }

    /*! \internal Distributes values for each option named \a optionName
     */
    static
    void distributeValuesToOptionsForName(const QString & optionsName, const QStringList & optionsValues, std::vector<ParserResultOption> & allOptions)
    {
      int valuesIndex = 0;

      for(ParserResultOption & option : allOptions){
        if( option.name() == optionsName ){
          if( valuesIndex < optionsValues.size() ){
            option.setValue( optionsValues.at(valuesIndex) );
            ++valuesIndex;
          }
        }
      }
    }

    /*! \internal Fill a result command from \a qtParser
     */
    static
    void fillResultCommandFromQtParser(ParserResultCommand & command, const QCommandLineParser & qtParser, const ParserDefinitionCommand & defCommand)
    {
      const QStringList optionNames = qtParser.optionNames();
      std::vector<ParserResultOption> options;
      options.reserve( static_cast<size_t>( optionNames.size() ) );
      for(const QString & optionName : optionNames){
        options.emplace_back( resultOptionFromName( optionName, defCommand.options() ) );
      }
      for(int i=0; i<optionNames.size(); ++i){
        const QStringList optionValues = qtParser.values( optionNames.at(i) );
        distributeValuesToOptionsForName(options[static_cast<size_t>(i)].name(), optionValues, options);
      }
      command.setOptions(options);

      const QStringList positionalArguments = qtParser.positionalArguments();
      for(const QString & argument : positionalArguments){
        if( argument == QLatin1String("-") ){
          command.addOption( ParserResultOption() );
        }else{
          command.addPositionalArgument(argument);
        }
      }
    }

    /*! \internal
     */
    static
    bool parseCommand(const ParserDefinitionCommand & command, const QStringList & arguments, ParserResult & result, QCommandLineParser & qtParser)
    {
      setupQtParser(qtParser, command);
      if( !qtParser.parse(arguments) ){
        result.setErrorText( qtParser.errorText() );
        return false;
      }

      return true;
    }

    /*! \internal
     */
    static
    bool parseMainCommandToResult(const ParserDefinitionCommand & command, const QStringList & arguments, ParserResult & result)
    {
      QCommandLineParser qtParser;

      if( !parseCommand(command, arguments, result, qtParser) ){
        return false;
      }

      ParserResultCommand resultCommand;
      fillResultCommandFromQtParser(resultCommand, qtParser, command);
      result.setMainCommand(resultCommand);

      return true;
    }

    /*! \internal
     */
    static
    bool parseSubCommandToResult(const ParserDefinitionCommand & command, const QStringList & arguments, ParserResult & result)
    {
      QCommandLineParser qtParser;

      if( !parseCommand(command, arguments, result, qtParser) ){
        return false;
      }

      ParserResultCommand resultCommand( command.name() );
      fillResultCommandFromQtParser(resultCommand, qtParser, command);
      result.setSubCommand(resultCommand);

      return true;
    }

  } // namespace Impl{

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_IMPL_PARSER_PARSER_H
