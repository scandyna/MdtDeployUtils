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
#include "Mdt/CommandLineParser/ParserResultInCommandLineIndexMap.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/Parser.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>

// #include <QDebug>

using namespace Mdt::CommandLineParser;

bool parseArgumentsToResult(const ParserDefinition & parserDefinition, const QStringList & arguments, ParserResult & result)
{
  Parser parser;

  result = parser.parse(parserDefinition, arguments);

//   qDebug() << result.errorText();

  return !result.hasError();
}


TEST_CASE("ParserResultInCommandLineIndexMap")
{
  ParserDefinition parserDefinition;
  QStringList arguments{QLatin1String("myapp")};
  ParserResult result;

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
    parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose option") );
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    SECTION("myapp")
    {
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      ParserResultInCommandLineIndexMap map(result);

      REQUIRE( map.commandLineArgumentCount() == 1 );

      SECTION("command-line index 0")
      {
        const int commandLineIndex = 0;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }
    }

    SECTION("app file.txt /tmp")
    {
      arguments << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      ParserResultInCommandLineIndexMap map(result);

      REQUIRE( map.commandLineArgumentCount() == 3 );

      SECTION("command-line index 0")
      {
        const int commandLineIndex = 0;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 1")
      {
        const int commandLineIndex = 1;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) == 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 2")
      {
        const int commandLineIndex = 2;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) == 1 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }
    }

    SECTION("app -f --verbose file.txt /tmp")
    {
      arguments << QLatin1String("-f") << QLatin1String("--verbose") << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      ParserResultInCommandLineIndexMap map(result);

      REQUIRE( map.commandLineArgumentCount() == 5 );

      SECTION("command-line index 0 (app)")
      {
        const int commandLineIndex = 0;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 1 (-f)")
      {
        const int commandLineIndex = 1;
        REQUIRE( map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 2 (--verbose)")
      {
        const int commandLineIndex = 2;
        REQUIRE( map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 3 (file.txt)")
      {
        const int commandLineIndex = 3;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) == 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 4 (/tmp)")
      {
        const int commandLineIndex = 4;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) == 1 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }
    }

    // Use case: Bash completion
    SECTION("app -")
    {
      arguments << QLatin1String("-");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      ParserResultInCommandLineIndexMap map(result);

      REQUIRE( map.commandLineArgumentCount() == 2 );

      SECTION("command-line index 0 (app)")
      {
        const int commandLineIndex = 0;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 1 (-)")
      {
        const int commandLineIndex = 1;
        REQUIRE( map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }
    }
  }

  SECTION("App with sub-command")
  {
    parserDefinition.addOption( QLatin1String("verbose"), QLatin1String("Verbose option") );
    parserDefinition.addPositionalArgument( QLatin1String("mode"), QLatin1String("Mode arg") );

    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.addHelpOption();
    copyCommand.addOption( 'f', QLatin1String("force"), QLatin1String("Force option") );
    copyCommand.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    copyCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
    parserDefinition.addSubCommand(copyCommand);

    SECTION("app")
    {
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      ParserResultInCommandLineIndexMap map(result);

      SECTION("command-line index 0 (app)")
      {
        const int commandLineIndex = 0;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }
    }

    SECTION("app --verbose arg1 copy -f file.txt /tmp")
    {
      arguments << QLatin1String("--verbose") << QLatin1String("arg1") << QLatin1String("copy") << QLatin1String("-f") << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      ParserResultInCommandLineIndexMap map(result);

      REQUIRE( map.commandLineArgumentCount() == 7 );

      SECTION("command-line index 0 (app)")
      {
        const int commandLineIndex = 0;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 1 (--verbose)")
      {
        const int commandLineIndex = 1;
        REQUIRE( map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 2 (arg1)")
      {
        const int commandLineIndex = 2;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) == 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 3 (copy)")
      {
        const int commandLineIndex = 3;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 4 (-f)")
      {
        const int commandLineIndex = 4;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 5 (file.txt)")
      {
        const int commandLineIndex = 5;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) == 0 );
      }

      SECTION("command-line index 6 (/tmp)")
      {
        const int commandLineIndex = 6;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) == 1 );
      }
    }

    // Use case: Bash completion
    SECTION("app -")
    {
      arguments << QLatin1String("-");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      ParserResultInCommandLineIndexMap map(result);

      REQUIRE( map.commandLineArgumentCount() == 2 );

      SECTION("command-line index 0 (app)")
      {
        const int commandLineIndex = 0;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 1 (-)")
      {
        const int commandLineIndex = 1;
        REQUIRE( map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }
    }

    // Use case: Bash completion
    SECTION("app copy -")
    {
      arguments << QLatin1String("copy") << QLatin1String("-");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      ParserResultInCommandLineIndexMap map(result);

      REQUIRE( map.commandLineArgumentCount() == 3 );

      SECTION("command-line index 0 (app)")
      {
        const int commandLineIndex = 0;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 1 (copy)")
      {
        const int commandLineIndex = 1;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( !map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }

      SECTION("command-line index 2 (-)")
      {
        const int commandLineIndex = 2;
        REQUIRE( !map.isCommandLineIndexAtMainCommandOption(commandLineIndex) );
        REQUIRE( map.mainCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
        REQUIRE( map.isCommandLineIndexAtSubCommandOption(commandLineIndex) );
        REQUIRE( map.subCommandPositionalArgumentIndexFromCommandLineIndex(commandLineIndex) < 0 );
      }
    }
  }
}
