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
#include "Mdt/CommandLineParser/BashCompletion/ActionList.h"
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

TEST_CASE("toCompreplyArrayItemString")
{
  Action action;
  QString expectedString;

  SECTION("Default constructed")
  {
    REQUIRE( action.toCompreplyArrayItemString().isEmpty() );
  }

  SECTION("List files compgen action")
  {
    CompgenCommand command;
    command.addAction(CompgenAction::ListFiles);
    action.setCompgenCommand(command);
    expectedString = QLatin1String("$(compgen -A file -- \"$cur\")");
    REQUIRE( action.toCompreplyArrayItemString() == expectedString );
  }

  SECTION("Custom action using curl")
  {
    action.setCustomAction( QLatin1String("curl www.somesite.org") );
    expectedString = QLatin1String("$(curl www.somesite.org)");
    REQUIRE( action.toCompreplyArrayItemString() == expectedString );
  }

  SECTION("Custom action to list packages")
  {
    action.setCustomAction( QLatin1String("compgen -W \"$(\"$executable\" completion-list-packages)\" -- \"$cur\"") );
    expectedString = QLatin1String("$(compgen -W \"$(\"$executable\" completion-list-packages)\" -- \"$cur\")");
    REQUIRE( action.toCompreplyArrayItemString() == expectedString );
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
    expectedString = QLatin1String("COMPREPLY=($(curl www.somesite.org))");
    REQUIRE( action.toCompreplyString() == expectedString );
  }

  SECTION("Custom action to list packages")
  {
    action.setCustomAction( QLatin1String("compgen -W \"$(\"$executable\" completion-list-packages)\" -- \"$cur\"") );
    expectedString = QLatin1String("COMPREPLY=($(compgen -W \"$(\"$executable\" completion-list-packages)\" -- \"$cur\"))");
    REQUIRE( action.toCompreplyString() == expectedString );
  }
}

TEST_CASE("ActionList_attributes")
{
  ActionList list;

  SECTION("Default constructed")
  {
    REQUIRE( list.isEmpty() );
    REQUIRE( list.actionCount() == 0 );
  }

  SECTION("Add a action")
  {
    Action action;
    CompgenCommand command;
    command.addAction(CompgenAction::ListFiles);
    action.setCompgenCommand(command);

    list.addAction(action);
    REQUIRE( !list.isEmpty() );
    REQUIRE( list.actionCount() == 1 );
    REQUIRE( list.actionAt(0).isDefined() );
  }
}

TEST_CASE("ActionList_toCompreplyString")
{
  Action action;
  ActionList list;
  QString expectedString;

  SECTION("Default constructed")
  {
    REQUIRE( list.toCompreplyString().isEmpty() );
  }

  SECTION("List files compgen action")
  {
    CompgenCommand command;
    command.addAction(CompgenAction::ListFiles);
    action.setCompgenCommand(command);
    list.addAction(action);
    expectedString = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
    REQUIRE( list.toCompreplyString() == expectedString );
  }

  SECTION("2 compgen actions")
  {
    CompgenCommand command1;
    command1.addAction(CompgenAction::ListFiles);
    action.setCompgenCommand(command1);
    list.addAction(action);

    CompgenCommand command2;
    command2.addAction(CompgenAction::ListDirectories);
    action.setCompgenCommand(command2);
    list.addAction(action);

    expectedString = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\") $(compgen -A directory -- \"$cur\"))");
    REQUIRE( list.toCompreplyString() == expectedString );
  }
}
