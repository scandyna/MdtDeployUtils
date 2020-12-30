/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2020 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "catch2/catch.hpp"
#include "Mdt/CommandLineParser/BashCompletionParser.h"
#include "Mdt/CommandLineParser/Impl/BashCompletionParser.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/Parser.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>

using namespace Mdt::CommandLineParser;

bool parseArgumentsToResult(const ParserDefinition & parserDefinition, const QStringList & arguments, ParserResult & result)
{
  Parser parser;

  result = parser.parse(parserDefinition, arguments);

  return !result.hasError();
}


TEST_CASE("findCurrentPositionalArgumentName")
{
  using Impl::findCurrentPositionalArgumentName;
  using Impl::completionFindCurrentPositionalArgumentNameString;

  ParserDefinition parserDefinition;
  QStringList arguments{QLatin1String("myapp"),completionFindCurrentPositionalArgumentNameString()};
  ParserResult result;

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    SECTION("myapp completion-find-current-positional-argument-name")
    {
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("source") );
    }

    SECTION("myapp completion-find-current-positional-argument-name -h")
    {
      arguments << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("source") );
    }

    SECTION("myapp completion-find-current-positional-argument-name file.txt")
    {
      arguments << QLatin1String("file.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("destination") );
    }

    SECTION("myapp completion-find-current-positional-argument-name file.txt /tmp")
    {
      arguments << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition).isEmpty() );
    }
  }

  SECTION("App with subCommand")
  {
    parserDefinition.addHelpOption();

    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.addHelpOption();
    copyCommand.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    copyCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
    parserDefinition.addSubCommand(copyCommand);

    SECTION("myapp completion-find-current-positional-argument-name")
    {
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("command") );
    }

    SECTION("myapp completion-find-current-positional-argument-name -h")
    {
      arguments << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("command") );
    }

    SECTION("myapp completion-find-current-positional-argument-name unknown")
    {
      arguments << QLatin1String("unknown");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition).isEmpty() );
    }

    SECTION("myapp completion-find-current-positional-argument-name copy")
    {
      arguments << QLatin1String("copy");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-source") );
    }

    SECTION("myapp completion-find-current-positional-argument-name copy -h")
    {
      arguments << QLatin1String("copy") << QLatin1String("-h");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-source") );
    }

    SECTION("myapp completion-find-current-positional-argument-name copy file.txt")
    {
      arguments << QLatin1String("copy") << QLatin1String("file.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("copy-destination") );
    }

    SECTION("myapp completion-find-current-positional-argument-name copy file.txt /tmp")
    {
      arguments << QLatin1String("copy") << QLatin1String("file.txt") << QLatin1String("/tmp");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition).isEmpty() );
    }
  }

  SECTION("App with main positional arg and subCommand")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("file"), QLatin1String("File to threat") );

    ParserDefinitionCommand catCommand( QLatin1String("cat") );
    catCommand.addHelpOption();
    catCommand.addPositionalArgument( ArgumentType::File, QLatin1String("file1"), QLatin1String("File to cat") );
    parserDefinition.addSubCommand(catCommand);

    SECTION("myapp completion-find-current-positional-argument-name")
    {
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("file") );
    }

    SECTION("myapp completion-find-current-positional-argument-name file.txt")
    {
      arguments << QLatin1String("file.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("command") );
    }

    SECTION("myapp completion-find-current-positional-argument-name file.txt cat")
    {
      arguments << QLatin1String("file.txt") << QLatin1String("cat");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition) == QLatin1String("cat-file1") );
    }

    SECTION("myapp completion-find-current-positional-argument-name file.txt cat other.txt")
    {
      arguments << QLatin1String("file.txt") << QLatin1String("cat") << QLatin1String("other.txt");
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( findCurrentPositionalArgumentName(result, parserDefinition).isEmpty() );
    }
  }
}

TEST_CASE("handleBashCompletion")
{
  using Impl::completionFindCurrentPositionalArgumentNameString;

  ParserDefinition parserDefinition;
  QStringList arguments{QLatin1String("myapp")};
  ParserResult result;

  SECTION("Simple app")
  {
    parserDefinition.addHelpOption();
    parserDefinition.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );
    parserDefinition.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );

    SECTION("myapp")
    {
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( !handleBashCompletion(result, parserDefinition) );
    }

    SECTION("myapp completion-find-current-positional-argument-name")
    {
      arguments << completionFindCurrentPositionalArgumentNameString();
      REQUIRE( parseArgumentsToResult(parserDefinition, arguments, result) );
      REQUIRE( handleBashCompletion(result, parserDefinition) );
    }
  }
}
