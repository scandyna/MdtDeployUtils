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
#include "TestUtils.h"
#include "Mdt/CommandLineParser/ParserResult.h"
#include "Mdt/CommandLineParser/ParserDefinitionOption.h"
#include <QLatin1String>

using namespace Mdt::CommandLineParser;

TEST_CASE("ParserResultOption")
{
  SECTION("help")
  {
    ParserResultOption option( QLatin1String("help") );
    REQUIRE( option.name() == QLatin1String("help") );
  }

  SECTION("destination /tmp")
  {
    ParserResultOption option( QLatin1String("destination") );
    REQUIRE( !option.hasValue() );
    option.setValue( QLatin1String("/tmp") );
    REQUIRE( option.hasValue() );
    REQUIRE( option.value() == QLatin1String("/tmp") );
  }
}

TEST_CASE("ParserResultCommand_options_isSet")
{
  ParserDefinitionOption helpOptionDefinition( 'h', QLatin1String("help"), QLatin1String("Help option") );
  ParserResultCommand command;

  SECTION("help")
  {
    SECTION("Empty result command")
    {
      REQUIRE( !command.isOptionLongNameSet( QLatin1String("help") ) );
      REQUIRE( !command.isOptionShortNameSet('h') );
      REQUIRE( !command.isSet(helpOptionDefinition) );
      REQUIRE( !command.isHelpOptionSet() );
    }

    SECTION("--help")
    {
      command.addOption( ParserResultOption( QLatin1String("help") ) );
      REQUIRE( command.isOptionLongNameSet( QLatin1String("help") ) );
      REQUIRE( !command.isOptionShortNameSet('h') );
      REQUIRE( command.isSet(helpOptionDefinition) );
      REQUIRE( command.isHelpOptionSet() );
    }

    SECTION("-h")
    {
      command.addOption( ParserResultOption( QLatin1String("h") ) );
      REQUIRE( !command.isOptionLongNameSet( QLatin1String("help") ) );
      REQUIRE( command.isOptionShortNameSet('h') );
      REQUIRE( command.isSet(helpOptionDefinition) );
      REQUIRE( command.isHelpOptionSet() );
    }
  }
}

TEST_CASE("ParserResultCommand_options_getValues")
{
  ParserDefinitionOption paramOption( 'p', QLatin1String("param"), QLatin1String("Parameters") );
  paramOption.setValueName( QLatin1String("value") );

  ParserResultCommand command;

  SECTION("Empty result")
  {
    REQUIRE( command.getOptionLongNameValues( QLatin1String("param") ).isEmpty() );
    REQUIRE( command.getOptionShortNameValues('p').isEmpty() );
    REQUIRE( command.getValues(paramOption).isEmpty() );
  }

  SECTION("--param storage=csv")
  {
    ParserResultOption option( QLatin1String("param") );
    option.setValue( QLatin1String("storage=csv") );
    command.addOption(option);

    const QStringList expectedValues = qStringListFromUtf8Strings({"storage=csv"});
    REQUIRE( command.getOptionLongNameValues( QLatin1String("param") ) == expectedValues );
    REQUIRE( command.getOptionShortNameValues('p').isEmpty() );
    REQUIRE( command.getValues(paramOption) == expectedValues );
  }

  SECTION("-p storage=csv")
  {
    ParserResultOption option( QLatin1String("p") );
    option.setValue( QLatin1String("storage=csv") );
    command.addOption(option);

    const QStringList expectedValues = qStringListFromUtf8Strings({"storage=csv"});
    REQUIRE( command.getOptionLongNameValues( QLatin1String("param") ).isEmpty() );
    REQUIRE( command.getOptionShortNameValues('p') == expectedValues );
    REQUIRE( command.getValues(paramOption) == expectedValues );
  }

  SECTION("-p storage=csv --param render=sw")
  {
    ParserResultOption option1( QLatin1String("p") );
    option1.setValue( QLatin1String("storage=csv") );
    ParserResultOption option2( QLatin1String("param") );
    option2.setValue( QLatin1String("render=sw") );
    command.addOption(option1);
    command.addOption(option2);

    REQUIRE( command.getOptionLongNameValues( QLatin1String("param") ) == qStringListFromUtf8Strings({"render=sw"}) );
    REQUIRE( command.getOptionShortNameValues('p') == qStringListFromUtf8Strings({"storage=csv"}) );
    REQUIRE( command.getValues(paramOption) == qStringListFromUtf8Strings({"render=sw","storage=csv"}) );
  }
}

TEST_CASE("ParserResultCommand_options")
{
  ParserResultCommand command;

  SECTION("getValues")
  {
    ParserDefinitionOption destinationDefinition( QLatin1String("destination"), QLatin1String("Destination directory") );
    destinationDefinition.setValueName( QLatin1String("directory") );
    destinationDefinition.setDefaultValue( QLatin1String("/usr/bin") );

    QStringList expectedValues;

    SECTION("destination /tmp")
    {
      ParserResultOption destination( QLatin1String("destination") );
      destination.setValue( QLatin1String("/tmp") );
      command.addOption(destination);

      expectedValues << QLatin1String("/tmp");
      REQUIRE( command.getValues(destinationDefinition) == expectedValues );
    }

    SECTION("destination /tmp ~/opt")
    {
      ParserResultOption destination1( QLatin1String("destination") );
      destination1.setValue( QLatin1String("/tmp") );
      ParserResultOption destination2( QLatin1String("destination") );
      destination2.setValue( QLatin1String("~/opt") );
      command.addOption(destination1);
      command.addOption(destination2);

      expectedValues << QLatin1String("/tmp") << QLatin1String("~/opt");
      REQUIRE( command.getValues(destinationDefinition) == expectedValues );
    }

    SECTION("destination /tmp force")
    {
      ParserResultOption destination( QLatin1String("destination") );
      destination.setValue( QLatin1String("/tmp") );
      command.addOption(destination);
      command.addOption( ParserResultOption( QLatin1String("force") ) );

      expectedValues << QLatin1String("/tmp");
      REQUIRE( command.getValues(destinationDefinition) == expectedValues );
    }

    SECTION("source /usr destination /tmp")
    {
      ParserResultOption source( QLatin1String("source") );
      source.setValue( QLatin1String("/usr") );
      command.addOption(source);

      ParserResultOption destination( QLatin1String("destination") );
      destination.setValue( QLatin1String("/tmp") );
      command.addOption(destination);

      expectedValues << QLatin1String("/tmp");
      REQUIRE( command.getValues(destinationDefinition) == expectedValues );
    }
  }
}

TEST_CASE("ParserResultCommand_positionalArguments")
{
  ParserResultCommand command;

  SECTION("empty")
  {
    REQUIRE( command.positionalArgumentCount() == 0 );
    REQUIRE( !command.hasPositionalArgumentAt(0) );
  }

  SECTION("file.txt,/tmp")
  {
    command.addPositionalArgument( QLatin1String("file.txt") );
    command.addPositionalArgument( QLatin1String("/tmp") );
    REQUIRE( command.positionalArgumentCount() == 2 );
    REQUIRE( command.hasPositionalArgumentAt(0) );
    REQUIRE( command.positionalArgumentAt(0) == QLatin1String("file.txt") );
    REQUIRE( command.hasPositionalArgumentAt(1) );
    REQUIRE( command.positionalArgumentAt(1) == QLatin1String("/tmp") );
  }
}
