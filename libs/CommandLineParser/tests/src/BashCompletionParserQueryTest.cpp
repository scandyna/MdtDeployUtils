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
#include "Mdt/CommandLineParser/BashCompletionParserQuery.h"
#include <QString>
#include <QLatin1String>

using namespace Mdt::CommandLineParser;


QString completionFindCurrentArgumentString()
{
  return BashCompletionParserQuery::findCurrentArgumentString();
}

TEST_CASE("isBashCompletionOption")
{
  CommandLine::Argument argument;

  SECTION("Option")
  {
    argument = CommandLine::Option{QLatin1String("help")};
    REQUIRE( isBashCompletionOption(argument) );
  }

  SECTION("Unknown option")
  {
    argument = CommandLine::UnknownOption{QLatin1String("help")};
    REQUIRE( isBashCompletionOption(argument) );
  }

  SECTION("Option expecting value")
  {
    argument = CommandLine::Option{QLatin1String("copy-mode"),true};
    REQUIRE( isBashCompletionOption(argument) );
  }

  SECTION("OptionValue")
  {
    argument = CommandLine::OptionValue{QLatin1String("help")};
    REQUIRE( !isBashCompletionOption(argument) );
  }

  SECTION("OptionWithValue")
  {
    argument = CommandLine::OptionWithValue{QLatin1String("help"),QLatin1String("true")};
    REQUIRE( isBashCompletionOption(argument) );
  }

  SECTION("-")
  {
    argument = CommandLine::SingleDash{};
    REQUIRE( isBashCompletionOption(argument) );
  }

  SECTION("--")
  {
    argument = CommandLine::DoubleDash{};
    REQUIRE( isBashCompletionOption(argument) );
  }
}

TEST_CASE("isValidBashCompletionQuery")
{
  CommandLine::CommandLine commandLine;
  ParserDefinition parserDefinition;

  SECTION("argument count validity")
  {
    SECTION("empty")
    {
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("app")
    {
      commandLine.setExecutableName( QLatin1String("app") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("app completion-current-argument")
    {
      commandLine.setExecutableName( QLatin1String("app") );
      commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("app completion-current-argument 1")
    {
      commandLine.setExecutableName( QLatin1String("app") );
      commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );
      commandLine.appendPositionalArgument( QLatin1String("1") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("app completion-current-argument 1 app")
    {
      commandLine.setExecutableName( QLatin1String("myapp") );
      commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("myapp") );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }
  }

  SECTION("cursor validity")
  {
    commandLine.setExecutableName( QLatin1String("app") );
    commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

    SECTION("-1")
    {
      commandLine.appendPositionalArgument( QLatin1String("-1") );
      commandLine.appendPositionalArgument( QLatin1String("app") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("app completion-current-argument 0 app")
    {
      commandLine.appendPositionalArgument( QLatin1String("0") );
      commandLine.appendPositionalArgument( QLatin1String("app") );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("app completion-current-argument 1 app")
    {
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("app") );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("app completion-current-argument 2 app")
    {
      commandLine.appendPositionalArgument( QLatin1String("2") );
      commandLine.appendPositionalArgument( QLatin1String("app") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("app completion-current-argument 100 app")
    {
      commandLine.appendPositionalArgument( QLatin1String("100") );
      commandLine.appendPositionalArgument( QLatin1String("app") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("A")
    {
      commandLine.appendPositionalArgument( QLatin1String("A") );
      commandLine.appendPositionalArgument( QLatin1String("app") );
      REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }

    SECTION("1")
    {
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("app") );
      REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
    }
  }

  SECTION("Sub-command name validity")
  {
    commandLine.setExecutableName( QLatin1String("app") );
    commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("Parser definition without sub-command")
    {
      SECTION("1 app file.txt")
      {
        commandLine.appendPositionalArgument( QLatin1String("file.txt") );
        REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
      }

      SECTION("1 app copy")
      {
        commandLine.appendSubCommandName( QLatin1String("copy") );
        REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
      }
    }

    SECTION("Parser definition with copy sub-command")
    {
      ParserDefinitionCommand copyCommand( QLatin1String("copy") );
      parserDefinition.addSubCommand(copyCommand);

      SECTION("1 app file.txt")
      {
        commandLine.appendPositionalArgument( QLatin1String("file.txt") );
        REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
      }

      SECTION("1 app copy")
      {
        commandLine.appendSubCommandName( QLatin1String("copy") );
        REQUIRE( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
      }

      SECTION("1 app cut")
      {
        commandLine.appendSubCommandName( QLatin1String("cut") );
        REQUIRE( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );
      }
    }
  }
}

TEST_CASE("SimpleApp")
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
    REQUIRE( !query.parserDefinitionHasSubCommand() );
    REQUIRE( query.parserDefinitionMainCommandPositionalArgumentCount() == 2 );
    REQUIRE( query.compLineArgumentCount() == 1 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
    REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("source") );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 0")
    {
      const int compLineIndex = 0;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }
  }

  SECTION("app completion-find-current-argument 1 app --he")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendUnknownOption( QLatin1String("he") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 2 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
    REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.isCursorAtOption() );
  }

  SECTION("app completion-find-current-argument 2 app --verbose")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendOption( QLatin1String("verbose") );
    BashCompletionParserQuery query(commandLine, parserDefinition);
    REQUIRE( query.compLineArgumentCount() == 2 );
    REQUIRE( query.cursorInCompLinePositionIndex() == 2 );
    REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("source") );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 1 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("source") );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.optionNameRelatedToCurrentOptionValue() == QLatin1String("overwrite-behavior") );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 3")
    {
      const int compLineIndex = 3;
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("source") );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 1")
    {
      const int compLineIndex = 1;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 3")
    {
      const int compLineIndex = 3;
      REQUIRE( query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 4")
    {
      const int compLineIndex = 4;
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 1 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );

    SECTION("cursor based results")
    {
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 1 );
      REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("destination") );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("compLineIndex 2")
    {
      const int compLineIndex = 2;
      REQUIRE( !query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 3")
    {
      const int compLineIndex = 3;
      REQUIRE( query.isCompLineIndexAtMainCommandOptionValue(compLineIndex) );
    }

    SECTION("compLineIndex 4")
    {
      const int compLineIndex = 4;
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 1 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( query.isCursorInMainCommand() );
    REQUIRE( !query.isCursorAtSubCommandName() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 0 );
    REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("source") );
    REQUIRE( !query.isCursorAtOption() );
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 1 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 1 );
    REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("destination") );
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 2 );
    REQUIRE( !query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( query.isCursorInMainCommand() );
    REQUIRE( !query.isCursorAtSubCommandName() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 1 );
    REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("destination") );
    REQUIRE( !query.isCursorAtOption() );
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
    REQUIRE( query.mainCommandPositionalArgumentCount() == 2 );
    REQUIRE( query.isCursorPastTheCompLine() );
    REQUIRE( !query.compLineHasSubCommand() );
    REQUIRE( query.isCursorInMainCommand() );
    REQUIRE( !query.isCursorAtSubCommandName() );
    REQUIRE( !query.isCursorInSubCommand() );
    REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
    REQUIRE( !query.isCursorAtMainCommandOptionValue() );
  }
}

TEST_CASE("AppWithSubCommand")
{
  ParserDefinitionOption overwriteBehavior( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehavior.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addHelpOption();
  copyCommand.addOption( QLatin1String("dereference"), QLatin1String("Dereference option") );
  copyCommand.addOption(overwriteBehavior);
  copyCommand.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file") );
  copyCommand.addPositionalArgument( QLatin1String("destination"), QLatin1String("Destination directory") );

  ParserDefinitionCommand cutCommand( QLatin1String("cut") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose mode") );
  parserDefinition.addSubCommand(copyCommand);
  parserDefinition.addSubCommand(cutCommand);

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  SECTION("Methods not depending on argument types")
  {
    SECTION("1 app")
    {
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("app") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.compLineArgumentCount() == 1 );
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( query.isCursorPastTheCompLine() );
    }

    SECTION("1 app copy")
    {
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("app") );
      commandLine.appendSubCommandName( QLatin1String("copy") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.compLineArgumentCount() == 2 );
      REQUIRE( query.cursorInCompLinePositionIndex() == 1 );
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( !query.isCursorAtOption() );
    }
  }

  SECTION("Query about sub-command name and availability")
  {
    SECTION("1 app (x)")
    {
      commandLine.appendPositionalArgument( QLatin1String("1") );
      commandLine.appendPositionalArgument( QLatin1String("app") );

      SECTION("1 app")
      {
        BashCompletionParserQuery query(commandLine, parserDefinition);
        REQUIRE( !query.compLineHasSubCommand() );
      }

      SECTION("1 app file.txt")
      {
        commandLine.appendPositionalArgument( QLatin1String("file.txt") );

        BashCompletionParserQuery query(commandLine, parserDefinition);
        REQUIRE( !query.compLineHasSubCommand() );
      }

      SECTION("1 app copy")
      {
        commandLine.appendSubCommandName( QLatin1String("copy") );

        BashCompletionParserQuery query(commandLine, parserDefinition);
        REQUIRE( query.compLineHasSubCommand() );
        REQUIRE( query.subCommandName() == QLatin1String("copy") );
      }

      SECTION("1 app cut")
      {
        commandLine.appendSubCommandName( QLatin1String("cut") );

        BashCompletionParserQuery query(commandLine, parserDefinition);
        REQUIRE( query.compLineHasSubCommand() );
        REQUIRE( query.subCommandName() == QLatin1String("cut") );
      }
    }
  }

  SECTION("1 app (x)")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("1 app")
    {
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.isCursorPastTheCompLine() );
      REQUIRE( !query.compLineHasSubCommand() );
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("1 app co")
    {
      commandLine.appendPositionalArgument( QLatin1String("co") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.mainCommandPositionalArgumentCount() == 1 );
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( !query.compLineHasSubCommand() );
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 0 );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
    }

    SECTION("1 app copy")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() < 0 );
    }

    SECTION("1 app cut")
    {
      commandLine.appendSubCommandName( QLatin1String("cut") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() < 0 );
    }
  }

  SECTION("2 app x (y)")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app copy")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      ///REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() == 0 );
    }

    SECTION("2 app copy file.txt")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 1 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() == 0 );
      REQUIRE( query.cursorSubCommandPositionalArgumentName() == QLatin1String("source") );
    }

    SECTION("2 app copy --verbose")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendOption( QLatin1String("verbose") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() < 0 );
    }
  }

  SECTION("3 app x y (z)")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("3 app copy --overwrite-behavior")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() < 0 );
    }

    SECTION("3 app copy file.txt /tmp")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      commandLine.appendPositionalArgument( QLatin1String("/tmp") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() == 1 );
      REQUIRE( query.cursorSubCommandPositionalArgumentName() == QLatin1String("destination") );
    }

    SECTION("3 app copy file.txt")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 1 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() == 1 );
      REQUIRE( query.cursorSubCommandPositionalArgumentName() == QLatin1String("destination") );
    }

  }

  SECTION("4 app x y z (...)")
  {
    commandLine.appendPositionalArgument( QLatin1String("4") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("app --verbose copy --overwrite-behavior ke")
    {
      commandLine.appendOption( QLatin1String("verbose") );
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
      commandLine.appendOptionValue( QLatin1String("ke") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.isCursorAtOptionValue() );
      REQUIRE( query.optionNameRelatedToCurrentOptionValue() == QLatin1String("overwrite-behavior") );
      REQUIRE( query.subCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() < 0 );
    }

    SECTION("app --verbose copy --overwrite-behavior")
    {
      commandLine.appendOption( QLatin1String("verbose") );
      commandLine.appendSubCommandName( QLatin1String("copy") );
      commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( query.isCursorAtOptionValue() );
      REQUIRE( query.optionNameRelatedToCurrentOptionValue() == QLatin1String("overwrite-behavior") );
      REQUIRE( query.subCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() < 0 );
    }
  }
}

TEST_CASE("MainPositionalArgument_and_AppWithSubCommand")
{
  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addHelpOption();
  copyCommand.addOption( QLatin1String("dereference"), QLatin1String("Dereference option") );
  copyCommand.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file") );
  copyCommand.addPositionalArgument( QLatin1String("destination"), QLatin1String("Destination directory") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose mode") );
  parserDefinition.addPositionalArgument( QLatin1String("config"), QLatin1String("Config file") );
  parserDefinition.addSubCommand(copyCommand);

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  SECTION("1 app (x)")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("1 app")
    {
      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( !query.compLineHasSubCommand() );
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 0 );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( !query.isCursorAtOptionValue() );
    }

    SECTION("1 app -")
    {
      commandLine.appendSingleDash();

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 0 );
      REQUIRE( !query.compLineHasSubCommand() );
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( !query.isCursorAtOptionValue() );
    }

    SECTION("1 app fi")
    {
      commandLine.appendPositionalArgument( QLatin1String("fi") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( !query.compLineHasSubCommand() );
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 1 );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 0 );
      REQUIRE( query.cursorMainCommandPositionalArgumentName() == QLatin1String("config") );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( !query.isCursorAtOptionValue() );
    }
  }

  SECTION("2 app x (y)")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app file.conf")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.conf") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( query.isCursorPastTheCompLine() );
      REQUIRE( !query.compLineHasSubCommand() );
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 1 );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( !query.isCursorAtOptionValue() );
    }

    SECTION("2 app file.conf c")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.conf") );
      commandLine.appendPositionalArgument( QLatin1String("c") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( !query.compLineHasSubCommand() );
      REQUIRE( query.isCursorInMainCommand() );
      REQUIRE( !query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() == 1 );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( !query.isCursorAtOptionValue() );
    }

    SECTION("2 app file.conf copy")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.conf") );
      commandLine.appendSubCommandName( QLatin1String("copy") );

      BashCompletionParserQuery query(commandLine, parserDefinition);
      REQUIRE( !query.isCursorPastTheCompLine() );
      REQUIRE( query.compLineHasSubCommand() );
      REQUIRE( !query.isCursorInMainCommand() );
      REQUIRE( query.isCursorAtSubCommandName() );
      REQUIRE( !query.isCursorInSubCommand() );
      REQUIRE( query.mainCommandPositionalArgumentCount() == 1 );
      REQUIRE( query.cursorMainCommandPositionalArgumentIndexInDefinition() < 0 );
      REQUIRE( !query.isCursorAtOption() );
      REQUIRE( !query.isCursorAtMainCommandOptionValue() );
      REQUIRE( !query.isCursorAtOptionValue() );
      REQUIRE( query.subCommandPositionalArgumentCount() == 0 );
      REQUIRE( query.parserDefinitionSubCommandPositionalArgumentCount() == 2 );
      REQUIRE( query.cursorSubCommandPositionalArgumentIndexInDefinition() < 0 );
    }
  }
}
