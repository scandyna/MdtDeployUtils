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

QString completionFindCurrentArgumentString()
{
  return BashCompletionParserQuery::findCurrentPositionalArgumentNameString();
}

TEST_CASE("BashCompletionParserQuery_isValidBashCompletionQuery_OLD")
{
  ParserDefinition parserDefinition;
  QStringList arguments;
  ParserResult result;

  SECTION("argument count validity")
  {
    SECTION("empty")
    {
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("myapp")
    {
      arguments << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("myapp completion-find-current-positional-argument-name")
    {
      arguments << QLatin1String("myapp") << completionFindCurrentPositionalArgumentNameString();
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1")
    {
      arguments << QLatin1String("myapp") << completionFindCurrentPositionalArgumentNameString() << QLatin1String("1");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << QLatin1String("myapp") << completionFindCurrentPositionalArgumentNameString() << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }
  }

  SECTION("cursor validity")
  {
    arguments << QLatin1String("myapp") << completionFindCurrentPositionalArgumentNameString();

    // The parser not accept this, because it threat -1 as a unknown option
//     SECTION("-1")
//     {
//       arguments << QLatin1String("-1") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(result) );
//     }

    SECTION("myapp completion-find-current-positional-argument-name 0 myapp")
    {
      arguments << QLatin1String("0") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp")
    {
      arguments << QLatin1String("2") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 100 myapp")
    {
      arguments << QLatin1String("100") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("A")
    {
      arguments << QLatin1String("A") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }

    SECTION("1")
    {
      arguments << QLatin1String("1") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(result) );
    }
  }
}

TEST_CASE("BashCompletionParserQuery_isValidBashCompletionQuery")
{
  CommandLine::CommandLine commandLine;

  SECTION("argument count validity")
  {
    SECTION("empty")
    {
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("myapp")
    {
      commandLine.setExecutableName( QLatin1String("myapp") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("myapp completion-find-current-positional-argument-name")
    {
      commandLine.setExecutableName( QLatin1String("myapp") );
      commandLine.appendPositionalArgument( completionFindCurrentPositionalArgumentNameString() );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1")
    {
      commandLine.setExecutableName( QLatin1String("myapp") );
      commandLine.appendPositionalArgument( completionFindCurrentPositionalArgumentNameString() );
      commandLine.appendPositionalArgument( QLatin1String("1") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      commandLine.setExecutableName( QLatin1String("myapp") );
      commandLine.appendPositionalArgument( completionFindCurrentPositionalArgumentNameString() );
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }
  }

  SECTION("cursor validity")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendPositionalArgument( completionFindCurrentPositionalArgumentNameString() );

    SECTION("-1")
    {
      commandLine.appendPositionalArgument( QLatin1String("-1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 0 myapp")
    {
      commandLine.appendPositionalArgument( QLatin1String("0") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 2 myapp")
    {
      commandLine.appendPositionalArgument( QLatin1String("2") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("myapp completion-find-current-positional-argument-name 100 myapp")
    {
      commandLine.appendPositionalArgument( QLatin1String("100") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("A")
    {
      commandLine.appendPositionalArgument( QLatin1String("A") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }

    SECTION("1")
    {
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine) );
    }
  }
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

/** \todo Test case in transtion for CommandLine proof of concept
 *
 * Must be adapted once done
 */
TEST_CASE("BashCompletionParserQuery")
{
  ParserDefinition parserDefinition;
//   QStringList arguments{QLatin1String("myapp"),completionFindCurrentPositionalArgumentNameString()};
//   ParserResult result;
  
  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("myapp") );
  commandLine.appendPositionalArgument( completionFindCurrentPositionalArgumentNameString() );

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp")
    {
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      
//       arguments << QLatin1String("1") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      
//       BashCompletionParserQuery query(result, parserDefinition);
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("1") << QLatin1String("myapp")  << QLatin1String("-");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineHasSubCommand() );
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
    SECTION("myapp completion-find-current-positional-argument-name 1 myapp --")
    {
//       arguments << QLatin1String("1") << QLatin1String("myapp")  << QLatin1String("--");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendDoubleDash();
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineHasSubCommand() );
      REQUIRE( query.compLineSubCommandNamePositionIndex() < 0 );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentsCount() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandPositionalArgumentsIndexInDefinition() );
      REQUIRE( query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtSubCommandNameIndexInDefinition() );
      REQUIRE( !query.isCursorAtSubCommandOption() );
    }

    SECTION("myapp completion-find-current-positional-argument-name 1 myapp -h")
    {
//       arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("-h");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendOption( QLatin1String("h") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("-h");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("2") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendOption( QLatin1String("h") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("file.txt");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("2") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("1") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("-h");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("2") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendOption( QLatin1String("h") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("copy");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("2") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendSubCommandName( QLatin1String("copy") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("co");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendPositionalArgument( QLatin1String("co") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("file.txt");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("3") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 3 );
      REQUIRE( query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("3") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("-h");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("3") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendOption( QLatin1String("h") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 3 );
      REQUIRE( query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("copy") << QLatin1String("-h");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("2") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendOption( QLatin1String("h") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( query.compLineHasSubCommand() );
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
//       
//       commandLine.appendPositionalArgument( QLatin1String("2") );
//       commandLine.appendPositionalArgument( QLatin1String("myapp") );
//       commandLine.appendSubCommandName( QLatin1String("copy") );
//       commandLine.appendOption( QLatin1String("h") );
//       BashCompletionParserQuery query(commandLine, parserDefinition);
//       REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
//       REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("1") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("2") << QLatin1String("myapp") << QLatin1String("arg1");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("2") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendPositionalArgument( QLatin1String("arg1") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("arg1");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendPositionalArgument( QLatin1String("arg1") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.compLineHasSubCommand() );
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
//       arguments << QLatin1String("1") << QLatin1String("myapp") << QLatin1String("copy");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       BashCompletionParserQuery query(result, parserDefinition);
      
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      commandLine.appendSubCommandName( QLatin1String("copy") );
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( query.compLineHasSubCommand() );
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

TEST_CASE("BashCompletionParserQuery_SimpleApp")
{
  ParserDefinitionOption overwriteBehavior( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehavior.setValueName( QLatin1String("behavior") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose mode") );
  parserDefinition.addOption(overwriteBehavior);
  parserDefinition.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file") );
  parserDefinition.addPositionalArgument( QLatin1String("destination"), QLatin1String("Destination directory") );

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  SECTION("app completion-find-current-argument 1 app")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 1 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 0")
    {
      const int compLineIndex = 0;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 1 app -")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSingleDash();
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 2 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 1 app --")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendDoubleDash();
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 2 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 2 app --verbose")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 2 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 2 app --verbose fi")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendPositionalArgument( QLatin1String("fi") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 3 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

  }

  SECTION("app completion-find-current-argument 2 app --verbose -")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendSingleDash();
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 3 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 3 app --verbose --overwrite-behavior")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 3 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 3 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.optionNameRelatedToCurrentOptionValue() == QLatin1String("overwrite-behavior") );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 3")
    {
      const int compLineIndex = 3;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 4 app --verbose --overwrite-behavior keep")
  {
    commandLine.appendPositionalArgument( QLatin1String("4") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
    commandLine.appendOptionValue( QLatin1String("keep") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 4 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 4 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 3")
    {
      const int compLineIndex = 3;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 4")
    {
      const int compLineIndex = 4;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 5 app --verbose --overwrite-behavior keep file.txt")
  {
    commandLine.appendPositionalArgument( QLatin1String("5") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
    commandLine.appendOptionValue( QLatin1String("keep") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 5 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 5 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 1 );
      REQUIRE( !query.isCursorAtMainCommandOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 3")
    {
      const int compLineIndex = 3;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 4")
    {
      const int compLineIndex = 4;
      REQUIRE( !query.isCompLineIndexAtMainCommandOption(compLineIndex) );
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 1 app file.txt")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 2 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 0 );
    REQUIRE( !query.isCursorAtMainCommandOption() );
    REQUIRE( !query.isCursorAtMainCommandOptionValue() );
  }

  SECTION("app completion-find-current-argument 2 app file.txt")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 2 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 1 );
    REQUIRE( !query.isCursorAtMainCommandOption() );
    REQUIRE( !query.isCursorAtMainCommandOptionValue() );
  }

  SECTION("app completion-find-current-argument 2 app file.txt /tmp")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 3 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() == 1 );
    REQUIRE( !query.isCursorAtMainCommandOption() );
    REQUIRE( !query.isCursorAtMainCommandOptionValue() );
  }

  SECTION("app completion-find-current-argument 4 app --verbose file.txt /tmp")
  {
    commandLine.appendPositionalArgument( QLatin1String("4") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 4 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 4 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.cursorMainCommandPositionalArgumentsIndexInDefinition() < 0 );
    REQUIRE( !query.isCursorAtMainCommandOption() );
    REQUIRE( !query.isCursorAtMainCommandOptionValue() );
  }
}

TEST_CASE("BashCompletionParserQuery_AppWithSubCommand")
{
  SECTION("Test not complete")
  {
    REQUIRE(false);
  }

}

TEST_CASE("BashCompletionParserQuery_MainPositionalArgument_and_AppWithSubCommand")
{
  SECTION("Test not complete")
  {
    REQUIRE(false);
  }
}


TEST_CASE("completionFindCurrentArgument_SimpleApp")
{
  using Impl::completionFindCurrentArgument;

  ParserDefinitionOption overwriteBehavior( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehavior.setValueName( QLatin1String("behavior") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose mode") );
  parserDefinition.addOption(overwriteBehavior);
  parserDefinition.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file") );
  parserDefinition.addPositionalArgument( QLatin1String("destination"), QLatin1String("Destination directory") );

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  SECTION("app completion-find-current-argument 1 app")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options-or-source") );
  }

  SECTION("app completion-find-current-argument 1 app -")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSingleDash();
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options") );
  }

  SECTION("app completion-find-current-argument 1 app --")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendDoubleDash();
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options") );
  }

  SECTION("app completion-find-current-argument 2 app --verbose")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options-or-source") );
  }

  SECTION("app completion-find-current-argument 2 app --verbose fi")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendPositionalArgument( QLatin1String("fi") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("source") );
  }

  SECTION("app completion-find-current-argument 2 app --verbose -")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendSingleDash();
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options") );
  }

  SECTION("app completion-find-current-argument 3 app --verbose --overwrite-behavior")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("option-overwrite-behavior-value") );
  }

  SECTION("app completion-find-current-argument 4 app --verbose --overwrite-behavior keep")
  {
    commandLine.appendPositionalArgument( QLatin1String("4") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
    commandLine.appendOptionValue( QLatin1String("keep") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options-or-source") );
  }

  SECTION("app completion-find-current-argument 5 app --verbose --overwrite-behavior keep file.txt")
  {
    commandLine.appendPositionalArgument( QLatin1String("5") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
    commandLine.appendOptionValue( QLatin1String("keep") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("destination") );
  }
}

TEST_CASE("completionFindCurrentArgument_AppWithSubCommand")
{
  using Impl::completionFindCurrentArgument;

  ParserDefinitionOption overwriteBehavior( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehavior.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addHelpOption();
  copyCommand.addOption( QLatin1String("dereference"), QLatin1String("Dereference option") );
  copyCommand.addOption(overwriteBehavior);

  ParserDefinitionCommand cutCommand( QLatin1String("cut") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose mode") );
  parserDefinition.addSubCommand(copyCommand);
  parserDefinition.addSubCommand(cutCommand);

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  SECTION("app completion-find-current-argument 1 app")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options-or-commands") );
  }

  SECTION("app completion-find-current-argument 1 app -")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSingleDash();
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options") );
  }

  SECTION("app completion-find-current-argument 1 app --")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendDoubleDash();
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options") );
  }

  SECTION("app completion-find-current-argument 2 app --verbose")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options-or-commands") );
  }

  SECTION("app completion-find-current-argument 1 app c")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendPositionalArgument( QLatin1String("c") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("commands") );
  }

  SECTION("app completion-find-current-argument 1 app fi")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendPositionalArgument( QLatin1String("fi") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("commands") );
  }

  SECTION("app completion-find-current-argument 2 app copy")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("copy-options-or-source") );
  }

  SECTION("app completion-find-current-argument 2 app copy -")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendSingleDash();
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("copy-options") );
  }

  SECTION("app completion-find-current-argument 3 app copy --dereference")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendOption( QLatin1String("dereference") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("copy-options-or-source") );
  }

  SECTION("app completion-find-current-argument 3 app copy --overwrite-behavior")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("copy-option-overwrite-behavior-value") );
  }

  SECTION("app completion-find-current-argument 2 app copy fi")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendPositionalArgument( QLatin1String("fi") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("copy-source") );
  }

  SECTION("app completion-find-current-argument 3 app copy file.txt")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("copy-destination") );
  }
}

TEST_CASE("completionFindCurrentArgument_MainPositionalArgument_and_AppWithSubCommand")
{
  using Impl::completionFindCurrentArgument;

  ParserDefinitionOption overwriteBehavior( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehavior.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addHelpOption();
  copyCommand.addOption( QLatin1String("dereference"), QLatin1String("Dereference option") );
  copyCommand.addOption(overwriteBehavior);

  ParserDefinitionCommand cutCommand( QLatin1String("cut") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose mode") );
  parserDefinition.addPositionalArgument( QLatin1String("config"), QLatin1String("Config file") );
  parserDefinition.addSubCommand(copyCommand);
  parserDefinition.addSubCommand(cutCommand);

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  SECTION("app completion-find-current-argument 1 app")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options-or-config") );
  }

  SECTION("app completion-find-current-argument 1 app -")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSingleDash();
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options") );
  }

  SECTION("app completion-find-current-argument 1 app c")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendPositionalArgument( QLatin1String("c") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("config") );
  }

  SECTION("app completion-find-current-argument 1 app fi")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendPositionalArgument( QLatin1String("fi") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("config") );
  }

  SECTION("app completion-find-current-argument 2 app file.conf")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendPositionalArgument( QLatin1String("file.conf") );
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("commands") );
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

    // The parser not accept this, because it threat -1 as a unknown option
//     SECTION("myapp completion-find-current-positional-argument-name -1 myapp (invalid cursor)")
//     {
//       arguments << completionFindCurrentPositionalArgumentNameString() << QLatin1String("-1") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       REQUIRE( !handleBashCompletion(result, parserDefinition) );
//     }

    SECTION("myapp completion-find-current-positional-argument-name 100 myapp (invalid cursor)")
    {
      arguments << completionFindCurrentPositionalArgumentNameString() << QLatin1String("100") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !handleBashCompletion(result, parserDefinition) );
    }

    SECTION("myapp completion-find-current-positional-argument-name A myapp (invalid cursor)")
    {
      arguments << completionFindCurrentPositionalArgumentNameString() << QLatin1String("A") << QLatin1String("myapp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !handleBashCompletion(result, parserDefinition) );
    }
  }
}
