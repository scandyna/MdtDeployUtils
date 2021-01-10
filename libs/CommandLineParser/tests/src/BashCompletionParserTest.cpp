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
#include "Mdt/CommandLineParser/BashCompletionParser.h"
#include "Mdt/CommandLineParser/BashCompletionParserQuery.h"
#include "Mdt/CommandLineParser/Impl/BashCompletionParser.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/Parser.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>

// #include "Mdt/CommandLineParser/ParserResultDebug.h"
// #include <iostream>

using namespace Mdt::CommandLineParser;

bool parseArgumentsToResult(const ParserDefinition & parserDefinition, const QStringList & arguments, ParserResult & result)
{
  Parser parser;

  result = parser.parse(parserDefinition, arguments);

  return !result.hasError();
}

QString completionFindCurrentPositionalArgumentNameString()
{
  return BashCompletionParserQuery::findCurrentPositionalArgumentNameString();
}

TEST_CASE("BashCompletionParserQuery_parserResultWithoutQueryArguments")
{
  ParserResult queryResult;

  ParserResultCommand mainCommand;
  mainCommand.addOption( ParserResultOption( QLatin1String("verbose") ) );
  mainCommand.addPositionalArgument( completionFindCurrentPositionalArgumentNameString() );
  mainCommand.addPositionalArgument( QLatin1String("2") );
  mainCommand.addPositionalArgument( QLatin1String("myapp") );
  mainCommand.addPositionalArgument( QLatin1String("mode-a") );
  queryResult.setMainCommand(mainCommand);

  ParserResultCommand copyCommand( QLatin1String("copy") );
  copyCommand.addOption( ParserResultOption( QLatin1String("force") ) );
  copyCommand.addPositionalArgument( QLatin1String("file.txt") );
  copyCommand.addPositionalArgument( QLatin1String("/tmp") );
  queryResult.setSubCommand(copyCommand);

  const auto result = BashCompletionParserQuery::parserResultWithoutQueryArguments(queryResult);
  REQUIRE( result.mainCommand().optionCount() == 1 );
  REQUIRE( result.mainCommand().options()[0].name() == QLatin1String("verbose") );
  REQUIRE( result.mainCommand().positionalArgumentCount() == 1 );
  REQUIRE( result.mainCommand().positionalArgumentAt(0) == QLatin1String("mode-a") );
  REQUIRE( result.subCommand().name() == QLatin1String("copy") );
  REQUIRE( result.subCommand().optionCount() == 1 );
  REQUIRE( result.subCommand().options()[0].name() == QLatin1String("force") );
  REQUIRE( result.subCommand().positionalArgumentCount() == 2 );
  REQUIRE( result.subCommand().positionalArgumentAt(0) == QLatin1String("file.txt") );
  REQUIRE( result.subCommand().positionalArgumentAt(1) == QLatin1String("/tmp") );
}

TEST_CASE("BashCompletionParserQuery")
{
  ParserDefinition parserDefinition;
  QStringList arguments{QLatin1String("myapp"),completionFindCurrentPositionalArgumentNameString()};
  ParserResult result;

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() < 0 );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp -")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp")  << QLatin1String("-");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() < 0 );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    /// Currently not supported because QCommandLineParser or Bash filters -- out
//     SECTION("myapp completion-find-current-positional-argument-name 1 myapp --")
//     {
//       arguments << QLatin1String("1") << QLatin1String("myapp")  << QLatin1String("--");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
//       REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
//       REQUIRE( query.compLineSubCommandNamePositionIndex() < 0 );
//       REQUIRE( !query.isCursorInSubCommand() );
//       REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
//       REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
//       REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
//       REQUIRE( query.isCursorAtMainCommandOption() );
//       REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
//       REQUIRE( !query.isCursorAtSubCommandOption() );
//     }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp -h")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() < 0 );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp -h")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() < 0 );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp file.txt")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("file.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() < 0 );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 1 );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 1 );
      REQUIRE( query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }
  }

  SECTION("App with subCommand")
  {
    parserDefinition.addHelpOption();

    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.addHelpOption();
    copyCommand.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    copyCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
    parserDefinition.addSubCommand(copyCommand);

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() == 1 );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp -h")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() == 2 );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp copy")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("copy");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() == 1 );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp co (user is typing)")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("co");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() == 1 );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 1 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 3 myapp copy file.txt")
    {
      arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 3 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() == 1 );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 1 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() == 1 );
      REQUIRE( query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 3 myapp copy -h")
    {
      arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 3 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() == 1 );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp copy -h")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() == 1 );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( query.isCursorAtSubCommandOption() );
    }

    /// Currently not supported because QCommandLineParser or Bash filters -- out
//     SECTION("myapp completion-find-current-positional-argument-name 2 myapp unknown")
//     {
//       arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("unknown");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
//       REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
//       REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
//       REQUIRE( query.compLineSubCommandNamePositionIndex() < 0 );
//       REQUIRE( !query.isCursorInSubCommand() );
//       REQUIRE( query.mainCommandPositionalArgumentsCount() == 1 );
//       REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 1 );
//       REQUIRE( query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
//       REQUIRE( !query.isCursorAtMainCommandOption() );
//       REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
//       REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
//       REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
//       REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
//       REQUIRE( !query.isCursorAtSubCommandOption() );
//     }

  }

  SECTION("App with main arg and subCommand")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( QLatin1String("main-arg"), QLatin1String("Some main argument") );

    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.addHelpOption();
    copyCommand.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    copyCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
    parserDefinition.addSubCommand(copyCommand);

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
//       REQUIRE( query.compLineSubCommandNamePositionIndex() == 1 );
//       REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp arg1")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("arg1");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
//       REQUIRE( query.compLineSubCommandNamePositionIndex() == 2 );
//       REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 1 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp arg1")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("arg1");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
//       REQUIRE( query.compLineSubCommandNamePositionIndex() == 2 );
//       REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 1 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp copy")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("copy");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      BashCompletionParserQuery query(result, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineCouldBeMainCommandPositionalArgumentOrSubCommandName() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() == 1 );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.subCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtSubCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

  }
}

TEST_CASE("findCurrentPositionalArgumentName")
{
  using Impl::findCurrentPositionalArgumentName;
//   using Impl::completionFindCurrentPositionalArgumentNameString;

  ParserDefinition parserDefinition;
  QStringList arguments{QLatin1String("myapp"),completionFindCurrentPositionalArgumentNameString()};
  ParserResult result;

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("source") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp -")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("-");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("options") );
    }

    /// Currently not supported because QCommandLineParser or Bash filters -- out
//     SECTION("myapp completion-find-current-positional-argument-name 1 myapp --")
//     {
//       arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("--");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("options") );
//     }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp -h")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("options") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp -h")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("source") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp file.txt")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("file.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("destination") );
    }

    // We still want completion here
    SECTION("myapp completion-find-current-positional-argument-name 2 myapp file.txt /tmp")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("destination") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 3 myapp file.txt /tmp")
    {
      arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition).isEmpty() );
    }
  }

  SECTION("App with subCommand")
  {
    parserDefinition.addHelpOption();

    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.addHelpOption();
    copyCommand.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    copyCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
    parserDefinition.addSubCommand(copyCommand);

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("command") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp -")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("-");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("options") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp -h")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("command") );
    }

    /// \todo seems invalid
    SECTION("myapp completion-find-current-positional-argument-name 2 myapp unknown")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("unknown");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition).isEmpty() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp co (user is typing)")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("co");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("command") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp copy")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("copy");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-source") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp copy -")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("-");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-options") );
    }

    /// Currently not supported because QCommandLineParser or Bash filters -- out
//     SECTION("myapp completion-find-current-positional-argument-name 2 myapp copy --")
//     {
//       arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("--");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-options") );
//     }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp copy -h")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-options") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 3 myapp copy -h")
    {
      arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-source") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 3 myapp copy file.txt")
    {
      arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-destination") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 3 myapp copy file.txt /tmp")
    {
      arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-destination") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 4 myapp copy file.txt /tmp")
    {
      arguments << QLatin1String("4") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition).isEmpty() );
    }
  }

  SECTION("App with main positional arg and subCommand")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("file"), QLatin1String("File to threat") );

    ParserDefinitionCommand catCommand( QLatin1String("cat") );
    catCommand.addHelpOption();
    catCommand.addPositionalArgument( ArgumentType::File, QLatin1String("file1"), QLatin1String("File to cat") );
    parserDefinition.addSubCommand(catCommand);

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("file-or-command") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp file.txt")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("file.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("command") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 3 myapp file.txt cat")
    {
      arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("file.txt") << QLatin1String("cat");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("cat-file1") );
    }

    SECTION("myapp completion-find-current-positional-argument-name 4 myapp file.txt cat other.txt")
    {
      arguments << QLatin1String("4") << QLatin1String("myapp") << QLatin1String("file.txt") << QLatin1String("cat") << QLatin1String("other.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition).isEmpty() );
    }
  }
}

TEST_CASE("handleBashCompletion")
{
//   using Impl::completionFindCurrentPositionalArgumentNameString;

  ParserDefinition parserDefinition;
  QStringList arguments{QLatin1String("myapp")};
  ParserResult result;

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    SECTION("myapp")
    {
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !handleBashCompletion(result, parserDefinition) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << completionFindCurrentPositionalArgumentNameString() << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( handleBashCompletion(result, parserDefinition) );
    }
  }
}
