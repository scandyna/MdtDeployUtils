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

    /*! \internal Map \a argumentType to corresponding CompgenAction
     *
     * \pre \a argumentType must not be Unspecified
     */
    static
    BashCompletion::CompgenAction compgenActionFromValueType(ValueType valueType) noexcept
    {
      assert( valueType != ValueType::Unspecified );

      using BashCompletion::CompgenAction;

      switch(valueType){
        case ValueType::File:
        case ValueType::DirectoryOrFile:
          return CompgenAction::ListFiles;
        case ValueType::Directory:
          return CompgenAction::ListDirectories;
        case ValueType::Unspecified:
          break;
      }

      // Should never be reached (just make the compiler happy)
      return CompgenAction::ListFiles;
    }

    /*! \internal Get a compgen command from \a option
     */
    inline
    BashCompletion::CompgenCommand compgenCommandFromParserDefinitionOption(const ParserDefinitionOption & option) noexcept
    {
      BashCompletion::CompgenCommand compgenCommand;

      if( !option.hasValueName() ){
        return compgenCommand;
      }

      if( option.valueType() != ValueType::Unspecified ){
        compgenCommand.addAction( compgenActionFromValueType( option.valueType() ) );
      }

      if( option.hasPossibleValues() ){
        compgenCommand.addWordList( option.possibleValues() );
      }

      return compgenCommand;
    }

    /*! \internal Get a compgen command from \a argument
     */
    inline
    BashCompletion::CompgenCommand compgenCommandFromParserDefinitionPositionalArgument(const ParserDefinitionPositionalArgument & argument) noexcept
    {
      BashCompletion::CompgenCommand compgenCommand;

      if( argument.valueType() != ValueType::Unspecified ){
        compgenCommand.addAction( compgenActionFromValueType( argument.valueType() ) );
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
    inline
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

  } // namespace Impl{

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_IMPL_BASH_COMPLETION_GENERATOR_H
