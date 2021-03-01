/****************************************************************************
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "Mdt/CommandLineParser/CommandLine/ArgumentListAttributes.h"
#include "Mdt/CommandLineParser/CommandLine/CommandLine.h"

using namespace Mdt::CommandLineParser::CommandLine;

ArgumentListAttributes makeAttributes(const Mdt::CommandLineParser::CommandLine::CommandLine & commandLine)
{
  return ArgumentListAttributes( commandLine.argumentList().cbegin(), commandLine.argumentList().cend() );
}

TEST_CASE("DefaultConstruct_then_setArgumentList")
{
  ArgumentListAttributes attributes;
  REQUIRE( attributes.commandLineArgumentCount() == 0 );
  REQUIRE( attributes.subCommandNameIndex() < 0 );

  Mdt::CommandLineParser::CommandLine::CommandLine commandLine;
  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("app file.txt")
  {
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );

    attributes.setArgumentList( commandLine.argumentList() );
    REQUIRE( attributes.commandLineArgumentCount() == 2 );
    REQUIRE( attributes.subCommandNameIndex() < 0 );
  }

  SECTION("app copy file.txt")
  {
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );

    attributes.setArgumentList( commandLine.argumentList() );
    REQUIRE( attributes.commandLineArgumentCount() == 3 );
    REQUIRE( attributes.subCommandNameIndex() == 1 );
  }
}

TEST_CASE("SimpleApp")
{
  using Mdt::CommandLineParser::CommandLine::isOptionOrOptionWithValueOrAnyDash;
  using Mdt::CommandLineParser::CommandLine::isPositionalArgument;

  Mdt::CommandLineParser::CommandLine::CommandLine commandLine;

  commandLine.setExecutableName( QLatin1String("myapp") );

  SECTION("myapp")
  {
    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 1 );
    REQUIRE( attributes.subCommandNameIndex() < 0 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 0 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 0 );

    SECTION("command-line index 0")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandName(commandLineIndex) );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }
  }

  SECTION("app file.txt /tmp")
  {
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 3 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 2 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 0 );

    SECTION("command-line index 0")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandName(commandLineIndex) );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 1")
    {
      const int commandLineIndex = 1;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandName(commandLineIndex) );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 2")
    {
      const int commandLineIndex = 2;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 1 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandName(commandLineIndex) );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }
  }

  SECTION("app -f --verbose file.txt /tmp")
  {
    commandLine.appendOption( QLatin1String("f") );
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 5 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 2 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 0 );

    SECTION("command-line index 0 (app)")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 1 (-f)")
    {
      const int commandLineIndex = 1;
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 2 (--verbose)")
    {
      const int commandLineIndex = 2;
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 3 (file.txt)")
    {
      const int commandLineIndex = 3;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 4 (/tmp)")
    {
      const int commandLineIndex = 4;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 1 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }
  }

  SECTION("app -f --overwrite-behavior keep file.txt /tmp")
  {
    commandLine.appendOption( QLatin1String("f") );
    commandLine.appendOptionExpectingValue( QLatin1String("overwrite-behavior") );
    commandLine.appendOptionValue( QLatin1String("keep") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 6 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 2 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 0 );

    SECTION("command-line index 0 (app)")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.optionNameAtCommandLineIndex(commandLineIndex).isEmpty() );
    }

    SECTION("command-line index 1 (-f)")
    {
      const int commandLineIndex = 1;
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.optionNameAtCommandLineIndex(commandLineIndex) == QLatin1String("f") );
    }

    SECTION("command-line index 2 (--overwrite-behavior)")
    {
      const int commandLineIndex = 2;
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.optionNameAtCommandLineIndex(commandLineIndex) == QLatin1String("overwrite-behavior") );
    }

    SECTION("command-line index 3 (keep)")
    {
      const int commandLineIndex = 3;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.optionNameAtCommandLineIndex(commandLineIndex).isEmpty() );
    }

    SECTION("command-line index 4 (file.txt)")
    {
      const int commandLineIndex = 4;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.optionNameAtCommandLineIndex(commandLineIndex).isEmpty() );
    }

    SECTION("command-line index 5 (/tmp)")
    {
      const int commandLineIndex = 5;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 1 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.optionNameAtCommandLineIndex(commandLineIndex).isEmpty() );
    }
  }

  SECTION("app -f --overwrite-behavior=keep file.txt /tmp")
  {
    commandLine.appendOption( QLatin1String("f") );
    commandLine.appendOptionWithValue( QLatin1String("overwrite-behavior"), QLatin1String("keep") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 5 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 2 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 0 );

    SECTION("command-line index 0 (app)")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 1 (-f)")
    {
      const int commandLineIndex = 1;
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 2 (--overwrite-behavior=keep)")
    {
      const int commandLineIndex = 2;
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 3 (file.txt)")
    {
      const int commandLineIndex = 3;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 4 (/tmp)")
    {
      const int commandLineIndex = 4;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 1 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }
  }

  // Use case: Bash completion
  /// \todo clarify if - is a positional argument or not
  SECTION("app -")
  {
    commandLine.appendSingleDash();

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 2 );
    
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 0 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 0 );

    SECTION("command-line index 0 (app)")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 1 (-)")
    {
      const int commandLineIndex = 1;
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }
  }
}

TEST_CASE("AppWithSubCommand")
{
  Mdt::CommandLineParser::CommandLine::CommandLine commandLine;

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("app copy file.txt /tmp")
  {
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 4 );
    REQUIRE( attributes.subCommandNameIndex() == 1 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 0 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 2 );

    SECTION("command-line index 0 (app)")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandName(commandLineIndex) );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 1 (copy)")
    {
      const int commandLineIndex = 1;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.isCommandLineIndexAtSubCommandName(commandLineIndex) );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 2 (file.txt)")
    {
      const int commandLineIndex = 2;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandName(commandLineIndex) );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 0 );
    }

    SECTION("command-line index 3 (/tmp)")
    {
      const int commandLineIndex = 3;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandName(commandLineIndex) );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 1 );
    }
  }

  SECTION("app --verbose arg1 copy -f file.txt /tmp")
  {
    commandLine.appendOption( QLatin1String("verbose") );
    commandLine.appendPositionalArgument( QLatin1String("arg1") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendOption( QLatin1String("f") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 7 );
    REQUIRE( attributes.subCommandNameIndex() == 3 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 1 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 2 );

    SECTION("command-line index 0 (app)")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 1 (--verbose)")
    {
      const int commandLineIndex = 1;
      REQUIRE( attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 2 (arg1)")
    {
      const int commandLineIndex = 2;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 3 (copy)")
    {
      const int commandLineIndex = 3;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 4 (-f)")
    {
      const int commandLineIndex = 4;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 5 (file.txt)")
    {
      const int commandLineIndex = 5;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 0 );
    }

    SECTION("command-line index 6 (/tmp)")
    {
      const int commandLineIndex = 6;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 1 );
    }
  }

  SECTION("app copy --overwrite-behavior=keep --input-file file.txt /tmp")
  {
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendOptionWithValue( QLatin1String("overwrite-behavior"), QLatin1String("keep") );
    commandLine.appendOptionExpectingValue( QLatin1String("input-file") );
    commandLine.appendOptionValue( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 6 );
    REQUIRE( attributes.subCommandNameIndex() == 1 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 0 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 1 );

    SECTION("command-line index 0 (app)")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 1 (copy)")
    {
      const int commandLineIndex = 1;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 2 (--overwrite-behavior=keep)")
    {
      const int commandLineIndex = 2;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 3 (--input-file)")
    {
      const int commandLineIndex = 3;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 4 (file.txt)")
    {
      const int commandLineIndex = 4;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 5 (/tmp)")
    {
      const int commandLineIndex = 5;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOptionExpectingValue(commandLineIndex) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) == 0 );
    }
  }

  // Use case: Bash completion
  SECTION("app copy -")
  {
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendSingleDash();

    const auto attributes = makeAttributes(commandLine);
    REQUIRE( attributes.commandLineArgumentCount() == 3 );
    REQUIRE( attributes.findMainCommandPositionalArgumentCount(isPositionalArgument) == 0 );
    REQUIRE( attributes.findSubCommandPositionalArgumentCount(isPositionalArgument) == 0 );

    SECTION("command-line index 0 (app)")
    {
      const int commandLineIndex = 0;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 1 (copy)")
    {
      const int commandLineIndex = 1;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( !attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }

    SECTION("command-line index 2 (-)")
    {
      const int commandLineIndex = 2;
      REQUIRE( !attributes.isCommandLineIndexAtMainCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findMainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
      REQUIRE( attributes.isCommandLineIndexAtSubCommandOption(commandLineIndex, isOptionOrOptionWithValueOrAnyDash) );
      REQUIRE( attributes.findSubCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex, isPositionalArgument) < 0 );
    }
  }
}
