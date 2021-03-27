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

TEST_CASE("getOptionName")
{
  Argument argument;

  SECTION("Option")
  {
    argument = Option{QLatin1String("help")};
    REQUIRE( getOptionName(argument) == QLatin1String("help") );
  }

  SECTION("Unknown option")
  {
    argument = UnknownOption{QLatin1String("unknown-option")};
    REQUIRE( getOptionName(argument) == QLatin1String("unknown-option") );
  }

  SECTION("Option expecting value")
  {
    argument = Option{QLatin1String("copy-mode"),true};
    REQUIRE( getOptionName(argument) == QLatin1String("copy-mode") );
  }

  SECTION("OptionValue")
  {
    argument = OptionValue{QLatin1String("help")};
    REQUIRE( getOptionName(argument).isEmpty() );
  }

  SECTION("OptionWithValue")
  {
    argument = OptionWithValue{QLatin1String("help"),QLatin1String("true")};
    REQUIRE( getOptionName(argument) == QLatin1String("help") );
  }

  SECTION("-")
  {
    argument = SingleDash{};
    REQUIRE( getOptionName(argument).isEmpty() );
  }

  SECTION("--")
  {
    argument = DoubleDash{};
    REQUIRE( getOptionName(argument).isEmpty() );
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

  SECTION("Short option list")
  {
    argument = ShortOptionList{{'h'},false};
    REQUIRE( !isOptionExpectingValue(argument) );
  }

  SECTION("Short option list with the last one expecting a value")
  {
    argument = ShortOptionList{{'o'},true};
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

TEST_CASE("argumentTypeChecksAndGetters")
{
  Argument executableName = Executable{QLatin1String("app")};
  Argument subCommandName = SubCommandName{QLatin1String("copy")};
  Argument positionalArgument = PositionalArgument{QLatin1String("file.txt")};
  Argument option = Option{QLatin1String("help")};
  Argument unknownOption = UnknownOption{QLatin1String("unknown-option")};
  Argument shortOptionList = ShortOptionList{{'f','h'}};
  Argument optionValue = OptionValue{QLatin1String("keep")};
  Argument optionWithValue = OptionWithValue{QLatin1String("dir"),QLatin1String("/tmp")};
  Argument shortOptionListWithLastHavingValue = ShortOptionListWithLastHavingValue{{'f','o'},QLatin1String("skip")};
  Argument singleDash = SingleDash{};
  Argument doubleDash = DoubleDash{};

  SECTION("isExecutableName")
  {
    REQUIRE( isExecutableName(executableName) );
    REQUIRE( !isExecutableName(subCommandName) );
    REQUIRE( !isExecutableName(positionalArgument) );
    REQUIRE( !isExecutableName(option) );
    REQUIRE( !isExecutableName(unknownOption) );
    REQUIRE( !isExecutableName(shortOptionList) );
    REQUIRE( !isExecutableName(optionValue) );
    REQUIRE( !isExecutableName(optionWithValue) );
    REQUIRE( !isExecutableName(shortOptionListWithLastHavingValue) );
    REQUIRE( !isExecutableName(singleDash) );
    REQUIRE( !isExecutableName(doubleDash) );
  }

  SECTION("isSingleDash")
  {
    REQUIRE( !isSingleDash(executableName) );
    REQUIRE( !isSingleDash(subCommandName) );
    REQUIRE( !isSingleDash(positionalArgument) );
    REQUIRE( !isSingleDash(option) );
    REQUIRE( !isSingleDash(unknownOption) );
    REQUIRE( !isSingleDash(shortOptionList) );
    REQUIRE( !isSingleDash(optionValue) );
    REQUIRE( !isSingleDash(optionWithValue) );
    REQUIRE( !isSingleDash(shortOptionListWithLastHavingValue) );
    REQUIRE( isSingleDash(singleDash) );
    REQUIRE( !isSingleDash(doubleDash) );
  }

  SECTION("isDoubleDash")
  {
    REQUIRE( !isDoubleDash(executableName) );
    REQUIRE( !isDoubleDash(subCommandName) );
    REQUIRE( !isDoubleDash(positionalArgument) );
    REQUIRE( !isDoubleDash(option) );
    REQUIRE( !isDoubleDash(unknownOption) );
    REQUIRE( !isDoubleDash(shortOptionList) );
    REQUIRE( !isDoubleDash(optionValue) );
    REQUIRE( !isDoubleDash(optionWithValue) );
    REQUIRE( !isDoubleDash(shortOptionListWithLastHavingValue) );
    REQUIRE( !isDoubleDash(singleDash) );
    REQUIRE( isDoubleDash(doubleDash) );
  }

  SECTION("isUnknownOption")
  {
    REQUIRE( !isUnknownOption(executableName) );
    REQUIRE( !isUnknownOption(subCommandName) );
    REQUIRE( !isUnknownOption(positionalArgument) );
    REQUIRE( !isUnknownOption(option) );
    REQUIRE( isUnknownOption(unknownOption) );
    REQUIRE( !isUnknownOption(shortOptionList) );
    REQUIRE( !isUnknownOption(optionValue) );
    REQUIRE( !isUnknownOption(optionWithValue) );
    REQUIRE( !isUnknownOption(shortOptionListWithLastHavingValue) );
    REQUIRE( !isUnknownOption(singleDash) );
    REQUIRE( !isUnknownOption(doubleDash) );
  }

  SECTION("isShortOptionList")
  {
    REQUIRE( !isShortOptionList(executableName) );
    REQUIRE( !isShortOptionList(subCommandName) );
    REQUIRE( !isShortOptionList(positionalArgument) );
    REQUIRE( !isShortOptionList(option) );
    REQUIRE( !isShortOptionList(unknownOption) );
    REQUIRE( isShortOptionList(shortOptionList) );
    REQUIRE( !isShortOptionList(optionValue) );
    REQUIRE( !isShortOptionList(optionWithValue) );
    REQUIRE( !isShortOptionList(shortOptionListWithLastHavingValue) );
    REQUIRE( !isShortOptionList(singleDash) );
    REQUIRE( !isShortOptionList(doubleDash) );
  }

  SECTION("isOptionValue")
  {
    REQUIRE( !isOptionValue(executableName) );
    REQUIRE( !isOptionValue(subCommandName) );
    REQUIRE( !isOptionValue(positionalArgument) );
    REQUIRE( !isOptionValue(option) );
    REQUIRE( !isOptionValue(unknownOption) );
    REQUIRE( !isOptionValue(shortOptionList) );
    REQUIRE( isOptionValue(optionValue) );
    REQUIRE( !isOptionValue(optionWithValue) );
    REQUIRE( !isOptionValue(shortOptionListWithLastHavingValue) );
    REQUIRE( !isOptionValue(singleDash) );
    REQUIRE( !isOptionValue(doubleDash) );
  }

  SECTION("isOptionWithValue")
  {
    REQUIRE( !isOptionWithValue(executableName) );
    REQUIRE( !isOptionWithValue(subCommandName) );
    REQUIRE( !isOptionWithValue(positionalArgument) );
    REQUIRE( !isOptionWithValue(option) );
    REQUIRE( !isOptionWithValue(unknownOption) );
    REQUIRE( !isOptionWithValue(shortOptionList) );
    REQUIRE( !isOptionWithValue(optionValue) );
    REQUIRE( isOptionWithValue(optionWithValue) );
    REQUIRE( !isOptionWithValue(shortOptionListWithLastHavingValue) );
    REQUIRE( !isOptionWithValue(singleDash) );
    REQUIRE( !isOptionWithValue(doubleDash) );
  }

  SECTION("isShortOptionListWithLastHavingValue")
  {
    REQUIRE( !isShortOptionListWithLastHavingValue(executableName) );
    REQUIRE( !isShortOptionListWithLastHavingValue(subCommandName) );
    REQUIRE( !isShortOptionListWithLastHavingValue(positionalArgument) );
    REQUIRE( !isShortOptionListWithLastHavingValue(option) );
    REQUIRE( !isShortOptionListWithLastHavingValue(unknownOption) );
    REQUIRE( !isShortOptionListWithLastHavingValue(shortOptionList) );
    REQUIRE( !isShortOptionListWithLastHavingValue(optionValue) );
    REQUIRE( !isShortOptionListWithLastHavingValue(optionWithValue) );
    REQUIRE( isShortOptionListWithLastHavingValue(shortOptionListWithLastHavingValue) );
    REQUIRE( !isShortOptionListWithLastHavingValue(singleDash) );
    REQUIRE( !isShortOptionListWithLastHavingValue(doubleDash) );
  }

  SECTION("getOptionValue")
  {
    REQUIRE( getOptionValue(executableName).isEmpty() );
    REQUIRE( getOptionValue(subCommandName).isEmpty() );
    REQUIRE( getOptionValue(positionalArgument).isEmpty() );
    REQUIRE( getOptionValue(option).isEmpty() );
    REQUIRE( getOptionValue(unknownOption).isEmpty() );
    REQUIRE( getOptionValue(shortOptionList).isEmpty() );
    REQUIRE( getOptionValue(optionValue) == QLatin1String("keep") );
    REQUIRE( getOptionValue(optionWithValue) == QLatin1String("/tmp") );
    REQUIRE( getOptionValue(shortOptionListWithLastHavingValue) == QLatin1String("skip") );
    REQUIRE( getOptionValue(singleDash).isEmpty() );
    REQUIRE( getOptionValue(doubleDash).isEmpty() );
  }

  SECTION("getShortOptionListNames")
  {
    REQUIRE( getShortOptionListNames(executableName).empty() );
    REQUIRE( getShortOptionListNames(subCommandName).empty() );
    REQUIRE( getShortOptionListNames(positionalArgument).empty() );
    REQUIRE( getShortOptionListNames(option).empty() );
    REQUIRE( getShortOptionListNames(unknownOption).empty() );
    REQUIRE( getShortOptionListNames(shortOptionList) == std::vector<char>{'f','h'} );
    REQUIRE( getShortOptionListNames(optionValue).empty() );
    REQUIRE( getShortOptionListNames(optionWithValue).empty() );
    REQUIRE( getShortOptionListNames(shortOptionListWithLastHavingValue) == std::vector<char>{'f','o'} );
    REQUIRE( getShortOptionListNames(singleDash).empty() );
    REQUIRE( getShortOptionListNames(doubleDash).empty() );
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
    REQUIRE( commandLine.executableName() == QLatin1String("myapp") );
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
//     REQUIRE( commandLine.argumentAtPositionalArgumentValue(6)

    REQUIRE( isOptionExpectingValue( commandLine.argumentAt(1) ) );
    REQUIRE( !isOptionExpectingValue( commandLine.argumentAt(2) ) );
    REQUIRE( !isOptionExpectingValue( commandLine.argumentAt(3) ) );

    commandLine.clear();
    REQUIRE( commandLine.argumentCount() == 0 );
  }

  SECTION("myapp -fh")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendShortOptionList({'f','h'});

    REQUIRE( commandLine.argumentCount() == 2 );
//     REQUIRE( !isOptionExpectingValue( commandLine.argumentAt(1) ) );
  }

  SECTION("myapp -fo keep")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendShortOptionListWithLastExpectingValue({'f','o'});
    commandLine.appendOptionValue( QLatin1String("keep") );

    REQUIRE( commandLine.argumentCount() == 3 );
  }

  SECTION("myapp -fo=keep")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendShortOptionListWithLastHavingValue({'f','o'},QLatin1String("keep"));

    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isShortOptionListWithLastHavingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
  }

  SECTION("myapp --unknown-option")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendUnknownOption( QLatin1String("unknown-option") );

    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("unknown-option") );
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

TEST_CASE("findSubCommandName")
{
  CommandLine commandLine;

  SECTION("empty")
  {
    REQUIRE( findSubCommandName(commandLine).isEmpty() );
  }

  SECTION("app")
  {
    commandLine.setExecutableName( QLatin1String("app") );
    REQUIRE( findSubCommandName(commandLine).isEmpty() );
  }

  SECTION("app copy")
  {
    commandLine.setExecutableName( QLatin1String("app") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    REQUIRE( findSubCommandName(commandLine) == QLatin1String("copy") );
  }
}
