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
#include "Mdt/CommandLineParser/Impl/Parser.h"
#include "Mdt/CommandLineParser/CommandLine/Algorithm.h"
#include <QLatin1String>
#include <QStringList>

using namespace Mdt::CommandLineParser;
using namespace Mdt::CommandLineParser::Impl;

TEST_CASE("findOptionByNameInCommand")
{
  ParserDefinitionCommand command;
  command.addHelpOption();

  SECTION("find help")
  {
    const auto it = findOptionByLongNameInCommand( QLatin1String("help"), command );
    REQUIRE( it != command.options().cend() );
    REQUIRE( it->name() == QLatin1String("help") );
  }

  SECTION("find h")
  {
    const auto it = findOptionByShortNameInCommand( 'h', command );
    REQUIRE( it != command.options().cend() );
    REQUIRE( it->name() == QLatin1String("help") );
  }

  SECTION("unknown option - by long name")
  {
    const auto it = findOptionByLongNameInCommand( QLatin1String("unknown"), command );
    REQUIRE( it == command.options().cend() );
  }

  SECTION("unknown option - by short name")
  {
    const auto it = findOptionByShortNameInCommand( 'u', command );
    REQUIRE( it == command.options().cend() );
  }
}

TEST_CASE("addShortOptionsToCommandLine")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );

  ParserDefinitionOption overwriteBehaviorOption( 'o', QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand command;
  command.addHelpOption();
  command.addOption(forceOption);
  command.addOption(overwriteBehaviorOption);

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("-h")
  {
    arguments = qStringListFromUtf8Strings({"-h"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o keep")
  {
    arguments = qStringListFromUtf8Strings({"-o","keep"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
    REQUIRE( current == (arguments.cbegin()+1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o")
  {
    arguments = qStringListFromUtf8Strings({"-o"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-fh")
  {
    arguments = qStringListFromUtf8Strings({"-fh"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','h'} );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo keep")
  {
    arguments = qStringListFromUtf8Strings({"-fo","keep"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
    REQUIRE( current == (arguments.cbegin()+1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo")
  {
    arguments = qStringListFromUtf8Strings({"-fo"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-of keep")
  {
    arguments = qStringListFromUtf8Strings({"-of","keep"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
//     REQUIRE( commandLine.argumentCount() == 3 );
//     REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
//     REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'o','f'} );
    /// \todo What should keep be ? A positional argument ?
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-of")
  {
    arguments = qStringListFromUtf8Strings({"-of"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
//     REQUIRE( commandLine.argumentCount() == 2 );
//     REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
//     REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'o','f'} );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-o=keep")
  {
    arguments = qStringListFromUtf8Strings({"-o=keep"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o=")
  {
    arguments = qStringListFromUtf8Strings({"-o="});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
//     REQUIRE( commandLine.argumentCount() == 2 );
//     REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
//     REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("-o=1")
  {
    arguments = qStringListFromUtf8Strings({"-o=1"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("1") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo=keep")
  {
    arguments = qStringListFromUtf8Strings({"-fo=keep"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isShortOptionListWithLastHavingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o=key=value")
  {
    arguments = qStringListFromUtf8Strings({"-o=key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("key=value") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-o key=value")
  {
    arguments = qStringListFromUtf8Strings({"-o","key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("key=value") );
    REQUIRE( current == (arguments.cbegin()+1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo=key=value")
  {
    arguments = qStringListFromUtf8Strings({"-fo=key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isShortOptionListWithLastHavingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("key=value") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("-fo key=value")
  {
    arguments = qStringListFromUtf8Strings({"-fo","key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','o'} );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("key=value") );
    REQUIRE( current == (arguments.cbegin()+1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("-u (unknown option)")
  {
    arguments = qStringListFromUtf8Strings({"-u"});
    auto current = arguments.cbegin();
    REQUIRE( !addShortOptionsToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("u") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }
}

TEST_CASE("addLongOptionToCommandLine")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );

  ParserDefinitionOption overwriteBehaviorOption( 'o', QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand command;
  command.addHelpOption();
  command.addOption(forceOption);
  command.addOption(overwriteBehaviorOption);

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("--help")
  {
    arguments = qStringListFromUtf8Strings({"--help"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("help") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("--unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"--unknown-option"});
    auto current = arguments.cbegin();
    REQUIRE( !addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("unknown-option") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("--overwrite-behavior keep")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior","keep"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
    REQUIRE( current == (arguments.cbegin() + 1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("--overwrite-behavior")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior"});
    auto current = arguments.cbegin();
    REQUIRE( !addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("--overwrite-behavior=keep")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior=keep"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("--overwrite-behavior=")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior="});
    auto current = arguments.cbegin();
    REQUIRE( !addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }

  SECTION("--overwrite-behavior=key=value")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior=key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("key=value") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("--overwrite-behavior key=value")
  {
    arguments = qStringListFromUtf8Strings({"--overwrite-behavior","key=value"});
    auto current = arguments.cbegin();
    REQUIRE( addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("key=value") );
    REQUIRE( current == (arguments.cbegin() + 1) );
    REQUIRE( !error.hasError() );
  }

  SECTION("--p")
  {
    arguments = qStringListFromUtf8Strings({"--p"});
    auto current = arguments.cbegin();
    REQUIRE( !addLongOptionToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( error.hasError() );
  }
}

TEST_CASE("addOptionOrDashToCommandLine")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionCommand command;
  command.addHelpOption();

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("app -h")
  {
    arguments = qStringListFromUtf8Strings({"-h"});
    auto current = arguments.cbegin();
    REQUIRE( addOptionOrDashToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("app --help")
  {
    arguments = qStringListFromUtf8Strings({"--help"});
    auto current = arguments.cbegin();
    REQUIRE( addOptionOrDashToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("help") );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }

  SECTION("app -")
  {
    arguments = qStringListFromUtf8Strings({"-"});
    auto current = arguments.cbegin();
    REQUIRE( addOptionOrDashToCommandLine(current, arguments.cend(), command, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isSingleDash( commandLine.argumentAt(1) ) );
    REQUIRE( current == arguments.cbegin() );
    REQUIRE( !error.hasError() );
  }
}

TEST_CASE("parseAsPositionalArgument")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("-h file.txt - -- --option")
  {
    arguments = qStringListFromUtf8Strings({"-h","file.txt","-","--","--option"});
    parseAsPositionalArgument( arguments.cbegin(), arguments.cend(), commandLine );
    REQUIRE( commandLine.argumentCount() == 6 );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(1) ) == QLatin1String("-h") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(2) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(2) ) == QLatin1String("file.txt") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(3) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(3) ) == QLatin1String("-") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(4) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(4) ) == QLatin1String("--") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(5) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(5) ) == QLatin1String("--option") );
  }
}

TEST_CASE("parseArgument")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption overwriteBehaviorOption( 'o', QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinitionCommand command;
  command.addHelpOption();
  command.addOption(overwriteBehaviorOption);

  commandLine.setExecutableName( QLatin1String("app") );

  SECTION("-h")
  {
    arguments = qStringListFromUtf8Strings({"-h"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgument( first, arguments.cend(), command, commandLine, error ) );
    REQUIRE( first == arguments.cbegin() );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
  }

  SECTION("--")
  {
    arguments = qStringListFromUtf8Strings({"--"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgument( first, arguments.cend(), command, commandLine, error ) );
    REQUIRE( first == arguments.cend() );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isDoubleDash( commandLine.argumentAt(1) ) );
  }

  SECTION("-- -h")
  {
    arguments = qStringListFromUtf8Strings({"--","-h"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgument( first, arguments.cend(), command, commandLine, error ) );
    REQUIRE( first == arguments.cend() );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isDoubleDash( commandLine.argumentAt(1) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(2) ) == QLatin1String("-h") );
  }

  SECTION("file.txt")
  {
    arguments = qStringListFromUtf8Strings({"file.txt"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgument( first, arguments.cend(), command, commandLine, error ) );
    REQUIRE( first == arguments.cbegin() );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(1) ) == QLatin1String("file.txt") );
  }

  SECTION("-o keep")
  {
    arguments = qStringListFromUtf8Strings({"-o","keep"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgument( first, arguments.cend(), command, commandLine, error ) );
    REQUIRE( first == arguments.cbegin()+1 );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
  }

  SECTION("-o -")
  {
    arguments = qStringListFromUtf8Strings({"-o","-"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgument( first, arguments.cend(), command, commandLine, error ) );
    REQUIRE( first == arguments.cbegin()+1 );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("-") );
  }
}

TEST_CASE("parseArgumentsUntilSubCommandName")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addOption(forceOption);

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addSubCommand(copyCommand);

  commandLine.setExecutableName( QLatin1String("app") );

  const ParserDefinitionCommand *command = nullptr;

  SECTION("-h")
  {
    arguments = qStringListFromUtf8Strings({"-h"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cend() );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( command == nullptr );
  }

  SECTION("file.txt")
  {
    arguments = qStringListFromUtf8Strings({"file.txt"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cend() );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(1) ) == QLatin1String("file.txt") );
    REQUIRE( command == nullptr );
  }

  SECTION("copy")
  {
    arguments = qStringListFromUtf8Strings({"copy"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cbegin() );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( getSubCommandName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( command != nullptr );
    REQUIRE( command->name() == QLatin1String("copy") );
  }

  SECTION("-h copy")
  {
    arguments = qStringListFromUtf8Strings({"-h","copy"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cbegin()+1 );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( getSubCommandName( commandLine.argumentAt(2) ) == QLatin1String("copy") );
    REQUIRE( command != nullptr );
    REQUIRE( command->name() == QLatin1String("copy") );
  }

  SECTION("-h copy -f")
  {
    arguments = qStringListFromUtf8Strings({"-h","copy","-f"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cbegin()+1 );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( getSubCommandName( commandLine.argumentAt(2) ) == QLatin1String("copy") );
    REQUIRE( command != nullptr );
    REQUIRE( command->name() == QLatin1String("copy") );
  }

  SECTION("--")
  {
    arguments = qStringListFromUtf8Strings({"--"});
    auto first = arguments.cbegin();
    REQUIRE( parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cend() );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isDoubleDash( commandLine.argumentAt(1) ) );
    REQUIRE( command == nullptr );
  }

  SECTION("--unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"--unknown-option"});
    auto first = arguments.cbegin();
    REQUIRE( !parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cend() );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("unknown-option") );
    REQUIRE( command == nullptr );
  }

  SECTION("-u --unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"-u","--unknown-option"});
    auto first = arguments.cbegin();
    REQUIRE( !parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cend() );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("u") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(2) ) == QLatin1String("unknown-option") );
    REQUIRE( command == nullptr );
  }

  SECTION("-u --")
  {
    arguments = qStringListFromUtf8Strings({"-u","--"});
    auto first = arguments.cbegin();
    REQUIRE( !parseArgumentsUntilSubCommandName( first, arguments.cend(), parserDefinition, &command, commandLine, error ) );
    REQUIRE( first == arguments.cend() );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("u") );
    REQUIRE( isDoubleDash( commandLine.argumentAt(2) ) );
    REQUIRE( command == nullptr );
  }
}

TEST_CASE("parse")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );

  ParserDefinitionOption overwriteBehaviorOption( 'o', QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption(forceOption);
  parserDefinition.addOption(overwriteBehaviorOption);

  SECTION("Empty")
  {
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.isEmpty() );
  }

  SECTION("app")
  {
    arguments = qStringListFromUtf8Strings({"app"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
  }

  SECTION("app file.txt")
  {
    arguments = qStringListFromUtf8Strings({"app","file.txt"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(1) ) == QLatin1String("file.txt") );
  }

  SECTION("app -h")
  {
    arguments = qStringListFromUtf8Strings({"app","-h"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
  }

  SECTION("app --help")
  {
    arguments = qStringListFromUtf8Strings({"app","--help"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("help") );
  }

  SECTION("app --unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"app","--unknown-option"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("unknown-option") );
    REQUIRE( error.hasError() );
  }

  SECTION("app -u")
  {
    arguments = qStringListFromUtf8Strings({"app","-u"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("u") );
    REQUIRE( error.hasError() );
  }

  SECTION("app -u --unknown-option")
  {
    arguments = qStringListFromUtf8Strings({"app","-u","--unknown-option"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("u") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(2) ) == QLatin1String("unknown-option") );
    REQUIRE( error.hasError() );
  }

  SECTION("app -")
  {
    arguments = qStringListFromUtf8Strings({"app","-"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isSingleDash(  commandLine.argumentAt(1)  ) );
  }

  SECTION("app --")
  {
    arguments = qStringListFromUtf8Strings({"app","--"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( isDoubleDash(  commandLine.argumentAt(1)  ) );
  }

  SECTION("app -fh")
  {
    arguments = qStringListFromUtf8Strings({"app","-fh"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isShortOptionList( commandLine.argumentAt(1) ) );
    REQUIRE( getShortOptionListNames( commandLine.argumentAt(1) ) == std::vector<char>{'f','h'} );
  }

  SECTION("app -o keep")
  {
    arguments = qStringListFromUtf8Strings({"app","-o","keep"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOptionExpectingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
  }

  SECTION("app -o=keep")
  {
    arguments = qStringListFromUtf8Strings({"app","-o=keep"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("o") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
  }

  SECTION("app --overwrite-behavior keep")
  {
    arguments = qStringListFromUtf8Strings({"app","--overwrite-behavior","keep"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOptionExpectingValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( isOptionValue( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionValue( commandLine.argumentAt(2) ) == QLatin1String("keep") );
  }

  SECTION("app --overwrite-behavior=keep")
  {
    arguments = qStringListFromUtf8Strings({"app","--overwrite-behavior=keep"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOptionWithValue( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("overwrite-behavior") );
    REQUIRE( getOptionValue( commandLine.argumentAt(1) ) == QLatin1String("keep") );
  }

  SECTION("app -- --unknown-option -h (Must be added as a positional argument)")
  {
    arguments = qStringListFromUtf8Strings({"app","--","--unknown-option","-h"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 4 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isDoubleDash( commandLine.argumentAt(1) ) );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(2) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(2) ) == QLatin1String("--unknown-option") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(3) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(3) ) == QLatin1String("-h") );
    REQUIRE( !error.hasError() );
  }
}

TEST_CASE("parse_AppWithSubCommand")
{
  QStringList arguments;
  CommandLine::CommandLine commandLine;
  ParseError error;

  ParserDefinitionOption forceOption( 'f', QLatin1String("force"), QLatin1String("Force") );
  ParserDefinitionOption copyOption( QLatin1String("copy"), QLatin1String("Copy") );

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.addOption(forceOption);

  ParserDefinition parserDefinition;
  parserDefinition.addHelpOption();
  parserDefinition.addOption(copyOption);
  parserDefinition.addSubCommand(copyCommand);

  SECTION("Empty")
  {
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.isEmpty() );
    REQUIRE( error.hasError() );
  }

  SECTION("app")
  {
    arguments = qStringListFromUtf8Strings({"app"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app -h")
  {
    arguments = qStringListFromUtf8Strings({"app","-h"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("h") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app -f")
  {
    arguments = qStringListFromUtf8Strings({"app","-f"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( error.hasError() );
  }

  SECTION("app copy -f")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","-f"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( isOption( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(2) ) == QLatin1String("f") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app copy")
  {
    arguments = qStringListFromUtf8Strings({"app","copy"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 2 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app copy -h")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","-h"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( error.hasError() );
  }

  SECTION("app copy -h --")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","-h","--"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( error.hasError() );
  }

  SECTION("app copy copy")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","copy"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( isPositionalArgument( commandLine.argumentAt(2) ) );
    REQUIRE( getPositionalArgumentValue( commandLine.argumentAt(2) ) == QLatin1String("copy") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app copy -")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","-"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( isSingleDash( commandLine.argumentAt(2) ) );
    REQUIRE( !error.hasError() );
  }

  SECTION("app copy --")
  {
    arguments = qStringListFromUtf8Strings({"app","copy","--"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(1) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( isDoubleDash( commandLine.argumentAt(2) ) );
    REQUIRE( !error.hasError() );
  }

  SECTION("app --copy copy")
  {
    arguments = qStringListFromUtf8Strings({"app","--copy","copy"});
    REQUIRE( parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 3 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("copy") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(2) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(2) ) == QLatin1String("copy") );
    REQUIRE( !error.hasError() );
  }

  SECTION("app -u -h --unknown-option copy --unknown-option -f -u")
  {
    arguments = qStringListFromUtf8Strings({"app","-u","-h","--unknown-option","copy","--unknown-option","-f","-u"});
    REQUIRE( !parse(arguments, parserDefinition, commandLine, error) );
    REQUIRE( commandLine.argumentCount() == 8 );
    REQUIRE( commandLine.executableName() == QLatin1String("app") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(1) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(1) ) == QLatin1String("u") );
    REQUIRE( isOption( commandLine.argumentAt(2) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(2) ) == QLatin1String("h") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(3) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(3) ) == QLatin1String("unknown-option") );
    REQUIRE( isSubCommandNameArgument( commandLine.argumentAt(4) ) );
    REQUIRE( getSubCommandName( commandLine.argumentAt(4) ) == QLatin1String("copy") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(5) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(5) ) == QLatin1String("unknown-option") );
    REQUIRE( isOption( commandLine.argumentAt(6) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(6) ) == QLatin1String("f") );
    REQUIRE( isUnknownOption( commandLine.argumentAt(7) ) );
    REQUIRE( getOptionName( commandLine.argumentAt(7) ) == QLatin1String("u") );
    REQUIRE( error.hasError() );
  }
}
