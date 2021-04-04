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

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  ParserDefinition parserDefinition;
  parserDefinition.addSubCommand(copyCommand);

  Mdt::CommandLineParser::CommandLine::CommandLine commandLine;
  ParserResult result;

  SECTION("empty")
  {
    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( !result.hasPositionalArguments() );
  }

  SECTION("myapp file.txt")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );

    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( result.positionalArgumentCount() == 1 );
    REQUIRE( result.positionalArgumentAt(0) == QLatin1String("file.txt") );
  }

  SECTION("myapp copy")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendSubCommandName( QLatin1String("copy") );

    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( !result.hasPositionalArguments() );
    REQUIRE( result.subCommand().name() == QLatin1String("copy") );
  }

  SECTION("myapp app.conf -f copy --mode keep -r=true file.txt /tmp - -- --as-positional-argument")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendPositionalArgument( QLatin1String("app.conf") );
    commandLine.appendOption( QLatin1String("f") );
    commandLine.appendSubCommandName( QLatin1String("copy") );
    commandLine.appendOptionExpectingValue( QLatin1String("mode") );
    commandLine.appendOptionValue( QLatin1String("keep") );
    commandLine.appendOptionWithValue( QLatin1String("r"), QLatin1String("true") );
    commandLine.appendPositionalArgument( QLatin1String("file.txt") );
    commandLine.appendPositionalArgument( QLatin1String("/tmp") );
    commandLine.appendSingleDash();
    commandLine.appendDoubleDash();
    commandLine.appendPositionalArgument( QLatin1String("--as-positional-argument") );

    result = parserResultFromCommandLine(commandLine, parserDefinition);
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

  SECTION("myapp -fh")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendShortOptionList({'f','h'});

    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( result.mainCommand().optionCount() == 2 );
    REQUIRE( result.mainCommand().options()[0].name() == QLatin1String("f") );
    REQUIRE( !result.mainCommand().options()[0].hasValue() );
    REQUIRE( result.mainCommand().options()[1].name() == QLatin1String("h") );
    REQUIRE( !result.mainCommand().options()[1].hasValue() );
  }

  SECTION("myapp -fm keep")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendShortOptionListWithLastExpectingValue({'f','m'});
    commandLine.appendOptionValue( QLatin1String("keep") );

    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( result.mainCommand().optionCount() == 2 );
    REQUIRE( result.mainCommand().options()[0].name() == QLatin1String("f") );
    REQUIRE( !result.mainCommand().options()[0].hasValue() );
    REQUIRE( result.mainCommand().options()[1].name() == QLatin1String("m") );
    REQUIRE( result.mainCommand().options()[1].hasValue() );
    REQUIRE( result.mainCommand().options()[1].value() == QLatin1String("keep") );
  }

  SECTION("myapp -fm=keep")
  {
    commandLine.setExecutableName( QLatin1String("myapp") );
    commandLine.appendShortOptionListWithLastHavingValue( {'f','m'}, QLatin1String("keep") );

    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( result.mainCommand().optionCount() == 2 );
    REQUIRE( result.mainCommand().options()[0].name() == QLatin1String("f") );
    REQUIRE( !result.mainCommand().options()[0].hasValue() );
    REQUIRE( result.mainCommand().options()[1].name() == QLatin1String("m") );
    REQUIRE( result.mainCommand().options()[1].hasValue() );
    REQUIRE( result.mainCommand().options()[1].value() == QLatin1String("keep") );
  }
}

TEST_CASE("addOptionsWithDefaultValuesMissingInResultCommand")
{
  using Impl::addOptionsWithDefaultValuesMissingInResultCommand;

  // Option without default value
  ParserDefinitionOption overwriteBehaviorOption( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinitionOption paramOption( 'p', QLatin1String("param"), QLatin1String("Pass parameter to the backend") );
  paramOption.setValueName( QLatin1String("key=value") );
  paramOption.setDefaultValues( {QLatin1String("storage=sql"),QLatin1String("render=glx")} );

  ParserDefinitionCommand definitionCommand;
  definitionCommand.addOption(paramOption);

  ParserResultCommand resultCommand;

  SECTION("No option provided")
  {
    addOptionsWithDefaultValuesMissingInResultCommand(definitionCommand, resultCommand);
    REQUIRE( resultCommand.optionCount() == 2 );
    REQUIRE( resultCommand.options()[0].name() == QLatin1String("param") );
    REQUIRE( resultCommand.options()[0].value() == QLatin1String("storage=sql") );
    REQUIRE( resultCommand.options()[1].name() == QLatin1String("param") );
    REQUIRE( resultCommand.options()[1].value() == QLatin1String("render=glx") );
  }

  SECTION("--param storage=csv provided")
  {
    ParserResultOption option( QLatin1String("param") );
    option.setValue( QLatin1String("storage=csv") );
    resultCommand.addOption(option);

    addOptionsWithDefaultValuesMissingInResultCommand(definitionCommand, resultCommand);
    REQUIRE( resultCommand.optionCount() == 1 );
    REQUIRE( resultCommand.options()[0].name() == QLatin1String("param") );
    REQUIRE( resultCommand.options()[0].value() == QLatin1String("storage=csv") );
  }

  SECTION("-p storage=csv provided")
  {
    ParserResultOption option( QLatin1String("p") );
    option.setValue( QLatin1String("storage=csv") );
    resultCommand.addOption(option);

    addOptionsWithDefaultValuesMissingInResultCommand(definitionCommand, resultCommand);
    REQUIRE( resultCommand.optionCount() == 1 );
    REQUIRE( resultCommand.options()[0].name() == QLatin1String("p") );
    REQUIRE( resultCommand.options()[0].value() == QLatin1String("storage=csv") );
  }

  SECTION("overwrite-behavior provided")
  {
    ParserResultOption option( QLatin1String("overwrite-behavior") );
    option.setValue( QLatin1String("keep") );
    resultCommand.addOption(option);

    addOptionsWithDefaultValuesMissingInResultCommand(definitionCommand, resultCommand);
    REQUIRE( resultCommand.optionCount() == 3 );
    REQUIRE( resultCommand.options()[0].name() == QLatin1String("overwrite-behavior") );
    REQUIRE( resultCommand.options()[0].value() == QLatin1String("keep") );
    REQUIRE( resultCommand.options()[1].name() == QLatin1String("param") );
    REQUIRE( resultCommand.options()[1].value() == QLatin1String("storage=sql") );
    REQUIRE( resultCommand.options()[2].name() == QLatin1String("param") );
    REQUIRE( resultCommand.options()[2].value() == QLatin1String("render=glx") );
  }
}

TEST_CASE("optionsWithDefaultValues")
{
  using Impl::parserResultFromCommandLine;

  Mdt::CommandLineParser::CommandLine::CommandLine commandLine;
  ParserResult result;

  ParserDefinitionOption paramOption( 'p', QLatin1String("param"), QLatin1String("Pass parameter to the backend") );
  paramOption.setValueName( QLatin1String("key=value") );
  paramOption.setDefaultValues( {QLatin1String("storage=sql"),QLatin1String("render=glx")} );

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addOption(paramOption);

  ParserDefinition parserDefinition;
  parserDefinition.addOption(paramOption);
  parserDefinition.addSubCommand(copyCommand);

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("app --param storage=csv --param render=sw (values provided on the command line)")
  {
    commandLine.appendOptionExpectingValue( QLatin1String("param") );
    commandLine.appendOptionValue( QLatin1String("storage=csv") );
    commandLine.appendOptionExpectingValue( QLatin1String("param") );
    commandLine.appendOptionValue( QLatin1String("render=sw") );

    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( result.mainCommand().optionCount() == 2 );
    REQUIRE( result.mainCommand().options()[0].name() == QLatin1String("param") );
    REQUIRE( result.mainCommand().options()[0].value() == QLatin1String("storage=csv") );
    REQUIRE( result.mainCommand().options()[1].name() == QLatin1String("param") );
    REQUIRE( result.mainCommand().options()[1].value() == QLatin1String("render=sw") );
  }

  SECTION("app --param storage=csv (one value provided on the command line)")
  {
    commandLine.appendOptionExpectingValue( QLatin1String("param") );
    commandLine.appendOptionValue( QLatin1String("storage=csv") );

    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( result.mainCommand().optionCount() == 1 );
    REQUIRE( result.mainCommand().options()[0].name() == QLatin1String("param") );
    REQUIRE( result.mainCommand().options()[0].value() == QLatin1String("storage=csv") );
  }

  SECTION("app (no value provided on the command line, use default ones)")
  {
    result = parserResultFromCommandLine(commandLine, parserDefinition);
    REQUIRE( result.mainCommand().optionCount() == 2 );
    REQUIRE( result.mainCommand().options()[0].name() == QLatin1String("param") );
    REQUIRE( result.mainCommand().options()[0].value() == QLatin1String("storage=sql") );
    REQUIRE( result.mainCommand().options()[1].name() == QLatin1String("param") );
    REQUIRE( result.mainCommand().options()[1].value() == QLatin1String("render=glx") );
  }

  SECTION("copy sub-command")
  {
    commandLine.appendSubCommandName( QLatin1String("copy") );

    SECTION("app copy --param storage=csv --param render=sw (values provided on the command line)")
    {
      commandLine.appendOptionExpectingValue( QLatin1String("param") );
      commandLine.appendOptionValue( QLatin1String("storage=csv") );
      commandLine.appendOptionExpectingValue( QLatin1String("param") );
      commandLine.appendOptionValue( QLatin1String("render=sw") );

      result = parserResultFromCommandLine(commandLine, parserDefinition);
      REQUIRE( result.hasSubCommand() );
      REQUIRE( result.subCommand().optionCount() == 2 );
      REQUIRE( result.subCommand().options()[0].name() == QLatin1String("param") );
      REQUIRE( result.subCommand().options()[0].value() == QLatin1String("storage=csv") );
      REQUIRE( result.subCommand().options()[1].name() == QLatin1String("param") );
      REQUIRE( result.subCommand().options()[1].value() == QLatin1String("render=sw") );
    }

    SECTION("app (no value provided on the command line, use default ones)")
    {
      result = parserResultFromCommandLine(commandLine, parserDefinition);
      REQUIRE( result.hasSubCommand() );
      REQUIRE( result.subCommand().optionCount() == 2 );
      REQUIRE( result.subCommand().options()[0].name() == QLatin1String("param") );
      REQUIRE( result.subCommand().options()[0].value() == QLatin1String("storage=sql") );
      REQUIRE( result.subCommand().options()[1].name() == QLatin1String("param") );
      REQUIRE( result.subCommand().options()[1].value() == QLatin1String("render=glx") );
    }
  }
}
