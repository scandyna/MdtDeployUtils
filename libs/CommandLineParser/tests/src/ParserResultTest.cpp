/****************************************************************************
 **
 ** Copyright (C) 2020-2020 Philippe Steinmann.
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
#include "Mdt/CommandLineParser/ParserResult.h"
#include <QLatin1String>

using namespace Mdt::CommandLineParser;

TEST_CASE("ParserResult_error")
{
  ParserResult result;

  SECTION("No error")
  {
    REQUIRE( !result.hasError() );
  }

  SECTION("With error")
  {
    result.setErrorText( QLatin1String("Some error") );
    REQUIRE( result.hasError() );
    REQUIRE( result.errorText() == QLatin1String("Some error") );
  }
}

TEST_CASE("ParserResultCommand_options")
{
  ParserResultCommand command;

  SECTION("empty")
  {
    REQUIRE( !command.isSet( QLatin1String("verbose") ) );
  }

  SECTION("force")
  {
    command.addOption( ParserResultOption( QLatin1String("force") ) );
    REQUIRE( command.isSet( QLatin1String("force") ) );
    REQUIRE( !command.isSet( QLatin1String("verbose") ) );
  }

  SECTION("help")
  {
    command.addOption( ParserResultOption( QLatin1String("help") ) );
    REQUIRE( command.isHelpOptionSet() );
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
