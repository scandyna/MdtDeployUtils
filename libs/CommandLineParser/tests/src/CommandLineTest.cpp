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
#include "Mdt/CommandLineParser/CommandLine/CommandLine.h"
#include "Mdt/CommandLineParser/CommandLine/Algorithm.h"
#include "Mdt/CommandLineParser/CommandLine/CommandLineAlgorithm.h"
#include <QLatin1String>

using namespace Mdt::CommandLineParser::CommandLine;

TEST_CASE("isOption")
{
  Argument argument;

  SECTION("Option")
  {
    argument = Option{QLatin1String("help")};
    REQUIRE( isOption(argument) );
  }

  SECTION("OptionValue")
  {
    argument = OptionValue{QLatin1String("help")};
    REQUIRE( !isOption(argument) );
  }

  SECTION("OptionWithValue")
  {
    argument = OptionWithValue{QLatin1String("help"),QLatin1String("true")};
    REQUIRE( !isOption(argument) );
  }
}

TEST_CASE("isOptionOrOptionWithValueOrAnyDash")
{
  Argument argument;

  SECTION("Option")
  {
    argument = Option{QLatin1String("help")};
    REQUIRE( isOptionOrOptionWithValueOrAnyDash(argument) );
  }

  SECTION("Option expecting value")
  {
    argument = Option{QLatin1String("copy-mode"),true};
    REQUIRE( isOptionOrOptionWithValueOrAnyDash(argument) );
  }

  SECTION("OptionValue")
  {
    argument = OptionValue{QLatin1String("help")};
    REQUIRE( !isOptionOrOptionWithValueOrAnyDash(argument) );
  }

  SECTION("OptionWithValue")
  {
    argument = OptionWithValue{QLatin1String("help"),QLatin1String("true")};
    REQUIRE( isOptionOrOptionWithValueOrAnyDash(argument) );
  }

  SECTION("-")
  {
    argument = SingleDash{};
    REQUIRE( isOptionOrOptionWithValueOrAnyDash(argument) );
  }

  SECTION("--")
  {
    argument = DoubleDash{};
    REQUIRE( isOptionOrOptionWithValueOrAnyDash(argument) );
  }
}

TEST_CASE("isOptionExpectingValue")
{
  Argument argument;

  SECTION("Option")
  {
    argument = Option{QLatin1String("help")};
    REQUIRE( !isOptionExpectingValue(argument) );
  }

  SECTION("Option expecting value")
  {
    argument = Option{QLatin1String("copy-mode"),true};
    REQUIRE( isOptionExpectingValue(argument) );
  }

  SECTION("OptionValue")
  {
    argument = OptionValue{QLatin1String("help")};
    REQUIRE( !isOptionExpectingValue(argument) );
  }

  SECTION("OptionWithValue")
  {
    argument = OptionWithValue{QLatin1String("help"),QLatin1String("true")};
    REQUIRE( !isOptionExpectingValue(argument) );
  }

  SECTION("-")
  {
    argument = SingleDash{};
    REQUIRE( !isOptionExpectingValue(argument) );
  }

  SECTION("--")
  {
    argument = DoubleDash{};
    REQUIRE( !isOptionExpectingValue(argument) );
  }
}

TEST_CASE("CommandLine")
{
  CommandLine commandLine;

  SECTION("empty")
  {
    REQUIRE( commandLine.argumentCount() == 0 );
    REQUIRE( commandLine.isEmpty() );
  }

  SECTION("myapp")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    REQUIRE( commandLine.argumentCount() == 1 );
    REQUIRE( !commandLine.isEmpty() );
  }

  SECTION("myapp file.txt")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    REQUIRE( commandLine.argumentCount() == 2 );
  }

  SECTION("myapp copy")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    REQUIRE( commandLine.argumentCount() == 2 );
  }

  SECTION("myapp --copy-mode keep -f copy -r=true file.txt - --")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendOptionExpectingValue( QLatin1String("copy-mode") );
    commandLine.appendOptionValue( QLatin1String("keep") );
    commandLine.appendOption( QLatin1String("f") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendOptionWithValue( QLatin1String("r"), QLatin1String("true") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendSingleDash();
    commandLine.appendDoubleDash();
    REQUIRE( commandLine.argumentCount() == 9 );

    REQUIRE( !commandLine.argumentAtIsPositionalArgument(0) );
    REQUIRE( !commandLine.argumentAtIsPositionalArgument(1) );
    REQUIRE( !commandLine.argumentAtIsPositionalArgument(2) );
    REQUIRE( !commandLine.argumentAtIsPositionalArgument(3) );
    REQUIRE( !commandLine.argumentAtIsPositionalArgument(4) );
    REQUIRE( !commandLine.argumentAtIsPositionalArgument(5) );
    REQUIRE(  commandLine.argumentAtIsPositionalArgument(6) );
    REQUIRE( !commandLine.argumentAtIsPositionalArgument(7) );
    REQUIRE( !commandLine.argumentAtIsPositionalArgument(8) );

    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(6) ) == QLatin1String("file.txt") );

    REQUIRE( isOptionExpectingValue( commandLine.argumentAt(1) ) );
    REQUIRE( !isOptionExpectingValue( commandLine.argumentAt(2) ) );
    REQUIRE( !isOptionExpectingValue( commandLine.argumentAt(3) ) );
  }
}

TEST_CASE("findSubCommandNameArgument")
{
  CommandLine commandLine;

  SECTION("empty")
  {
    REQUIRE( findSubCommandNameArgumentIndex(commandLine) < 0 );
  }

  SECTION("myapp")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    REQUIRE( findSubCommandNameArgument(commandLine) == commandLine.argumentList().cend() );
    REQUIRE( findSubCommandNameArgumentIndex(commandLine) < 0 );
  }

  SECTION("myapp copy")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    REQUIRE( findSubCommandNameArgumentIndex(commandLine) == 1 );
  }

  SECTION("myapp copy --help")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendOption( QLatin1String("help") );
    REQUIRE( findSubCommandNameArgumentIndex(commandLine) == 1 );
  }
}
