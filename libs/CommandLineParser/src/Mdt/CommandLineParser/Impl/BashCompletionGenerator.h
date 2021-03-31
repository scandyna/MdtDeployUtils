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
#include "../BashCompletion/ActionList.h"
#include "../BashCompletion/CompgenAction.h"
#include "../BashCompletion/CompgenCommand.h"
#include "../BashCompletion/ScriptCaseClause.h"
#include "../BashCompletion/ScriptCaseStatement.h"
#include "../BashCompletion/Script.h"
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

    /*! \internal
     */
    static
    QStringList commandOptionsToWordList(const BashCompletionGeneratorCommand & command)
    {
      QStringList wordList;
      addCommandOptionsToWordList(command, wordList);

      return wordList;
    }

    static
    QString commandOptionsToWordListString(const BashCompletionGeneratorCommand & command)
    {
      QStringList wordList;
      addCommandOptionsToWordList(command, wordList);

      return wordList.join( QLatin1Char(' ') );
    }

    static
    QString completionFindCurrentPositionalArgumentNameString()
    {
      return BashCompletionParserQuery::findCurrentArgumentString();
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

    /*! \internal Map \a argumentType to corresponding CompgenAction
     *
     * \pre \a argumentType must not be Unspecified
     */
    static
    BashCompletion::CompgenAction compgenActionFromArgumentType(ArgumentType argumentType) noexcept
    {
      assert( argumentType != ArgumentType::Unspecified );

      using BashCompletion::CompgenAction;

      switch(argumentType){
        case ArgumentType::File:
        case ArgumentType::DirectoryOrFile:
          return CompgenAction::ListFiles;
        case ArgumentType::Directory:
          return CompgenAction::ListDirectories;
        case ArgumentType::Unspecified:
          break;
      }

      // Should never be reached (just make the compiler happy)
      return CompgenAction::ListFiles;
    }

    /*! \internal Get a compgen command from \a option
     */
    static
    BashCompletion::CompgenCommand compgenCommandFromParserDefinitionOption(const ParserDefinitionOption & option) noexcept
    {
      BashCompletion::CompgenCommand compgenCommand;

      if( !option.hasValueName() ){
        return compgenCommand;
      }

      if( option.valueType() != ArgumentType::Unspecified ){
        compgenCommand.addAction( compgenActionFromArgumentType( option.valueType() ) );
      }

      if( option.hasPossibleValues() ){
        compgenCommand.addWordList( option.possibleValues() );
      }

      return compgenCommand;
    }

    /*! \internal Get a compgen command from \a option
     */
    static
    BashCompletion::CompgenCommand compgenCommandFromParserDefinitionPositionalArgument(const ParserDefinitionPositionalArgument & argument) noexcept
    {
      BashCompletion::CompgenCommand compgenCommand;

      if( argument.type() != ArgumentType::Unspecified ){
        compgenCommand.addAction( compgenActionFromArgumentType( argument.type() ) );
      }

      if( argument.hasPossibleValues() ){
        compgenCommand.addWordList( argument.possibleValues() );
      }

      return compgenCommand;
    }

    /*! \internal
     */
    inline
    QString caseClausePattern(const BashCompletionGeneratorCommand & command, const QString & rest) noexcept
    {
      if( command.hasName() ){
        return command.name() % QLatin1Char('-') % rest;
      }
      return rest;
    }

    /*! \internal
     *
     * \pre \a option must have a action
     */
    static
    BashCompletion::ScriptCaseClause caseClauseForOptionNameValue(const BashCompletionGeneratorCommand & command,
                                                                  const BashCompletionGeneratorOption & option) noexcept
    {
      assert( option.hasAction() );

      const QString name = QLatin1String("option-") % option.name() % QLatin1String("-value");
      const QString pattern = caseClausePattern( command, name );

      return BashCompletion::ScriptCaseClause( pattern, option.action().toCompreplyString() );
    }

    /*! \internal
     *
     * \pre \a option must have a action
     * \pre \a option must have a short name
     */
    static
    BashCompletion::ScriptCaseClause caseClauseForOptionShortNameValue(const BashCompletionGeneratorCommand & command,
                                                                       const BashCompletionGeneratorOption & option) noexcept
    {
      assert( option.hasAction() );
      assert( option.hasShortName() );

      const QString name = QLatin1String("option-") % QLatin1Char( option.shortName() ) % QLatin1String("-value");
      const QString pattern = caseClausePattern( command, name );

      return BashCompletion::ScriptCaseClause( pattern, option.action().toCompreplyString() );
    }

    /*! \internal
     *
     * \pre \a argument must have a action
     */
    static
    BashCompletion::ScriptCaseClause caseClauseForPositionalArgumentName(const BashCompletionGeneratorCommand & command,
                                                                         const BashCompletionGeneratorPositionalArgument & argument) noexcept
    {
      assert( argument.hasAction() );

      const QString pattern = caseClausePattern( command, argument.name() );

      return BashCompletion::ScriptCaseClause( pattern, argument.action().toCompreplyString() );
    }

    /*! \internal
     *
     * \pre \a command must have at least 1 option
     */
    inline
    BashCompletion::Action actionToListCommandOptions(const BashCompletionGeneratorCommand & command) noexcept
    {
      assert( command.hasOptions() );

      BashCompletion::Action action;

      BashCompletion::CompgenCommand compgenCommand;
      compgenCommand.addWordList( commandOptionsToWordList(command) );
      action.setCompgenCommand(compgenCommand);

      return action;
    }

    /*! \internal
     *
     * \pre \a command must have at least 1 option
     */
    static
    BashCompletion::ScriptCaseClause caseClauseToListCommandOptions(const BashCompletionGeneratorCommand & command) noexcept
    {
      assert( command.hasOptions() );

      const QString pattern = caseClausePattern( command, QLatin1String("options") );
      const auto action = actionToListCommandOptions(command);

      return BashCompletion::ScriptCaseClause( pattern, action.toCompreplyString() );
    }

    /*! \internal
     *
     * \pre \a command must have at least 1 option
     * \pre \a argument must have a action
     */
    static
    BashCompletion::ScriptCaseClause caseClauseForOptionsOrPositionalArgumentName(const BashCompletionGeneratorCommand & command,
                                                                                  const BashCompletionGeneratorPositionalArgument & argument) noexcept
    {
      assert( command.hasOptions() );
      assert( argument.hasAction() );

      const QString patternRightPart = QLatin1String("options") % QLatin1String("-or-") % argument.name();
      const QString pattern = caseClausePattern(command, patternRightPart);

      BashCompletion::ActionList actions;
      actions.addAction( actionToListCommandOptions(command) );
      actions.addAction( argument.action() );

      return BashCompletion::ScriptCaseClause( pattern, actions.toCompreplyString() );
    }

    /*! \internal
     */
    inline
    QStringList commandListToWordList(const BashCompletionGenerator & generator) noexcept
    {
      QStringList wordList;

      for( const auto & command : generator.subCommands() ){
        wordList.push_back( command.name() );
      }

      return wordList;
    }

    /*! \internal
     *
     * \pre \a generator must have at least 1 sub-command
     */
    static
    BashCompletion::ScriptCaseClause caseClauseToListAvailableSubCommands(const BashCompletionGenerator & generator) noexcept
    {
      assert( generator.hasSubCommands() );

      const QStringList wordList = commandListToWordList(generator);
      BashCompletion::Action action;
      BashCompletion::CompgenCommand compgenCommand;
      compgenCommand.addWordList(wordList);
      action.setCompgenCommand(compgenCommand);

      return BashCompletion::ScriptCaseClause( QLatin1String("commands"), action.toCompreplyString() );
    }

    /*! \internal
     *
     * \pre \a generator must have at least 1 sub-command
     * \pre \a generator must have at least 1 option
     */
    static
    BashCompletion::ScriptCaseClause caseClauseToListCommandOptionsOrAvailableSubCommands(const BashCompletionGenerator & generator) noexcept
    {
      assert( generator.hasSubCommands() );
      assert( generator.mainCommand().hasOptions() );

      const QStringList wordList = commandOptionsToWordList( generator.mainCommand() ) + commandListToWordList(generator);
      BashCompletion::Action action;
      BashCompletion::CompgenCommand compgenCommand;
      compgenCommand.addWordList(wordList);
      action.setCompgenCommand(compgenCommand);

      return BashCompletion::ScriptCaseClause( QLatin1String("options-or-commands"), action.toCompreplyString() );
    }

    /*! \internal
     */
    static
    void addCaseClausesForCommandPositionalArgumentsToScript(const BashCompletionGeneratorCommand & command, BashCompletion::Script & script) noexcept
    {
      for( const auto & argument : command.positionalArguments() ){
        if( argument.hasAction() ){
          script.addClause( caseClauseForPositionalArgumentName(command, argument) );
          if( command.hasOptions() ){
            script.addClause( caseClauseForOptionsOrPositionalArgumentName(command, argument) );
          }
        }
      }
    }

    /*! \internal
     */
    static
    void addCaseClausesForCommandOptionsWithValueToScript(const BashCompletionGeneratorCommand & command, BashCompletion::Script & script) noexcept
    {
      for( const auto & option : command.options() ){
        if( option.hasAction() ){
          script.addClause( caseClauseForOptionNameValue(command, option) );
          if( option.hasShortName() ){
            script.addClause( caseClauseForOptionShortNameValue(command, option) );
          }
        }
      }
    }

    /*! \internal
     */
    static
    void addCaseClausesToScript(const BashCompletionGenerator & generator, BashCompletion::Script & script) noexcept
    {
      if( generator.mainCommand().hasOptions() ){
        script.addClause( caseClauseToListCommandOptions( generator.mainCommand() ) );
      }
      addCaseClausesForCommandPositionalArgumentsToScript( generator.mainCommand(), script );
      addCaseClausesForCommandOptionsWithValueToScript( generator.mainCommand(), script );

      if( generator.hasSubCommands() ){
        script.addClause( caseClauseToListAvailableSubCommands(generator) );
        if( generator.mainCommand().hasOptions() ){
          script.addClause( caseClauseToListCommandOptionsOrAvailableSubCommands(generator) );
        }
        for( const auto & command : generator.subCommands() ){
          if( command.hasOptions() ){
            script.addClause( caseClauseToListCommandOptions(command) );
          }
          addCaseClausesForCommandPositionalArgumentsToScript(command, script);
          addCaseClausesForCommandOptionsWithValueToScript(command, script);
        }
      }
    }

    /*! \internal Get a COMPREPLY=($(compgen -W)) statement for available options in \a command
     *
     * \pre \a command must have at least 1 option
     */
    [[deprecated]]
    static
    QString compreplyStatementToListCommandOptions(const BashCompletionGeneratorCommand & command, const QString & curVariableName)
    {
      assert( command.hasOptions() );

      return QLatin1String("COMPREPLY=($(compgen -W \"") % commandOptionsToWordListString(command)
              % QLatin1String("\" -- \"$") % curVariableName % QLatin1String("\"))");
    }


    /*! \internal Get a COMPREPLY=($(compgen -A)) statement from \a argument
     */
    [[deprecated]]
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
    [[deprecated]]
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

      const QString optionsWordList = commandOptionsToWordListString(command);

      return QLatin1String("COMPREPLY=($(compgen") % actionArgument() % QLatin1String(" -W \"") % optionsWordList % QLatin1Char('"')
              % QLatin1String(" -- \"$") % curVariableName % QLatin1String("\"))");
    }

    /*! \internal Get a COMPREPLY=($(compgen ...)) for a positional argument in \a command
     *
     * For the first argument in \a command , the options of \a command are also added.
     *
     * \pre \a argumentIndex must be in valid range
     */
    [[deprecated]]
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

    /*! \internal Get a case pattern bloc from \a pattern and \a body
     *
     * \pre \a pattern and \a body must not be empty
     */
    [[deprecated]]
    static
    QString formatCasePatternBlock(const QString & pattern, const QString & body) noexcept
    {
      assert( !pattern.isEmpty() );
      assert( !body.isEmpty() );

      return QLatin1String("    ") % pattern % QLatin1String(")\n      ")
             % body % QLatin1String("\n      ;;\n");
    }

    /*! \internal Get a case pattern) for a positional argument in \a command
     *
     * \pre \a argumentIndex must be in valid range
     */
    [[deprecated]]
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

    [[deprecated]]
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

    [[deprecated]]
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
               % QLatin1String("      ") % compreplyStatementToListCommandOptions(command, curVariableName) % QLatin1Char('\n')
               % QLatin1String("      ;;\n\n");
      }

      if( command.hasPositionalArguments() ){
        block += casePatternsFromCommandPositionalArgumentList(command, curVariableName);
      }

      return block;
    }

    [[deprecated]]
    static
    QString generateMainCommandCasesBlock(const BashCompletionGenerator & generator, const QString & curVariableName)
    {
      return generateCommandCasesBlock( generator.mainCommand(), curVariableName );
    }

    [[deprecated]]
    static
    QString commandListToWordListString(const BashCompletionGenerator & generator)
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
    [[deprecated]]
    static
    QString compreplyStatementForCommandCaseBlock(const BashCompletionGenerator & generator, const QString & curVariableName)
    {
      assert( generator.hasSubCommands() );

      QString wordList;
      if( generator.mainCommand().hasOptions() ){
        wordList = commandOptionsToWordListString( generator.mainCommand() ) % QLatin1Char(' ');
      }
      wordList += commandListToWordListString(generator);

      return QLatin1String("COMPREPLY=($(compgen -W \"") % wordList % QLatin1String("\" -- \"$") % curVariableName % QLatin1String("\"))");
    }

    [[deprecated]]
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

    [[deprecated]]
    static
    QString generateSubCommandsCasesBlocks(const BashCompletionGenerator & generator, const QString & curVariableName)
    {
      QString block;

      for( const auto & command : generator.subCommands() ){
        block += generateCommandCasesBlock(command, curVariableName);
      }

      return block;
    }

    [[deprecated]]
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

    [[deprecated]]
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
