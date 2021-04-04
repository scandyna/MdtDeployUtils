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
#include "TestUtils.h"
#include "Mdt/CommandLineParser/BashCompletionParser.h"
#include "Mdt/CommandLineParser/BashCompletionParserQuery.h"
#include "Mdt/CommandLineParser/BashCompletionParserCurrentArgument.h"
#include "Mdt/CommandLineParser/Impl/BashCompletionParser.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/Parser.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <vector>
#include <string>

using namespace Mdt::CommandLineParser;

QString completionFindCurrentPositionalArgumentNameString()
{
  return BashCompletionParserQuery::findCurrentArgumentString();
}

QString completionFindCurrentArgumentString()
{
  return BashCompletionParserQuery::findCurrentArgumentString();
}

BashCompletionParserCurrentArgument
completionFindCurrentArgumentForCursorInTheCompLine(const CommandLine::CommandLine & commandLine, const ParserDefinition & parserDefinition)
{
  assert( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );

  BashCompletionParserQuery query(commandLine, parserDefinition);

  return Impl::completionFindCurrentArgumentForCursorInTheCompLine(query);
}

BashCompletionParserCurrentArgument
completionFindCurrentArgumentForCursorPastTheCompLine(const CommandLine::CommandLine & commandLine, const ParserDefinition & parserDefinition)
{
  assert( BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) );

  BashCompletionParserQuery query(commandLine, parserDefinition);

  return Impl::completionFindCurrentArgumentForCursorPastTheCompLine(query);
}


TEST_CASE("BashCompletionParserCurrentArgument")
{
  SECTION("Default constructed")
  {
    BashCompletionParserCurrentArgument currentArgument;
    REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Undefined );
    REQUIRE( !currentArgument.isInSubCommand() );
  }

  SECTION("set/get")
  {
    BashCompletionParserCurrentArgument currentArgument;
    currentArgument.setType(BashCompletionParserCurrentArgumentType::Options);
    currentArgument.setInSubCommand(true);
    REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
    REQUIRE( currentArgument.isInSubCommand() );
  }
}

TEST_CASE("completionFindCurrentArgumentForCursorInTheCompLine_SimpleApp")
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

  BashCompletionParserCurrentArgument currentArgument;

  SECTION("1 app x")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("1 app -")
    {
      commandLine.appendSingleDash();
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("1 app --")
    {
      commandLine.appendDoubleDash();
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("1 app -h")
    {
      commandLine.appendOption( QLatin1String("h") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("1 app --overwrite-behavior")
    {
      commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("1 app fi")
    {
      commandLine.appendPositionalArgument( QLatin1String("fi") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::PositionalArgumentName );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }

  SECTION("2 app x y")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app --overwrite-behavior k")
    {
      commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
      commandLine.appendOptionValue( QLatin1String("k") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionNameValue );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("2 app --verbose fi")
    {
      commandLine.appendOption( QLatin1String("verbose") );
      commandLine.appendPositionalArgument( QLatin1String("fi") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::PositionalArgumentName );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("2 app file.txt /")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      commandLine.appendPositionalArgument( QLatin1String("/") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::PositionalArgumentName );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }

  SECTION("3 app x y z")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("3 app file.txt /tmp o")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      commandLine.appendPositionalArgument( QLatin1String("/tmp") );
      commandLine.appendPositionalArgument( QLatin1String("o") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Undefined );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("3 app file.txt /tmp -")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      commandLine.appendPositionalArgument( QLatin1String("/tmp") );
      commandLine.appendSingleDash();
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("3 app file.txt /tmp --")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      commandLine.appendPositionalArgument( QLatin1String("/tmp") );
      commandLine.appendDoubleDash();
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("3 app file.txt /tmp --h (partially typed --help)")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      commandLine.appendPositionalArgument( QLatin1String("/tmp") );
      commandLine.appendUnknownOption( QLatin1String("h") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }
}

TEST_CASE("completionFindCurrentArgumentForCursorInTheCompLine_AppWithSubCommand")
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

  BashCompletionParserCurrentArgument currentArgument;

  SECTION("1 app x")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("1 app fi")
    {
      commandLine.appendPositionalArgument( QLatin1String("fi") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Commands );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("1 app copy")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Commands );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }

  SECTION("2 app x y")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app --verbose copy")
    {
      commandLine.appendOption( QLatin1String("verbose") );
      commandLine.appendSubCommandName( QLatin1String("copy") );

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Commands );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }

  SECTION("2 app copy x")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );

    SECTION("2 app copy -h")
    {
      commandLine.appendOption( QLatin1String("h") );

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( currentArgument.isInSubCommand() );
    }

    SECTION("2 app copy fi")
    {
      commandLine.appendPositionalArgument( QLatin1String("fi") );

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::PositionalArgumentName );
      REQUIRE( currentArgument.isInSubCommand() );
    }

    SECTION("2 app copy -")
    {
      commandLine.appendSingleDash();

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( currentArgument.isInSubCommand() );
    }

    SECTION("2 app copy --")
    {
      commandLine.appendDoubleDash();

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( currentArgument.isInSubCommand() );
    }

    SECTION("2 app copy --dereference")
    {
      commandLine.appendOption( QLatin1String("dereference") );

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( currentArgument.isInSubCommand() );
    }
  }

  SECTION("2 app cut x")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("cut") );

    SECTION("2 app cut fi")
    {
      commandLine.appendPositionalArgument( QLatin1String("fi") );

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Undefined );
      REQUIRE( currentArgument.isInSubCommand() );
    }
  }

  SECTION("3 app copy x y")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );

    SECTION("3 app copy --overwrite-behavior k")
    {
      commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
      commandLine.appendOptionValue( QLatin1String("k") );

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionNameValue );
      REQUIRE( currentArgument.isInSubCommand() );
    }

    SECTION("3 app copy file.txt /")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      commandLine.appendPositionalArgument( QLatin1String("/") );

      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::PositionalArgumentName );
      REQUIRE( currentArgument.isInSubCommand() );
    }
  }
}

TEST_CASE("completionFindCurrentArgumentForCursorInTheCompLine_MainPositionalArgument_and_AppWithSubCommand")
{
  ParserDefinitionCommand copyCommand( QLatin1String("copy") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addPositionalArgument( QLatin1String("config"), QLatin1String("Config") );
  parserDefinition.addSubCommand(copyCommand);

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  BashCompletionParserCurrentArgument currentArgument;

  SECTION("1 app x")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("1 app fi")
    {
      commandLine.appendPositionalArgument( QLatin1String("fi") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::PositionalArgumentName );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }

  SECTION("2 app x y")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app file.conf c")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.conf") );
      commandLine.appendPositionalArgument( QLatin1String("c") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Commands );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("2 app file.conf copy")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.conf") );
      commandLine.appendSubCommandName( QLatin1String("copy") );
      currentArgument = completionFindCurrentArgumentForCursorInTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Commands );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }
}

TEST_CASE("completionFindCurrentArgumentForCursorPastTheCompLine_SimpleApp_NoPositionalArgument")
{
  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose mode") );

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  BashCompletionParserCurrentArgument currentArgument;

  SECTION("1 app")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
    REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
    REQUIRE( !currentArgument.isInSubCommand() );
  }

  SECTION("2 app x")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app --verbose")
    {
      commandLine.appendOption( QLatin1String("verbose") );
      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }
}

TEST_CASE("completionFindCurrentArgumentForCursorPastTheCompLine_SimpleApp")
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

  BashCompletionParserCurrentArgument currentArgument;

  SECTION("1 app")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
    REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName );
    REQUIRE( !currentArgument.isInSubCommand() );
  }

  SECTION("2 app x")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app --verbose")
    {
      commandLine.appendOption( QLatin1String("verbose") );
      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("2 app --overwrite-behavior")
    {
      commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionNameValue );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("2 app file.txt")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }

  SECTION("3 app x y")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("3 app file.txt /tmp")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.txt") );
      commandLine.appendPositionalArgument( QLatin1String("/tmp") );
      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Options );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
  }
}

TEST_CASE("completionFindCurrentArgumentForCursorPastTheCompLine_AppWithSubCommand")
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

  BashCompletionParserCurrentArgument currentArgument;

  SECTION("1 app")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
    REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionsOrCommands );
    REQUIRE( !currentArgument.isInSubCommand() );
  }

  SECTION("2 app x")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app co")
    {
      commandLine.appendPositionalArgument( QLatin1String("co") );

      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::Undefined );
      REQUIRE( !currentArgument.isInSubCommand() );
    }

    SECTION("2 app copy")
    {
      commandLine.appendSubCommandName( QLatin1String("copy") );

      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName );
      REQUIRE( currentArgument.isInSubCommand() );
    }
  }

  SECTION("3 app copy x")
  {
    commandLine.appendPositionalArgument( QLatin1String("3") );
    commandLine.appendPositionalArgument( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );

    SECTION("3 app copy --dereference")
    {
      commandLine.appendOption( QLatin1String("dereference") );

      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName );
      REQUIRE( currentArgument.isInSubCommand() );
    }

    SECTION("3 app copy --overwrite-behavior")
    {
      commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );

      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionNameValue );
      REQUIRE( currentArgument.isInSubCommand() );
    }
  }
}

TEST_CASE("completionFindCurrentArgumentForCursorPastTheCompLine_MainPositionalArgument_and_AppWithSubCommand")
{
  ParserDefinitionCommand copyCommand( QLatin1String("copy") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addPositionalArgument( QLatin1String("config"), QLatin1String("Config") );
  parserDefinition.addSubCommand(copyCommand);

  CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );
  commandLine.appendPositionalArgument( completionFindCurrentArgumentString() );

  BashCompletionParserCurrentArgument currentArgument;

  SECTION("1 app")
  {
    commandLine.appendPositionalArgument( QLatin1String("1") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
    REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionsOrPositionalArgumentName );
    REQUIRE( !currentArgument.isInSubCommand() );
  }

  SECTION("2 app x")
  {
    commandLine.appendPositionalArgument( QLatin1String("2") );
    commandLine.appendPositionalArgument( QLatin1String("app") );

    SECTION("2 app file.conf")
    {
      commandLine.appendPositionalArgument( QLatin1String("file.conf") );

      currentArgument = completionFindCurrentArgumentForCursorPastTheCompLine(commandLine, parserDefinition);
      REQUIRE( currentArgument.type() == BashCompletionParserCurrentArgumentType::OptionsOrCommands );
      REQUIRE( !currentArgument.isInSubCommand() );
    }
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
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options-or-destination") );
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
//     qDebug() << "TEST: 2 app copy";
    
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
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("copy-options-or-destination") );
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
    REQUIRE( completionFindCurrentArgument(commandLine, parserDefinition) == QLatin1String("options-or-commands") );
  }
}

TEST_CASE("handleBashCompletion")
{
//   using Impl::completionFindCurrentPositionalArgumentNameString;

  ParserDefinition parserDefinition;
  QStringList arguments{QLatin1String("app")};
//   ParserResult result;

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ValueType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    Parser parser(parserDefinition);

    SECTION("app")
    {
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !handleBashCompletion(parser.commandLine(), parserDefinition) );
    }

    SECTION("app completion-find-current-argument 1 app")
    {
//       arguments << completionFindCurrentPositionalArgumentNameString() << QLatin1String("1") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       REQUIRE( handleBashCompletion(result, parserDefinition) );

      arguments << completionFindCurrentArgumentString() << qStringListFromUtf8Strings({"1","app"});
      REQUIRE( parser.parse(arguments) );
      REQUIRE( handleBashCompletion(parser.commandLine(), parserDefinition) );
    }

    // The parser not accept this, because it threat -1 as a unknown option
//     SECTION("myapp completion-find-current-positional-argument-name -1 myapp (invalid cursor)")
//     {
//       arguments << completionFindCurrentPositionalArgumentNameString() << QLatin1String("-1") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       REQUIRE( !handleBashCompletion(result, parserDefinition) );
//     }

    SECTION("app completion-find-current-argument 100 app (invalid cursor)")
    {
//       arguments << completionFindCurrentPositionalArgumentNameString() << QLatin1String("100") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       REQUIRE( !handleBashCompletion(result, parserDefinition) );

      arguments << completionFindCurrentArgumentString() << qStringListFromUtf8Strings({"100","app"});
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !handleBashCompletion(parser.commandLine(), parserDefinition) );
    }

    SECTION("myapp completion-find-current-argument A myapp (invalid cursor)")
    {
//       arguments << completionFindCurrentPositionalArgumentNameString() << QLatin1String("A") << QLatin1String("myapp");
//       REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
//       REQUIRE( !handleBashCompletion(result, parserDefinition) );

      arguments << completionFindCurrentArgumentString() << qStringListFromUtf8Strings({"A","app"});
      REQUIRE( parser.parse(arguments) );
      REQUIRE( !handleBashCompletion(parser.commandLine(), parserDefinition) );
    }
  }
}
