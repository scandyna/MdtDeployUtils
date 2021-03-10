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
#include "Mdt/CommandLineParser/BashCompletion/CompgenCommand.h"
#include "Mdt/CommandLineParser/BashCompletion/CompgenArgumentAlgorithm.h"
#include <QString>
#include <QLatin1String>

using namespace Mdt::CommandLineParser::BashCompletion;

TEST_CASE("compgenArgumentToString")
{
  SECTION("List files")
  {
    REQUIRE( compgenArgumentToString(CompgenAction::ListFiles) == QLatin1String("-A file") );
  }

  SECTION("List directories")
  {
    REQUIRE( compgenArgumentToString(CompgenAction::ListDirectories) == QLatin1String("-A directory") );
  }

  SECTION("-h --help word list")
  {
    const CompgenWordList wordList{{QLatin1String("-h"),QLatin1String("--help")}};
    REQUIRE( compgenArgumentToString(wordList) == QLatin1String("-W \"-h --help\"") );
  }
}

TEST_CASE("attributes")
{
  CompgenCommand command;

  SECTION("Default constructed")
  {
    REQUIRE( command.argumentCount() == 0 );
    REQUIRE( command.isEmpty() );
  }

  SECTION("1 action")
  {
    command.addAction(CompgenAction::ListFiles);
    REQUIRE( command.argumentCount() == 1 );
    REQUIRE( !command.isEmpty() );
    REQUIRE( compgenArgumentToString( command.argumentAt(0) ) == QLatin1String("-A file") );
  }

  SECTION("1 word list")
  {
    command.addWordList({QLatin1String("-h"),QLatin1String("--help")});
    REQUIRE( command.argumentCount() == 1 );
    REQUIRE( !command.isEmpty() );
    REQUIRE( compgenArgumentToString( command.argumentAt(0) ) == QLatin1String("-W \"-h --help\"") );
  }
}

TEST_CASE("toCompgenCommandString")
{
  CompgenCommand command;
  const QString cur = QLatin1String("$cur");

  SECTION("no argument")
  {
    REQUIRE( command.toCompgenCommandString(cur) == QLatin1String("compgen -- \"$cur\"") );
  }

  SECTION("List files")
  {
    command.addAction(CompgenAction::ListFiles);
    REQUIRE( command.toCompgenCommandString(cur) == QLatin1String("compgen -A file -- \"$cur\"") );
  }

  SECTION("-h --help word list")
  {
    command.addWordList({QLatin1String("-h"),QLatin1String("--help")});
    REQUIRE( command.toCompgenCommandString(cur) == QLatin1String("compgen -W \"-h --help\" -- \"$cur\"") );
  }

  SECTION("List files and -h --help words")
  {
    command.addAction(CompgenAction::ListFiles);
    command.addWordList({QLatin1String("-h"),QLatin1String("--help")});
    REQUIRE( command.toCompgenCommandString(cur) == QLatin1String("compgen -A file -W \"-h --help\" -- \"$cur\"") );
  }
}
