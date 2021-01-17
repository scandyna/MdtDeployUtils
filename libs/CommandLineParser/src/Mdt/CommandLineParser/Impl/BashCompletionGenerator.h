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
#ifndef MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_GENERATOR_H
#define MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_GENERATOR_H

#include "../BashCompletionGenerator.h"
#include "../BashCompletionGeneratorCommand.h"
#include "../BashCompletionGeneratorOption.h"
#include "../BashCompletionGeneratorPositionalArgument.h"
#include "../BashCompletionParserQuery.h"
#include "../Algorithm.h"
#include <QString>
#include <QStringList>
#include <QLatin1String>
#include <QChar>
#include <QLatin1Char>
#include <QStringBuilder>
#include <cassert>
#include <algorithm>

namespace Mdt{ namespace CommandLineParser{

  namespace Impl{

    inline
    void addCommandOptionToWordList(const BashCompletionGeneratorOption & option, QStringList & wordList)
    {
      if( option.hasShortName() ){
        wordList.push_back( option.shortNameWithDash() );
      }
      wordList.push_back( option.nameWithDashes() );
    }

    inline
    void addCommandOptionsToWordList(const BashCompletionGeneratorCommand& command, QStringList & wordList)
    {
      for( const auto & option : command.options() ){
        addCommandOptionToWordList(option, wordList);
      }
    }

    static
    QString commandOptionsToWordList(const BashCompletionGeneratorCommand & command)
    {
      QStringList wordList;
      addCommandOptionsToWordList(command, wordList);

      return wordList.join( QLatin1Char(' ') );
    }

    static
    QString completionFindCurrentPositionalArgumentNameString()
    {
      return BashCompletionParserQuery::findCurrentPositionalArgumentNameString();
    }

    /*! \internal Get a COMPREPLY=($(compgen -W)) statement for available options in \a command
     *
     * \pre \a command must have at least 1 option
     */
    static
    QString compreplyStatementFromCommandOptions(const BashCompletionGeneratorCommand & command, const QString & curVariableName)
    {
      assert( command.hasOptions() );

      return QLatin1String("COMPREPLY=($(compgen -W \"") % commandOptionsToWordList(command)
              % QLatin1String("\" -- \"$") % curVariableName % QLatin1String("\"))");
    }

    static
    QString compgenActionNameFromArgumentType(ArgumentType argumentType)
    {
      switch(argumentType){
        case ArgumentType::File:
        case ArgumentType::DirectoryOrFile:
          return QLatin1String("file");
        case ArgumentType::Directory:
          return QLatin1String("directory");
        case ArgumentType::Unspecified:
          return QString();
      }

      return QString();
    }

    /*! \internal Get a COMPREPLY=($(compgen -A)) statement from \a argument
     */
    static
    QString compreplyStatementFromPositionalArgument(const BashCompletionGeneratorPositionalArgument & argument, const QString & curVariableName)
    {
      const QString actionName = compgenActionNameFromArgumentType( argument.type() );

      if( actionName.isEmpty() ){
        return QString();
      }

      return QLatin1String("COMPREPLY=($(compgen -A ") % actionName
              % QLatin1String(" -- \"$") % curVariableName % QLatin1String("\"))");
    }

    /*! \internal Get a COMPREPLY=($(compgen ...)) statement from first positional argument in \a command
     *
     * \pre \a command must have at least 1 positional argument
     */
    static
    QString compreplyStatementFromFirstPositionalArgumentInCommand(const BashCompletionGeneratorCommand & command, const QString & curVariableName)
    {
      assert( command.hasPositionalArguments() );

      if( !command.hasOptions() ){
        return compreplyStatementFromPositionalArgument( command.positionalArgumentAt(0), curVariableName );
      }

      const QString actionName = compgenActionNameFromArgumentType( command.positionalArgumentAt(0).type() );
      const auto actionArgument = [&actionName](){
        if( actionName.isEmpty() ){
          return QString();
        }
        return QString( QLatin1String(" -A ") + actionName );
      };

      const QString optionsWordList = commandOptionsToWordList(command);

      return QLatin1String("COMPREPLY=($(compgen") % actionArgument() % QLatin1String(" -W \"") % optionsWordList % QLatin1Char('"')
              % QLatin1String(" -- \"$") % curVariableName % QLatin1String("\"))");
    }

    /*! \internal Get a COMPREPLY=($(compgen ...)) for a positional argument in \a command
     *
     * For the first argument in \a command , the options of \a command are also added.
     *
     * \pre \a argumentIndex must be in valid range
     */
    static
    QString compreplyStatementFromPositionalArgumentInCommand(const BashCompletionGeneratorCommand & command, int argumentIndex, const QString & curVariableName)
    {
      assert( argumentIndex >= 0 );
      assert( argumentIndex < command.positionalArgumentCount() );

      if(argumentIndex == 0){
        return compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName);
      }
      return compreplyStatementFromPositionalArgument( command.positionalArgumentAt(argumentIndex), curVariableName );
    }

    /*! \internal Get a case pattern) for a positional argument in \a command
     *
     * \pre \a argumentIndex must be in valid range
     */
    static
    QString casePatternFromCommandPositionalArgument(const BashCompletionGeneratorCommand & command, int argumentIndex, const QString & curVariableName)
    {
      assert( argumentIndex >= 0 );
      assert( argumentIndex < command.positionalArgumentCount() );

      const QString compreplyStatement = compreplyStatementFromPositionalArgument( command.positionalArgumentAt(argumentIndex), curVariableName );
      if( compreplyStatement.isEmpty() ){
        return QString();
      }

      const auto argumentName = [&command, argumentIndex](){
        if( command.hasName() ){
          return command.name() + QLatin1Char('-') + command.positionalArgumentAt(argumentIndex).name();
        }
        return command.positionalArgumentAt(argumentIndex).name();
      };

      return QLatin1String("    ") % argumentName() % QLatin1String(")\n")
           % QLatin1String("      ") % compreplyStatementFromPositionalArgumentInCommand( command, argumentIndex, curVariableName ) % QLatin1Char('\n')
           % QLatin1String("      ;;");
    }

    static
    QString casePatternsFromCommandPositionalArgumentList(const BashCompletionGeneratorCommand & command, const QString & curVariableName)
    {
      QString casePattern;

      for(int i=0; i < command.positionalArgumentCount(); ++i){
        const QString positionalArgumentCasePattern = casePatternFromCommandPositionalArgument(command, i, curVariableName);
        if( !positionalArgumentCasePattern.isEmpty() ){
          casePattern += positionalArgumentCasePattern % QLatin1String("\n\n");
        }
      }

      return casePattern;
    }

    static
    QString generateCommandCasesBlock(const BashCompletionGeneratorCommand & command, const QString & curVariableName)
    {
      QString block;

      const auto optionsCaseName = [&command](){
        if( command.hasName() ){
          return command.name() + QLatin1String("-options");
        }
        return QString( QLatin1String("options") );
      };

      if( command.hasOptions() ){
        block += QLatin1String("    ") % optionsCaseName() % QLatin1String(")\n")
               % QLatin1String("      ") % compreplyStatementFromCommandOptions(command, curVariableName) % QLatin1Char('\n')
               % QLatin1String("      ;;\n\n");
      }

      if( command.hasPositionalArguments() ){
        block += casePatternsFromCommandPositionalArgumentList(command, curVariableName);
      }

      return block;
    }

    static
    QString generateMainCommandCasesBlock(const BashCompletionGenerator & generator, const QString & curVariableName)
    {
      return generateCommandCasesBlock( generator.mainCommand(), curVariableName );
    }

    static
    QString commandListToWordList(const BashCompletionGenerator & generator)
    {
      QStringList wordList;

      for( const auto & command : generator.subCommands() ){
        wordList.push_back( command.name() );
      }

      return wordList.join( QLatin1Char(' ') );
    }

    /*! \internal Get a COMPREPLY=($(compgen -W)) statement for available commands in \a generator
     *
     * \pre \a generator must have at least 1 sub-command
     */
    static
    QString compreplyStatementForCommandCaseBlock(const BashCompletionGenerator & generator, const QString & curVariableName)
    {
      assert( generator.hasSubCommands() );

      QString wordList;
      if( generator.mainCommand().hasOptions() ){
        wordList = commandOptionsToWordList( generator.mainCommand() ) % QLatin1Char(' ');
      }
      wordList += commandListToWordList(generator);

      return QLatin1String("COMPREPLY=($(compgen -W \"") % wordList % QLatin1String("\" -- \"$") % curVariableName % QLatin1String("\"))");
    }

    static
    QString generateCommandCaseBlock(const BashCompletionGenerator & generator, const QString & curVariableName)
    {
      if( !generator.hasSubCommands() ){
        return QString();
      }

      return QLatin1String("    command)\n")
           % QLatin1String("      ") % compreplyStatementForCommandCaseBlock(generator, curVariableName) % QLatin1Char('\n')
           % QLatin1String("      ;;\n\n");
    }

    static
    QString generateSubCommandsCasesBlocks(const BashCompletionGenerator & generator, const QString & curVariableName)
    {
      QString block;

      for( const auto & command : generator.subCommands() ){
        block += generateCommandCasesBlock(command, curVariableName);
      }

      return block;
    }

    static
    QString generateCaseBlock(const BashCompletionGenerator & generator, const QString & queryVariableName)
    {
      const QString curVariableName = QLatin1String("cur");

      return QLatin1String("  case $") % queryVariableName % QLatin1String(" in\n\n")
        % generateMainCommandCasesBlock(generator, curVariableName)
        % generateCommandCaseBlock(generator, curVariableName)
        % generateSubCommandsCasesBlocks(generator, curVariableName)
        % QLatin1String("    *)\n      ;;\n\n")
        % QLatin1String("  esac");
    }

    static
    QString generateCompletionFunction(const BashCompletionGenerator & generator)
    {
      const QString completionFunctionName = QLatin1Char('_') % generator.applicationName() % QLatin1String("_completions()");
      const QString queryVariableName = QLatin1String("currentPositionalArgument");

      return completionFunctionName % QLatin1String("\n{\n")
        % QLatin1String("  local cur=\"${COMP_WORDS[COMP_CWORD]}\"\n")
        % QLatin1String("  local executable=\"$1\"\n")
        % QLatin1String("  local ") % queryVariableName % QLatin1String("=$(\"$executable\" ")
          % completionFindCurrentPositionalArgumentNameString() % QLatin1String(" $COMP_CWORD $COMP_LINE)\n\n")
        % generateCaseBlock(generator, queryVariableName)
        % QLatin1String("\n\n}");
    }

  } // namespace Impl{

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_GENERATOR_H
