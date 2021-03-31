/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "Mdt/CommandLineParser/BashCompletionGenerator.h"
#include "Mdt/CommandLineParser/Impl/BashCompletionGenerator.h"
#include "Mdt/CommandLineParser/BashCompletion/CompgenArgumentAlgorithm.h"
#include "Mdt/CommandLineParser/BashCompletion/ScriptCaseClause.h"
#include "Mdt/CommandLineParser/BashCompletion/Script.h"
#include <QCommandLineParser>
#include <QTemporaryDir>
#include <QDir>
#include <QString>
#include <QLatin1String>
#include <QByteArray>
#include <QFileInfo>
#include <iostream>

using namespace Mdt::CommandLineParser;

bool fileExists(const QTemporaryDir & dir, const QString & fileName)
{
  const QString filePath = QDir::cleanPath(dir.path() + QLatin1String("/") + fileName);

  return QFileInfo::exists(filePath);
}

void printScriptToConsole(const BashCompletion::Script & script)
{
  std::cout << "***** Script *****\n";
  std::cout << script.toString().toLocal8Bit().toStdString() << '\n';
  std::cout << "***** END script *****" << std::endl;
}

TEST_CASE("option")
{
  SECTION("short name and name")
  {
    BashCompletionGeneratorOption option( 'h', QLatin1String("help") );
    REQUIRE( option.hasShortName() );
    REQUIRE( option.shortNameWithDash() == QLatin1String("-h") );
    REQUIRE( option.nameWithDashes() == QLatin1String("--help") );
  }

  SECTION("name only")
  {
    BashCompletionGeneratorOption option( QLatin1String("help") );
    REQUIRE( !option.hasShortName() );
    REQUIRE( option.nameWithDashes() == QLatin1String("--help") );
  }

  SECTION("Set action")
  {
    BashCompletion::Action action;
    action.setCustomAction( QLatin1String("\"$executable\" completion-list-packages") );

    BashCompletionGeneratorOption option( QLatin1String("package") );
    REQUIRE( !option.hasAction() );

    option.setAction(action);
    REQUIRE( option.hasAction() );
    REQUIRE( option.action().isCustomAction() );
  }
}

TEST_CASE("Argument_OLD")
{
  BashCompletionGeneratorPositionalArgument argument( ArgumentType::Directory, QLatin1String("destination") );
  REQUIRE( argument.type() == ArgumentType::Directory );
  REQUIRE( argument.name() == QLatin1String("destination") );
}

TEST_CASE("PositionalArgument")
{
  SECTION("Constrcut")
  {
    BashCompletionGeneratorPositionalArgument argument( QLatin1String("destination") );
    REQUIRE( argument.type() == ArgumentType::Unspecified );
    REQUIRE( argument.name() == QLatin1String("destination") );
    REQUIRE( !argument.hasAction() );
  }
}

TEST_CASE("Command_arguments_OLD")
{
  BashCompletionGeneratorCommand command( QLatin1String("command") );
  command.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
  REQUIRE( command.positionalArguments().size() == 1 );
  REQUIRE( command.positionalArguments()[0].type() == ArgumentType::File );
  REQUIRE( command.positionalArguments()[0].name() == QLatin1String("source") );
}

TEST_CASE("Command_arguments")
{
  BashCompletionGeneratorCommand command( QLatin1String("command") );
  command.addPositionalArgument( QLatin1String("source") );
  REQUIRE( command.positionalArguments().size() == 1 );
  REQUIRE( command.positionalArguments()[0].name() == QLatin1String("source") );
}

TEST_CASE("Command_isEmpty")
{
  BashCompletionGeneratorCommand command( QLatin1String("command") );
  REQUIRE( command.isEmpty() );

  SECTION("arguments")
  {
    command.addPositionalArgument( QLatin1String("arg-a") );
    REQUIRE( !command.isEmpty() );
  }

  SECTION("options")
  {
    command.addOption( 'h', QLatin1String("help") );
    REQUIRE( !command.isEmpty() );
  }

  SECTION("arguments and options")
  {
    command.addPositionalArgument( QLatin1String("arg-a") );
    command.addOption( 'h', QLatin1String("help") );
    REQUIRE( !command.isEmpty() );
  }
}

TEST_CASE("ApplicationName")
{
  BashCompletionGenerator generator;
  REQUIRE( generator.applicationName().isEmpty() );

  generator.setApplicationName( QLatin1String("mytool") );
  REQUIRE( generator.applicationName() == QLatin1String("mytool") );
}

TEST_CASE("compgenActionNameFromArgumentType")
{
  using Impl::compgenActionNameFromArgumentType;

  SECTION("file")
  {
    REQUIRE( compgenActionNameFromArgumentType(ArgumentType::File) == QLatin1String("file") );
  }

  SECTION("directory")
  {
    REQUIRE( compgenActionNameFromArgumentType(ArgumentType::Directory) == QLatin1String("directory") );
  }

  SECTION("diretory or file")
  {
    REQUIRE( compgenActionNameFromArgumentType(ArgumentType::DirectoryOrFile) == QLatin1String("file") );
  }

  SECTION("Unspecified")
  {
    REQUIRE( compgenActionNameFromArgumentType(ArgumentType::Unspecified).isEmpty() );
  }
}

TEST_CASE("compgenActionFromArgumentType")
{
  using Impl::compgenActionFromArgumentType;
  using BashCompletion::CompgenAction;

  SECTION("file")
  {
    REQUIRE( compgenActionFromArgumentType(ArgumentType::File) == CompgenAction::ListFiles );
  }

  SECTION("directory")
  {
    REQUIRE( compgenActionFromArgumentType(ArgumentType::Directory) == CompgenAction::ListDirectories );
  }

  SECTION("diretory or file")
  {
    REQUIRE( compgenActionFromArgumentType(ArgumentType::DirectoryOrFile) == CompgenAction::ListFiles );
  }
}

TEST_CASE("compgenCommandFromParserDefinitionOption")
{
  using Impl::compgenCommandFromParserDefinitionOption;
  using BashCompletion::compgenArgumentToString;

  SECTION("Option without value")
  {
    ParserDefinitionOption verboseOptionDef( 'v', QLatin1String("verbose"), QLatin1String("Verbose option") );

    const auto compgenCommand = compgenCommandFromParserDefinitionOption(verboseOptionDef);
    REQUIRE( compgenCommand.isEmpty() );
  }

  SECTION("Option with file path value")
  {
    ParserDefinitionOption fileOptionDef( QLatin1String("file"), QLatin1String("File") );
    fileOptionDef.setValueName( QLatin1String("path") );
    fileOptionDef.setValueType(ArgumentType::File);

    const auto compgenCommand = compgenCommandFromParserDefinitionOption(fileOptionDef);
    REQUIRE( compgenCommand.argumentCount() == 1 );
    REQUIRE( compgenArgumentToString( compgenCommand.argumentAt(0) ) == QLatin1String("-A file") );
  }

  SECTION("Option with list of possible values")
  {
    ParserDefinitionOption optionDef( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
    optionDef.setValueName( QLatin1String("behavior") );
    optionDef.setPossibleValues({QLatin1String("keep"),QLatin1String("fail"),QLatin1String("overwrite")});

    const auto compgenCommand = compgenCommandFromParserDefinitionOption(optionDef);
    REQUIRE( compgenCommand.argumentCount() == 1 );
    REQUIRE( compgenArgumentToString( compgenCommand.argumentAt(0) ) == QLatin1String("-W \"keep fail overwrite\"") );
  }
}

TEST_CASE("BashCompletionGeneratorOption_fromParserDefinitionOption")
{
  using BashCompletion::isActionVariantCompgenCommand;
  using BashCompletion::isActionVariantCustomAction;

  ParserDefinitionOption verboseOptionDef( 'v', QLatin1String("verbose"), QLatin1String("Verbose option") );

  SECTION("Check that name and short name are correctly taken")
  {
    const auto verboseOption = BashCompletionGeneratorOption::fromParserDefinitionOption(verboseOptionDef);
    REQUIRE( verboseOption.shortNameWithDash() == QLatin1String("-v") );
    REQUIRE( verboseOption.nameWithDashes() == QLatin1String("--verbose") );
  }

  SECTION("Action mapping")
  {
    SECTION("Option without value")
    {
      const auto verboseOption = BashCompletionGeneratorOption::fromParserDefinitionOption(verboseOptionDef);
      REQUIRE( !verboseOption.hasAction() );
    }

    SECTION("Option with file path value")
    {
      ParserDefinitionOption fileOptionDef( QLatin1String("file"), QLatin1String("File") );
      fileOptionDef.setValueName( QLatin1String("path") );
      fileOptionDef.setValueType(ArgumentType::File);

      const auto fileOption = BashCompletionGeneratorOption::fromParserDefinitionOption(fileOptionDef);
      REQUIRE( fileOption.hasAction() );
      REQUIRE( fileOption.action().isCompgenCommand() );
    }
  }
}

TEST_CASE("compgenCommandFromParserDefinitionPositionalArgument")
{
  using Impl::compgenCommandFromParserDefinitionPositionalArgument;
  using BashCompletion::compgenArgumentToString;

  SECTION("Argument with unspecified value type")
  {
    ParserDefinitionPositionalArgument argumentDef( ArgumentType::Unspecified, QLatin1String("source"), QLatin1String("Source file") );

    const auto compgenCommand = compgenCommandFromParserDefinitionPositionalArgument(argumentDef);
    REQUIRE( compgenCommand.isEmpty() );
  }

  SECTION("Argument with file value type")
  {
    ParserDefinitionPositionalArgument argumentDef( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );

    const auto compgenCommand = compgenCommandFromParserDefinitionPositionalArgument(argumentDef);
    REQUIRE( compgenCommand.argumentCount() == 1 );
    REQUIRE( compgenArgumentToString( compgenCommand.argumentAt(0) ) == QLatin1String("-A file") );
  }

  SECTION("Argument with list of possible values")
  {
    ParserDefinitionPositionalArgument argumentDef( ArgumentType::Unspecified, QLatin1String("file-type"), QLatin1String("File type") );
    argumentDef.setPossibleValues({QLatin1String("executable"),QLatin1String("library")});

    const auto compgenCommand = compgenCommandFromParserDefinitionPositionalArgument(argumentDef);
    REQUIRE( compgenCommand.argumentCount() == 1 );
    REQUIRE( compgenArgumentToString( compgenCommand.argumentAt(0) ) == QLatin1String("-W \"executable library\"") );
  }
}

TEST_CASE("BashCompletionGeneratorPositionalArgument_fromParserDefinitionPositionalArgument")
{
  SECTION("Check that name is correctly taken")
  {
    ParserDefinitionPositionalArgument argumentDef( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );

    const auto argument = BashCompletionGeneratorPositionalArgument::fromParserDefinitionPositionalArgument(argumentDef);
//     REQUIRE( argument.type() == ArgumentType::File );
    REQUIRE( argument.name() == QLatin1String("source") );
  }

  SECTION("Action mapping")
  {
    SECTION("Argument with unspecified value type -> no action")
    {
      ParserDefinitionPositionalArgument argumentDef( ArgumentType::Unspecified, QLatin1String("source"), QLatin1String("Source file") );

      const auto argument = BashCompletionGeneratorPositionalArgument::fromParserDefinitionPositionalArgument(argumentDef);
      REQUIRE( !argument.hasAction() );
    }

    SECTION("Argument with File value type -> compgen -A file action")
    {
      ParserDefinitionPositionalArgument argumentDef( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );

      const auto argument = BashCompletionGeneratorPositionalArgument::fromParserDefinitionPositionalArgument(argumentDef);
      REQUIRE( argument.hasAction() );
      REQUIRE( argument.action().isCompgenCommand() );
    }
  }
}

TEST_CASE("BashCompletionGeneratorCommand_fromParserDefinitionCommand")
{
  SECTION("No name (must not assert)")
  {
    ParserDefinitionCommand commandDef;

    const auto copyCommand = BashCompletionGeneratorCommand::fromParserDefinitionCommand(commandDef);
    REQUIRE( copyCommand.name().isEmpty() );
  }

  SECTION("With name, options and positional arguments")
  {
    ParserDefinitionCommand copyCommandDef( QLatin1String("copy") );
    copyCommandDef.setDescription( QLatin1String("Copy a file") );
    copyCommandDef.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file to copy") );
    copyCommandDef.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
    copyCommandDef.addHelpOption();

    const auto copyCommand = BashCompletionGeneratorCommand::fromParserDefinitionCommand(copyCommandDef);
    REQUIRE( copyCommand.name() == QLatin1String("copy") );
    REQUIRE( copyCommand.positionalArguments().size() == 2 );
    REQUIRE( copyCommand.positionalArguments()[0].type() == ArgumentType::File );
    REQUIRE( copyCommand.positionalArguments()[0].name() == QLatin1String("source") );
    REQUIRE( copyCommand.positionalArguments()[1].type() == ArgumentType::Directory );
    REQUIRE( copyCommand.positionalArguments()[1].name() == QLatin1String("destination") );
    REQUIRE( copyCommand.options().size() == 1 );
    REQUIRE( copyCommand.options()[0].shortNameWithDash() == QLatin1String("-h") );
    REQUIRE( copyCommand.options()[0].nameWithDashes() == QLatin1String("--help") );
  }
}

TEST_CASE("fromParserDefinition")
{
  ParserDefinition parserDefinition;
  parserDefinition.setApplicationName( QLatin1String("myapp") );
  parserDefinition.setApplicationDescription( QLatin1String("My cool app") );
  parserDefinition.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("arg1"), QLatin1String("Some argument") );
  parserDefinition.addHelpOption();

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.setDescription( QLatin1String("Copy a file") );
  copyCommand.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file to copy") );
  copyCommand.addPositionalArgument( QLatin1String("destination"), QLatin1String("Destination directory") );
  copyCommand.addHelpOption();
  parserDefinition.addSubCommand(copyCommand);

  auto generator = BashCompletionGenerator::fromParserDefinition(parserDefinition);

  REQUIRE( generator.applicationName() == QLatin1String("myapp") );
  REQUIRE( generator.mainCommand().options().size() == 1 );
  REQUIRE( generator.mainCommand().options()[0].shortNameWithDash() == QLatin1String("-h") );
  REQUIRE( generator.mainCommand().options()[0].nameWithDashes() == QLatin1String("--help") );
  REQUIRE( generator.mainCommand().positionalArgumentCount() == 1 );

  REQUIRE( generator.subCommands().size() == 1 );
  REQUIRE( generator.subCommands()[0].name() == QLatin1String("copy") );
  REQUIRE( generator.subCommands()[0].options().size() == 1 );
  REQUIRE( generator.subCommands()[0].positionalArgumentCount() == 2 );
}

TEST_CASE("caseClauseForOptionNameValue")
{
  using Impl::caseClauseForOptionNameValue;
  using Impl::caseClauseForOptionShortNameValue;
  using BashCompletion::CompgenAction;

  BashCompletion::Action action;
  BashCompletion::CompgenCommand compgenCommand;

  BashCompletionGeneratorCommand mainCommand;
  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );

  SECTION("--overwrite-behavior")
  {
    BashCompletionGeneratorOption option( QLatin1String("overwrite-behavior") );
    compgenCommand.addWordList({QLatin1String("keep"),QLatin1String("overwrite"),QLatin1String("fail")});
    action.setCompgenCommand(compgenCommand);
    option.setAction(action);

    const auto caseClause = caseClauseForOptionNameValue(mainCommand, option);
    REQUIRE( caseClause.pattern() == QLatin1String("option-overwrite-behavior-value") );
    REQUIRE( !caseClause.command().isEmpty() );
  }

  SECTION("-f --file")
  {
    BashCompletionGeneratorOption option( 'f', QLatin1String("file") );
    compgenCommand.addAction(CompgenAction::ListFiles);
    action.setCompgenCommand(compgenCommand);
    option.setAction(action);

    SECTION("option-f-value)")
    {
      const auto caseClause = caseClauseForOptionShortNameValue(mainCommand, option);
      REQUIRE( caseClause.pattern() == QLatin1String("option-f-value") );
      REQUIRE( !caseClause.command().isEmpty() );
    }

    SECTION("option-file-value)")
    {
      const auto caseClause = caseClauseForOptionNameValue(mainCommand, option);
      REQUIRE( caseClause.pattern() == QLatin1String("option-file-value") );
      REQUIRE( !caseClause.command().isEmpty() );
    }

    SECTION("copy-option-f-value)")
    {
      const auto caseClause = caseClauseForOptionShortNameValue(copyCommand, option);
      REQUIRE( caseClause.pattern() == QLatin1String("copy-option-f-value") );
      REQUIRE( !caseClause.command().isEmpty() );
    }

    SECTION("copy-option-file-value)")
    {
      const auto caseClause = caseClauseForOptionNameValue(copyCommand, option);
      REQUIRE( caseClause.pattern() == QLatin1String("copy-option-file-value") );
      REQUIRE( !caseClause.command().isEmpty() );
    }
  }
}

TEST_CASE("caseClauseForPositionalArgumentName")
{
  using Impl::caseClauseForPositionalArgumentName;
  using BashCompletion::CompgenAction;

  BashCompletion::CompgenCommand listFilesCommand;
  listFilesCommand.addAction(CompgenAction::ListFiles);
  BashCompletion::Action listFilesAction;
  listFilesAction.setCompgenCommand(listFilesCommand);

  BashCompletionGeneratorCommand mainCommand;
  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );

  SECTION("source")
  {
    BashCompletionGeneratorPositionalArgument argument( QLatin1String("source") );
    argument.setAction(listFilesAction);

    const auto caseClause = caseClauseForPositionalArgumentName(mainCommand, argument);
    REQUIRE( caseClause.pattern() == QLatin1String("source") );
    REQUIRE( !caseClause.command().isEmpty() );
  }

  SECTION("copy-source")
  {
    BashCompletionGeneratorPositionalArgument argument( QLatin1String("source") );
    argument.setAction(listFilesAction);

    const auto caseClause = caseClauseForPositionalArgumentName(copyCommand, argument);
    REQUIRE( caseClause.pattern() == QLatin1String("copy-source") );
    REQUIRE( !caseClause.command().isEmpty() );
  }
}

TEST_CASE("commandOptionsToWordList","[.]")
{
  REQUIRE(false);
}

TEST_CASE("caseClauseToListCommandOptions")
{
  using Impl::caseClauseToListCommandOptions;

  SECTION("Main command ")
  {
    BashCompletionGeneratorCommand command;

    SECTION("--help")
    {
      command.addOption( QLatin1String("help") );

      const auto caseClause = caseClauseToListCommandOptions(command);
      REQUIRE( caseClause.pattern() == QLatin1String("options") );
      REQUIRE( !caseClause.command().isEmpty() );
    }

    SECTION("-h --help")
    {
      command.addOption( 'h', QLatin1String("help") );

      const auto caseClause = caseClauseToListCommandOptions(command);
      REQUIRE( caseClause.pattern() == QLatin1String("options") );
      REQUIRE( !caseClause.command().isEmpty() );
    }
  }

  SECTION("copy command")
  {
    BashCompletionGeneratorCommand command( QLatin1String("copy") );

    SECTION("--help")
    {
      command.addOption( QLatin1String("help") );

      const auto caseClause = caseClauseToListCommandOptions(command);
      REQUIRE( caseClause.pattern() == QLatin1String("copy-options") );
      REQUIRE( !caseClause.command().isEmpty() );
    }
  }
}

TEST_CASE("caseClauseForOptionsOrPositionalArgumentName")
{
  using Impl::caseClauseForOptionsOrPositionalArgumentName;
  using BashCompletion::CompgenAction;

  BashCompletion::Action action;
  BashCompletion::CompgenCommand compgenCommand;
  compgenCommand.addAction(CompgenAction::ListFiles);
  action.setCompgenCommand(compgenCommand);

  BashCompletionGeneratorPositionalArgument source( QLatin1String("source") );
  source.setAction(action);

  SECTION("Main command ")
  {
    BashCompletionGeneratorCommand command;
    command.addPositionalArgument(source);

    SECTION("--help")
    {
      command.addOption( QLatin1String("help") );

      const auto caseClause = caseClauseForOptionsOrPositionalArgumentName(command, source);
      REQUIRE( caseClause.pattern() == QLatin1String("options-or-source") );
      REQUIRE( !caseClause.command().isEmpty() );
    }
  }

  SECTION("copy command")
  {
    BashCompletionGeneratorCommand command( QLatin1String("copy") );
    command.addPositionalArgument(source);

    SECTION("--help")
    {
      command.addOption( QLatin1String("help") );

      const auto caseClause = caseClauseForOptionsOrPositionalArgumentName(command, source);
      REQUIRE( caseClause.pattern() == QLatin1String("copy-options-or-source") );
      REQUIRE( !caseClause.command().isEmpty() );
    }
  }
}

TEST_CASE("caseClauseToListAvailableSubCommands")
{
  using Impl::caseClauseToListAvailableSubCommands;

  BashCompletionGenerator generator;
  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  copyCommand.addPositionalArgument( QLatin1String("source") );
  generator.addSubCommand(copyCommand);

  const auto caseClause = caseClauseToListAvailableSubCommands(generator);
  REQUIRE( caseClause.pattern() == QLatin1String("commands") );
  REQUIRE( !caseClause.command().isEmpty() );
}

TEST_CASE("caseClauseToListCommandOptionsOrAvailableSubCommands")
{
  using Impl::caseClauseToListCommandOptionsOrAvailableSubCommands;

  BashCompletionGeneratorCommand mainCommand;
  mainCommand.addOption( QLatin1String("help") );

  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  copyCommand.addPositionalArgument( QLatin1String("source") );

  BashCompletionGenerator generator;
  generator.setMainCommand(mainCommand);
  generator.addSubCommand(copyCommand);

  const auto caseClause = caseClauseToListCommandOptionsOrAvailableSubCommands(generator);
  REQUIRE( caseClause.pattern() == QLatin1String("options-or-commands") );
  REQUIRE( !caseClause.command().isEmpty() );
}

TEST_CASE("addCaseClausesForCommandPositionalArgumentsToScript")
{
  using Impl::addCaseClausesForCommandPositionalArgumentsToScript;
  using BashCompletion::Action;
  using BashCompletion::CompgenAction;
  using BashCompletion::CompgenCommand;

  BashCompletion::Script script( QLatin1String("app") );
  BashCompletionGeneratorCommand mainCommand;
  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  BashCompletionGeneratorOption helpOption( 'h', QLatin1String("help") );
  BashCompletionGeneratorPositionalArgument source( QLatin1String("source") );

  CompgenCommand listFilesCommand;
  listFilesCommand.addAction(CompgenAction::ListFiles);
  Action listFilesAction;
  listFilesAction.setCompgenCommand(listFilesCommand);

  SECTION("1 positional argument without action")
  {
    mainCommand.addPositionalArgument(source);

    addCaseClausesForCommandPositionalArgumentsToScript(mainCommand, script);
    REQUIRE( script.clauseCount() == 0 );
  }

  SECTION("1 positional argument with a compgen action")
  {
    source.setAction(listFilesAction);
    mainCommand.addPositionalArgument(source);

    addCaseClausesForCommandPositionalArgumentsToScript(mainCommand, script);
    REQUIRE( script.clauseCount() == 1 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("source") );
  }

  SECTION("1 positional argument with a action in a command having a option")
  {
    source.setAction(listFilesAction);
    mainCommand.addPositionalArgument(source);
    mainCommand.addOption(helpOption);

    addCaseClausesForCommandPositionalArgumentsToScript(mainCommand, script);
    REQUIRE( script.clauseCount() == 2 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("source") );
    REQUIRE( script.clauseAt(1).pattern() == QLatin1String("options-or-source") );
  }

  SECTION("1 positional argument, in a sub-command, with action")
  {
    source.setAction(listFilesAction);
    copyCommand.addPositionalArgument(source);

    addCaseClausesForCommandPositionalArgumentsToScript(copyCommand, script);
    REQUIRE( script.clauseCount() == 1 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("copy-source") );
  }
}

TEST_CASE("addCaseClausesForCommandOptionsWithValueToScript")
{
  using Impl::addCaseClausesForCommandOptionsWithValueToScript;
  using BashCompletion::Action;
  using BashCompletion::CompgenAction;
  using BashCompletion::CompgenCommand;

  CompgenCommand compgenCommand;
  Action action;

  BashCompletionGeneratorOption overwriteBehaviorOption( QLatin1String("overwrite-behavior") );
  compgenCommand.addWordList({QLatin1String("keep"),QLatin1String("fail")});
  action.setCompgenCommand(compgenCommand);
  overwriteBehaviorOption.setAction(action);

  BashCompletionGeneratorOption fileOption( 'f', QLatin1String("file") );
  compgenCommand.addAction(CompgenAction::ListFiles);
  action.setCompgenCommand(compgenCommand);
  fileOption.setAction(action);

  BashCompletionGeneratorOption helpOption( 'h', QLatin1String("help") );

  BashCompletionGeneratorCommand mainCommand;
  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );

  BashCompletion::Script script( QLatin1String("app") );

  SECTION("help (no action)")
  {
    mainCommand.addOption(helpOption);

    addCaseClausesForCommandOptionsWithValueToScript(mainCommand, script);
    REQUIRE( script.clauseCount() == 0 );
  }

  SECTION("--overwrite-behavior")
  {
    mainCommand.addOption(overwriteBehaviorOption);

    addCaseClausesForCommandOptionsWithValueToScript(mainCommand, script);
    REQUIRE( script.clauseCount() == 1 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("option-overwrite-behavior-value") );
  }

  SECTION("-f --file")
  {
    mainCommand.addOption(fileOption);

    addCaseClausesForCommandOptionsWithValueToScript(mainCommand, script);
    REQUIRE( script.clauseCount() == 2 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("option-file-value") );
    REQUIRE( script.clauseAt(1).pattern() == QLatin1String("option-f-value") );
  }

  SECTION("copy -f --file")
  {
    copyCommand.addOption(fileOption);

    addCaseClausesForCommandOptionsWithValueToScript(copyCommand, script);
    REQUIRE( script.clauseCount() == 2 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("copy-option-file-value") );
    REQUIRE( script.clauseAt(1).pattern() == QLatin1String("copy-option-f-value") );
  }
}

TEST_CASE("addCaseClausesToScript")
{
  using Impl::addCaseClausesToScript;
  using BashCompletion::Action;
  using BashCompletion::CompgenAction;
  using BashCompletion::CompgenCommand;

  BashCompletion::Script script( QLatin1String("app") );

  CompgenCommand listFilesCommand;
  listFilesCommand.addAction(CompgenAction::ListFiles);
  Action listFilesAction;
  listFilesAction.setCompgenCommand(listFilesCommand);

  BashCompletionGeneratorPositionalArgument source( QLatin1String("source") );
  source.setAction(listFilesAction);

  BashCompletionGeneratorOption helpOption( 'h', QLatin1String("help") );

  BashCompletionGeneratorOption fileOption( QLatin1String("file") );
  fileOption.setAction(listFilesAction);

  BashCompletionGeneratorCommand mainCommand;
  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  BashCompletionGenerator generator;

  SECTION("Empty")
  {
    addCaseClausesToScript(generator, script);
    REQUIRE( script.clauseCount() == 0 );
  }

  SECTION("help option")
  {
    mainCommand.addOption(helpOption);
    generator.setMainCommand(mainCommand);

    addCaseClausesToScript(generator, script);
    REQUIRE( script.clauseCount() == 1 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("options") );
  }

  SECTION("positional argument")
  {
    mainCommand.addPositionalArgument(source);
    generator.setMainCommand(mainCommand);

    addCaseClausesToScript(generator, script);
    REQUIRE( script.clauseCount() == 1 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("source") );
  }

  SECTION("option and positional argument")
  {
    mainCommand.addOption(helpOption);
    mainCommand.addPositionalArgument(source);
    generator.setMainCommand(mainCommand);

    addCaseClausesToScript(generator, script);
    REQUIRE( script.clauseCount() == 3 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("options") );
    REQUIRE( script.clauseAt(1).pattern() == QLatin1String("source") );
    REQUIRE( script.clauseAt(2).pattern() == QLatin1String("options-or-source") );
  }

  SECTION("option with action and positional argument")
  {
    mainCommand.addOption(fileOption);
    mainCommand.addPositionalArgument(source);
    generator.setMainCommand(mainCommand);

    addCaseClausesToScript(generator, script);
    REQUIRE( script.clauseCount() == 4 );

    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("options") );
    REQUIRE( script.clauseAt(1).pattern() == QLatin1String("source") );
    REQUIRE( script.clauseAt(2).pattern() == QLatin1String("options-or-source") );
    REQUIRE( script.clauseAt(3).pattern() == QLatin1String("option-file-value") );
  }

  SECTION("copy sub-command with help option")
  {
    copyCommand.addOption(helpOption);
    generator.addSubCommand(copyCommand);

    addCaseClausesToScript(generator, script);
    REQUIRE( script.clauseCount() == 2 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("commands") );
    REQUIRE( script.clauseAt(1).pattern() == QLatin1String("copy-options") );
  }

  SECTION("option and copy sub-command")
  {
    mainCommand.addOption(helpOption);
    generator.setMainCommand(mainCommand);
    copyCommand.addOption(helpOption);
    generator.addSubCommand(copyCommand);

    addCaseClausesToScript(generator, script);
    REQUIRE( script.clauseCount() == 4 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("options") );
    REQUIRE( script.clauseAt(1).pattern() == QLatin1String("commands") );
    REQUIRE( script.clauseAt(2).pattern() == QLatin1String("options-or-commands") );
    REQUIRE( script.clauseAt(3).pattern() == QLatin1String("copy-options") );
  }

  SECTION("copy sub-command with positional argument (with action) and option with value (with action)")
  {
    copyCommand.addOption(fileOption);
    copyCommand.addPositionalArgument(source);
    generator.addSubCommand(copyCommand);

    addCaseClausesToScript(generator, script);
    REQUIRE( script.clauseCount() == 5 );
    REQUIRE( script.clauseAt(0).pattern() == QLatin1String("commands") );
    REQUIRE( script.clauseAt(1).pattern() == QLatin1String("copy-options") );
    REQUIRE( script.clauseAt(2).pattern() == QLatin1String("copy-source") );
    REQUIRE( script.clauseAt(3).pattern() == QLatin1String("copy-options-or-source") );
    REQUIRE( script.clauseAt(4).pattern() == QLatin1String("copy-option-file-value") );
  }
}

TEST_CASE("compreplyStatementToListCommandOptions")
{
  using Impl::compreplyStatementToListCommandOptions;

  BashCompletionGeneratorCommand command;
  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  SECTION("--help")
  {
    command.addOption( QLatin1String("help") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"--help\" -- \"$cur\"))");
    REQUIRE( compreplyStatementToListCommandOptions(command, curVariableName) == expectedResult );
  }

  SECTION("-h --help")
  {
    command.addOption( 'h', QLatin1String("help") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"-h --help\" -- \"$cur\"))");
    REQUIRE( compreplyStatementToListCommandOptions(command, curVariableName) == expectedResult );
  }
}

TEST_CASE("compreplyStatementFromPositionalArgument_OLD")
{
  using Impl::compreplyStatementFromPositionalArgument;

  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  SECTION("source file")
  {
    BashCompletionGeneratorPositionalArgument argument( ArgumentType::File, QLatin1String("source") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
    REQUIRE( compreplyStatementFromPositionalArgument(argument, curVariableName) == expectedResult );
  }

  SECTION("destination diretory")
  {
    BashCompletionGeneratorPositionalArgument argument( ArgumentType::Directory, QLatin1String("destination") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -A directory -- \"$cur\"))");
    REQUIRE( compreplyStatementFromPositionalArgument(argument, curVariableName) == expectedResult );
  }

  SECTION("destination diretory or file")
  {
    BashCompletionGeneratorPositionalArgument argument( ArgumentType::DirectoryOrFile, QLatin1String("destination") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
    REQUIRE( compreplyStatementFromPositionalArgument(argument, curVariableName) == expectedResult );
  }

  SECTION("somearg Unspecified")
  {
    BashCompletionGeneratorPositionalArgument argument( ArgumentType::Unspecified, QLatin1String("somearg") );
    expectedResult = QLatin1String("");
    REQUIRE( compreplyStatementFromPositionalArgument(argument, curVariableName) == expectedResult );
  }
}

TEST_CASE("compreplyStatementFromFirstPositionalArgumentInCommand")
{
  using Impl::compreplyStatementFromFirstPositionalArgumentInCommand;

  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  BashCompletionGeneratorCommand command;
  BashCompletionGeneratorPositionalArgument sourceFile( ArgumentType::File, QLatin1String("source") );
  BashCompletionGeneratorPositionalArgument someArg( ArgumentType::Unspecified, QLatin1String("somearg") );

  SECTION("command without option")
  {
    SECTION("source file")
    {
      command.addPositionalArgument(sourceFile);
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
      REQUIRE( compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName) == expectedResult );
    }

    SECTION("somearg Unspecified")
    {
      command.addPositionalArgument(someArg);
      REQUIRE( compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName).isEmpty() );
    }
  }

  SECTION("command with -h --help option")
  {
    command.addOption( 'h', QLatin1String("help") );

    SECTION("source file")
    {
      command.addPositionalArgument(sourceFile);
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -W \"-h --help\" -- \"$cur\"))");
      REQUIRE( compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName) == expectedResult );
    }

    SECTION("somearg Unspecified")
    {
      command.addPositionalArgument(someArg);
      expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"-h --help\" -- \"$cur\"))");
      REQUIRE( compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName) == expectedResult );
    }
  }
}

TEST_CASE("compreplyStatementFromPositionalArgumentInCommand")
{
  using Impl::compreplyStatementFromPositionalArgumentInCommand;

  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  BashCompletionGeneratorCommand command;
  command.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
  command.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination") );
  command.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("somearg") );

  SECTION("command without option")
  {
    SECTION("source file")
    {
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 0, curVariableName) == expectedResult );
    }

    SECTION("destination diretory")
    {
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A directory -- \"$cur\"))");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 1, curVariableName) == expectedResult );
    }

    SECTION("somearg Unspecified")
    {
      expectedResult = QLatin1String("");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 2, curVariableName) == expectedResult );
    }
  }

  SECTION("command with -h --help option")
  {
    command.addOption( 'h', QLatin1String("help") );

    SECTION("source file")
    {
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -W \"-h --help\" -- \"$cur\"))");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 0, curVariableName) == expectedResult );
    }

    SECTION("destination diretory")
    {
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A directory -- \"$cur\"))");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 1, curVariableName) == expectedResult );
    }

    SECTION("somearg Unspecified")
    {
      expectedResult = QLatin1String("");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 2, curVariableName) == expectedResult );
    }
  }
}

TEST_CASE("formatCasePatternBlock")
{
  using Impl::formatCasePatternBlock;

  QString expectedResult;

  expectedResult = QLatin1String("    pattern)\n      body\n      ;;\n");
  REQUIRE( formatCasePatternBlock( QLatin1String("pattern"), QLatin1String("body") ) == expectedResult );
}

TEST_CASE("casePatternFromCommandPositionalArgument")
{
  using Impl::casePatternFromCommandPositionalArgument;

  BashCompletionGeneratorCommand command;
  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  SECTION("source file")
  {
    command.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
    expectedResult = QLatin1String("    source)\n      COMPREPLY=($(compgen -A file -- \"$cur\"))\n      ;;");
    REQUIRE( casePatternFromCommandPositionalArgument(command, 0, curVariableName) == expectedResult );
  }

  SECTION("somearg Unspecified")
  {
    command.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("somearg") );
    expectedResult = QLatin1String("");
    REQUIRE( casePatternFromCommandPositionalArgument(command, 0, curVariableName) == expectedResult );
  }
}

TEST_CASE("compreplyStatementForCommandCaseBlock")
{
  using Impl::compreplyStatementForCommandCaseBlock;

  BashCompletionGenerator generator;
  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  BashCompletionGeneratorCommand mainCommand;

  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  copyCommand.addOption( 'f', QLatin1String("force") );

  BashCompletionGeneratorCommand cutCommand( QLatin1String("cut") );
  cutCommand.addOption( QLatin1String("verbose") );

  SECTION("copy")
  {
    generator.addSubCommand(copyCommand);

    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"copy\" -- \"$cur\"))");
    REQUIRE( compreplyStatementForCommandCaseBlock(generator, curVariableName) == expectedResult );
  }

  SECTION("copy cut")
  {
    generator.addSubCommand(copyCommand);
    generator.addSubCommand(cutCommand);

    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"copy cut\" -- \"$cur\"))");
    REQUIRE( compreplyStatementForCommandCaseBlock(generator, curVariableName) == expectedResult );
  }

  SECTION("--verbose copy")
  {
    mainCommand.addOption( QLatin1String("verbose") );
    generator.setMainCommand(mainCommand);
    generator.addSubCommand(copyCommand);

    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"--verbose copy\" -- \"$cur\"))");
    REQUIRE( compreplyStatementForCommandCaseBlock(generator, curVariableName) == expectedResult );
  }

  SECTION("-h --help copy")
  {
    mainCommand.addOption( 'h', QLatin1String("help") );
    generator.setMainCommand(mainCommand);
    generator.addSubCommand(copyCommand);

    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"-h --help copy\" -- \"$cur\"))");
    REQUIRE( compreplyStatementForCommandCaseBlock(generator, curVariableName) == expectedResult );
  }
}

TEST_CASE("generateCompletionFunction")
{
  using Impl::generateCompletionFunction;

  BashCompletionGenerator generator;
  generator.setApplicationName( QLatin1String("mytool") );

  REQUIRE( !generateCompletionFunction(generator).isEmpty() );
}

TEST_CASE("generateScriptToFile")
{
  using BashCompletion::Action;
  using BashCompletion::CompgenAction;
  using BashCompletion::CompgenCommand;

  QTemporaryDir dir;
  REQUIRE( dir.isValid() );

  CompgenCommand listFilesCommand;
  listFilesCommand.addAction(CompgenAction::ListFiles);
  Action listFilesAction;
  listFilesAction.setCompgenCommand(listFilesCommand);

  CompgenCommand listDirectoriesCommand;
  listDirectoriesCommand.addAction(CompgenAction::ListDirectories);
  Action listDirectoriesAction;
  listDirectoriesAction.setCompgenCommand(listDirectoriesCommand);

  BashCompletionGeneratorPositionalArgument repo( QLatin1String("repo") );
  repo.setAction(listFilesAction);

  BashCompletionGeneratorPositionalArgument source( QLatin1String("source") );
  source.setAction(listFilesAction);

  BashCompletionGeneratorPositionalArgument destination( QLatin1String("destination") );
  destination.setAction(listDirectoriesAction);

  BashCompletionGenerator generator;
  generator.setApplicationName( QLatin1String("mytool") );

  BashCompletionGeneratorCommand mainCommand;
  mainCommand.addPositionalArgument(repo);
  mainCommand.addOption( 'h', QLatin1String("help") );
  generator.setMainCommand(mainCommand);

  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  copyCommand.addPositionalArgument(source);
  copyCommand.addPositionalArgument(destination);
  copyCommand.addOption( 'h', QLatin1String("help") );
  copyCommand.addOption( 'i', QLatin1String("interactive") );
  generator.addSubCommand(copyCommand);

  std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;

  const QString fileName = QLatin1String("mytool-completion.bash");
  REQUIRE( !fileExists(dir, fileName) );
  generator.generateScriptToFile( dir.path() );
  REQUIRE( fileExists(dir, fileName) );
}

TEST_CASE("GenerateScript_sandbox")
{
  BashCompletionGenerator generator;
  generator.setApplicationName( QLatin1String("myapp") );
  BashCompletionGeneratorCommand mainCommand;

  SECTION("Simple app")
  {
    std::cout << "Simple app ";

    SECTION("with positional argument")
    {
      std::cout << "with positional argument" << std::endl;

      mainCommand.addPositionalArgument( ArgumentType::File, QLatin1String("mainfile") );
      generator.setMainCommand(mainCommand);
      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }

    SECTION("with option")
    {
      std::cout << "with option" << std::endl;

      mainCommand.addOption( 'h', QLatin1String("help") );
      generator.setMainCommand(mainCommand);
      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }

    SECTION("with option and file positional argument")
    {
      std::cout << "with option and file positional argument" << std::endl;

      mainCommand.addOption( 'h', QLatin1String("help") );
      mainCommand.addPositionalArgument( ArgumentType::File, QLatin1String("mainfile") );
      generator.setMainCommand(mainCommand);
      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }

    SECTION("with option and Unspecified positional argument")
    {
      std::cout << "with option and Unspecified positional argument" << std::endl;

      mainCommand.addOption( 'h', QLatin1String("help") );
      mainCommand.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("arg1") );
      generator.setMainCommand(mainCommand);
      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }
  }

  SECTION("App with sub-command")
  {
    std::cout << "App with sub-command ";

    SECTION("myapp [options] command [options] source destination")
    {
      std::cout << "myapp [options] command [options] source destination" << std::endl;

      mainCommand.addOption( 'h', QLatin1String("help") );
      generator.setMainCommand(mainCommand);
      BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
      copyCommand.addOption( QLatin1String("force") );
      copyCommand.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
      copyCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination") );
      generator.addSubCommand(copyCommand);

      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }
  }
}
