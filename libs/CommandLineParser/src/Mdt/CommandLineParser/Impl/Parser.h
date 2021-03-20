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
#include "../CommandLine/CommandLine.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <QLatin1Char>
#include <QStringList>
#include <QCoreApplication>

#include <QCommandLineOption>
#include <QCommandLineParser>

#include <algorithm>
#include <vector>
#include <iterator>
#include <cassert>
#include <cstddef>

#include <QDebug>

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

    /*! \internal
     */
    static
    std::vector<ParserDefinitionOption>::const_iterator
    findOptionByLongNameInCommand(const QString & name, const ParserDefinitionCommand & command) noexcept
    {
      const auto pred = [&name](const ParserDefinitionOption & option){
        return option.name() == name;
      };

      return std::find_if( command.options().cbegin(), command.options().cend(), pred );
    }

    /*! \internal
     */
    static
    std::vector<ParserDefinitionOption>::const_iterator
    findOptionByShortNameInCommand(char name, const ParserDefinitionCommand & command) noexcept
    {
      const auto pred = [name](const ParserDefinitionOption & option){
        return option.shortName() == name;
      };

      return std::find_if( command.options().cbegin(), command.options().cend(), pred );
    }

    /*! \internal
     */
    class ParseError
    {
      Q_DECLARE_TR_FUNCTIONS(ParseError)

     public:

      QString errorText;
      QStringList unknownOptionNames;

      bool hasError() const noexcept
      {
        return !errorText.isEmpty();
      }
    };

    /*! \internal
     *
     * If the last option expects a value in the next argument, and it is present,
     * \a current will be incremented.
     * The caller must only care to increment its current iterator by 1 to go to the next argument.
     *
     * Return true if at least 1 option was added, otherwise false.
     * On error, \a error's errorText will be set and false is returned.
     */
    static
    bool addShortOptionsToCommandLine(QStringList::const_iterator & current, const QStringList::const_iterator & last,
                                      const ParserDefinitionCommand & command, CommandLine::CommandLine & commandLine,
                                      ParseError & error) noexcept
    {
      assert(current != last);
      assert(current->length() >= 2);
      assert(current->at(0).toLatin1() == '-');
      assert(current->at(1).toLatin1() != '-');

      auto currentShortOptionIt = current->cbegin() + 1;
      const auto endShortOptionIt = std::find( currentShortOptionIt, current->cend(), QChar::fromLatin1('=') );
      const auto lastShortOptionIt = endShortOptionIt - 1;

      const bool valueShouldBeNextArgument = endShortOptionIt == current->cend();

      std::vector<char> optionNames;
      bool lastShortOptionExpectsValue = false;

      while(currentShortOptionIt != endShortOptionIt){
        const char optionName = currentShortOptionIt->toLatin1();
        assert( optionName != '=' );

        const auto parserDefinitionOptionIt = findOptionByShortNameInCommand(optionName, command);
        if( parserDefinitionOptionIt == command.options().cend() ){
          error.errorText = ParseError::tr("Unknown option '%1'").arg(optionName);
          error.unknownOptionNames.append( QString( QChar::fromLatin1(optionName) ) );
          return false;
        }

        optionNames.push_back(optionName);

        if( parserDefinitionOptionIt->hasValueName() ){
          if( currentShortOptionIt != lastShortOptionIt ){
            error.errorText = ParseError::tr("Short option '%1' expects a value, but is not the last").arg(optionName);
            return false;
          }
          lastShortOptionExpectsValue = true;
        }

        ++currentShortOptionIt;
      }

      if( optionNames.size() == 1 ){
        if(lastShortOptionExpectsValue){
          if(valueShouldBeNextArgument){
            commandLine.appendOptionExpectingValue( QString( QChar::fromLatin1(optionNames[0]) ) );
          }
        }else{
          commandLine.appendOption( QString( QChar::fromLatin1(optionNames[0]) ) );
        }
      }else{
        if(lastShortOptionExpectsValue){
          if(valueShouldBeNextArgument){
           commandLine.appendShortOptionListWithLastExpectingValue(optionNames);
          }
        }else{
          commandLine.appendShortOptionList(optionNames);
        }
      }

      if(lastShortOptionExpectsValue){
        if(valueShouldBeNextArgument){
          ++current;
          if(current == last){
            --current;
            error.errorText = ParseError::tr("option '%1' expects a value, but was not provided").arg( optionNames.back() );
            return false;
          }
          commandLine.appendOptionValue(*current);
        }else{
          assert( endShortOptionIt != current->cend() );
          const auto valueIt = endShortOptionIt + 1;
          if( valueIt == current->cend() ){
            error.errorText = ParseError::tr("option '%1' expects a value, but was not provided").arg( optionNames.back() );
            return false;
          }
          QString value;
          std::copy( valueIt, current->cend(), std::back_inserter(value) );
          if( optionNames.size() == 1 ){
            commandLine.appendOptionWithValue( QString( QChar::fromLatin1(optionNames[0]) ), value);
          }else{
            commandLine.appendShortOptionListWithLastHavingValue(optionNames, value);
          }
        }
      }

      return true;
    }

    /*! \internal
     *
     * If the option expects a value at the next argument, and it is present,
     * \a current will be incremented.
     * The caller must only care to increment its current iterator by 1 to go to the next argument.
     *
     * Return true if the option was added, otherwise false.
     * On error, \a error's errorText will be set and false is returned.
     */
    static
    bool addLongOptionToCommandLine(QStringList::const_iterator & current, const QStringList::const_iterator & last,
                                    const ParserDefinitionCommand & command, CommandLine::CommandLine & commandLine,
                                    ParseError & error) noexcept
    {
      assert(current != last);
      assert(current->length() >= 3);
      assert(current->at(0).toLatin1() == '-');
      assert(current->at(1).toLatin1() == '-');
//       assert(current->at(2).toLatin1() != '-');

      const auto optionNameFirstIt = current->cbegin() + 2;
      const auto optionNameEndIt = std::find( current->cbegin(), current->cend(), QChar::fromLatin1('=') );

      QString optionName;
      std::copy( optionNameFirstIt, optionNameEndIt, std::back_inserter(optionName) );

      const bool valueShouldBeNextArgument = optionNameEndIt == current->cend();

      const auto parserDefinitionOptionIt = findOptionByLongNameInCommand(optionName, command);
      if( parserDefinitionOptionIt == command.options().cend() ){
        error.errorText = ParseError::tr("Unknown option '%1'").arg(optionName);
        error.unknownOptionNames.append(optionName);
        return false;
      }

      const bool optionExpectsValue = parserDefinitionOptionIt->hasValueName();

      if(optionExpectsValue){
        if(valueShouldBeNextArgument){
          commandLine.appendOptionExpectingValue(optionName);
        }
      }else{
        commandLine.appendOption(optionName);
      }

      if(optionExpectsValue){
        if(valueShouldBeNextArgument){
          ++current;
          if(current == last){
            --current;
            error.errorText = ParseError::tr("option '%1' expects a value, but was not provided").arg(optionName);
            return false;
          }
          commandLine.appendOptionValue(*current);
        }else{
          assert( optionNameEndIt != current->cend() );
          const auto valueIt = optionNameEndIt + 1;
          if( valueIt == current->cend() ){
            error.errorText = ParseError::tr("option '%1' expects a value, but was not provided").arg(optionName);
            return false;
          }
          QString value;
          std::copy( valueIt, current->cend(), std::back_inserter(value) );
          commandLine.appendOptionWithValue(optionName, value);
        }
      }

      return true;
    }

    /*! \internal
     *
     * If the option expects a value that should be at next argument, and it is present,
     * \a current will be incremented.
     * The caller must only care to increment its current iterator by 1 to go to the next argument.
     *
     * Return true if a option was added, otherwise false.
     * On error, \a error's errorText will be set and false is returned.
     */
    static
    bool addOptionOrDashToCommandLine(QStringList::const_iterator & current, const QStringList::const_iterator & last,
                                      const ParserDefinitionCommand & command, CommandLine::CommandLine & commandLine,
                                      ParseError & error) noexcept
    {
      assert(current != last);
      assert( !error.hasError() );

      if( current->startsWith( QLatin1String("--") ) ){
//         if( current->length() == 2 ){
//           commandLine.appendDoubleDash();
//           return true;
//         }
        assert( current->length() > 2 );
        if( addLongOptionToCommandLine(current, last, command, commandLine, error) ){
          return true;
        }
        if( error.hasError() ){
          return false;
        }
      }

      if( current->startsWith( QLatin1Char('-') ) ){
        if( current->length() == 1 ){
          commandLine.appendSingleDash();
          return true;
        }
        if( current->length() >= 2 ){
          if( addShortOptionsToCommandLine(current, last, command, commandLine, error) ){
            return true;
          }
        }
      }

      return false;
    }

    /*! \internal
     */
    static
    void parseAsPositionalArgument(QStringList::const_iterator first, QStringList::const_iterator last, CommandLine::CommandLine & commandLine) noexcept
    {
      while(first != last){
        commandLine.appendPositionalArgument(*first);
        ++first;
      }
    }

    /*! \internal
     *
     * \todo should not search about sub-command in the loop, but once (benchmark this)
     */
    static
    bool parse(const QStringList & arguments, const ParserDefinition & parserDefinition, CommandLine::CommandLine & commandLine, ParseError & error) noexcept
    {
      if( arguments.isEmpty() ){
        error.errorText = ParseError::tr("a command line should at least contain the executable name");
        return false;
      }

      auto first = arguments.cbegin();
      const auto last = arguments.cend();
      assert( first != last );

      const ParserDefinitionCommand *command = &parserDefinition.mainCommand();
      bool inSubCommand = false;

      commandLine.setExecutableName(*first);
      ++first;

      while(first != last){
        if( *first == QLatin1String("--") ){
          commandLine.appendDoubleDash();
          ++first;
          parseAsPositionalArgument(first, last, commandLine);
          return true;
        }
        if(!inSubCommand){
          const auto subCommand = parserDefinition.findSubCommandByName(*first);
          if(subCommand){
            inSubCommand = true;
            command = &(*subCommand);
            commandLine.appendSubCommandName( command->name() );
            ++first;
            if(first == last){
              return true;
            }
          }
        }
        if( !addOptionOrDashToCommandLine(first, last, *command, commandLine, error) ){
          if( error.hasError() ){
            return false;
          }
          commandLine.appendPositionalArgument(*first);
        }
        ++first;
      }

      return true;
    }

  } // namespace Impl{

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_IMPL_PARSER_PARSER_H
