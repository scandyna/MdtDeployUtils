/****************************************************************************
 **
 ** Copyright (C) 2011-2021 Philippe Steinmann.
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
#include "Mdt/CommandLineParser/BashCompletion/Action.h"
#include <QString>
#include <QLatin1String>

using namespace Mdt::CommandLineParser::BashCompletion;

TEST_CASE("is_CompgenCommand_CustomAction")
{
  Action action;

  SECTION("Default constructed")
  {
    REQUIRE( !action.isCompgenCommand() );
    REQUIRE( !action.isCustomAction() );
  }

  SECTION("compgen command")
  {
    CompgenCommand command;
    command.addAction(CompgenAction::ListFiles);
    action.setCompgenCommand(command);
    REQUIRE( action.isCompgenCommand() );
    REQUIRE( !action.isCustomAction() );
  }

  SECTION("custom action")
  {
    action.setCustomAction( QLatin1String("$executable completion-list-packages") );
    REQUIRE( !action.isCompgenCommand() );
    REQUIRE( action.isCustomAction() );
  }
}

TEST_CASE("isDefined")
{
  Action action;

  SECTION("Default constructed")
  {
    REQUIRE( !action.isDefined() );
  }

  SECTION("Set a compgen command")
  {
    CompgenCommand command;
    command.addAction(CompgenAction::ListFiles);
    action.setCompgenCommand(command);
    REQUIRE( action.isDefined() );
  }

  SECTION("Set a custom action")
  {
    action.setCustomAction( QLatin1String("$executable completion-list-packages") );
    REQUIRE( action.isDefined() );
  }
}

TEST_CASE("toCompreplyString")
{
  Action action;
  QString expectedString;

  SECTION("Default constructed")
  {
    REQUIRE( action.toCompreplyString().isEmpty() );
  }

  SECTION("List files compgen action")
  {
    CompgenCommand command;
    command.addAction(CompgenAction::ListFiles);
    action.setCompgenCommand(command);
    expectedString = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
    REQUIRE( action.toCompreplyString() == expectedString );
  }

  SECTION("Custom action using curl")
  {
    action.setCustomAction( QLatin1String("curl www.somesite.org") );
    expectedString = QLatin1String("COMPREPLY=($(curl www.somesite.org -- \"$cur\"))");
    REQUIRE( action.toCompreplyString() == expectedString );
  }

  SECTION("Custom action to list packages")
  {
    action.setCustomAction( QLatin1String("\"$executable\" completion-list-packages") );
    expectedString = QLatin1String("COMPREPLY=($(\"$executable\" completion-list-packages -- \"$cur\"))");
    REQUIRE( action.toCompreplyString() == expectedString );
  }
}
