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
#include "Mdt/CommandLineParser/Impl/ParserResultFromCommandLine.h"
#include <QLatin1String>

using namespace Mdt::CommandLineParser;

TEST_CASE("parserResultFromCommandLine")
{
  using Impl::parserResultFromCommandLine;

  Mdt::CommandLineParser::CommandLine::CommandLine commandLine;
  ParserResult result;

  SECTION("empty")
  {
    result = parserResultFromCommandLine(commandLine);
    REQUIRE( !result.hasPositionalArguments() );
  }

  SECTION("myapp file.txt")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );

    result = parserResultFromCommandLine(commandLine);
    REQUIRE( result.positionalArgumentCount() == 1 );
    REQUIRE( result.positionalArgumentAt(0) == QLatin1String("file.txt") );
  }

  SECTION("myapp copy")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendSubCommandName( QLatin1String("copy") );

    result = parserResultFromCommandLine(commandLine);
    REQUIRE( !result.hasPositionalArguments() );
    REQUIRE( result.subCommand().name() == QLatin1String("copy") );
  }

  SECTION("myapp app.conf -f copy --mode keep -r=true file.txt /tmp - -- --as-positional-argument")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendPositionalArgument( QLatin1String("app.conf") );
    commandLine.appendOption( QLatin1String("f") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendOption( QLatin1String("mode") );
    commandLine.appendOptionValue( QLatin1String("keep") );
    commandLine.appendOptionWithValue( QLatin1String("r"), QLatin1String("true") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );
    commandLine.appendSingleDash();
    commandLine.appendDoubleDash();
    commandLine.appendPositionalArgument( QLatin1String("--as-positional-argument") );

    result = parserResultFromCommandLine(commandLine);
    REQUIRE( result.mainCommand().positionalArgumentCount() == 1 );
    REQUIRE( result.mainCommand().positionalArgumentAt(0) == QLatin1String("app.conf") );
    REQUIRE( result.mainCommand().optionCount() == 1 );
    REQUIRE( result.mainCommand().options()[0].name() == QLatin1String("f") );
    REQUIRE( !result.mainCommand().options()[0].hasValue() );
    REQUIRE( result.subCommand().name() == QLatin1String("copy") );
    REQUIRE( result.subCommand().positionalArgumentCount() == 4 );
    REQUIRE( result.subCommand().positionalArgumentAt(0) == QLatin1String("file.txt") );
    REQUIRE( result.subCommand().positionalArgumentAt(1) == QLatin1String("/tmp") );
    REQUIRE( result.subCommand().positionalArgumentAt(2) == QLatin1String("-") );
    REQUIRE( result.subCommand().positionalArgumentAt(3) == QLatin1String("--as-positional-argument") );
    REQUIRE( result.subCommand().optionCount() == 2 );
    REQUIRE( result.subCommand().options()[0].name() == QLatin1String("mode") );
    REQUIRE( result.subCommand().options()[0].value() == QLatin1String("keep") );
    REQUIRE( result.subCommand().options()[1].name() == QLatin1String("r") );
    REQUIRE( result.subCommand().options()[1].value() == QLatin1String("true") );
  }

}
