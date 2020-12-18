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
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/ParserDefinition_Impl.h"
#include <QString>
#include <QLatin1String>
#include <vector>

#include <QDebug>

using namespace Mdt::CommandLineParser;

bool isValidShortName(char name)
{
  return ParserDefinitionOption::isValidShortName(name);
}

bool isValidName(const QString & name)
{
  return ParserDefinitionOption::isValidName(name);
}

QString getUsageText(const QString & applicationName, const std::vector<ParserDefinitionOption> & options,
                     const std::vector<ParserDefinitionPositionalArgument> & arguments,
                     const std::vector<ParserDefinitionCommand> & subCommands)
{
  return ParserDefinition_Impl::getUsageText(applicationName, options, arguments, subCommands);
}

QString getOptionsHelpText(const std::vector<ParserDefinitionOption> & options, int maxLength)
{
  return ParserDefinition_Impl::getOptionsHelpText(options, maxLength);
}

QString getPositionalArgumentsHelpText(const std::vector<ParserDefinitionPositionalArgument> & arguments, int maxLength)
{
  return ParserDefinition_Impl::getPositionalArgumentsHelpText(arguments, maxLength);
}

QString getAvailableSubCommandsHelpText(const std::vector<ParserDefinitionCommand> & subCommands, int maxLength)
{
  return ParserDefinition_Impl::getAvailableSubCommandsHelpText(subCommands, maxLength);
}


TEST_CASE("ParserDefinitionOption_isValidShortName")
{
  SECTION("1")
  {
    REQUIRE( !isValidShortName('1') );
  }

  SECTION("space")
  {
    REQUIRE( !isValidShortName(' ') );
  }

  SECTION("-")
  {
    REQUIRE( !isValidShortName('-') );
  }

  SECTION("a")
  {
    REQUIRE( isValidShortName('a') );
  }

  SECTION("A")
  {
    REQUIRE( isValidShortName('A') );
  }
}

TEST_CASE("ParserDefinitionOption_isValidName")
{
  SECTION("empty")
  {
    REQUIRE( !isValidName( QLatin1String("") ) );
  }

  SECTION("space")
  {
    REQUIRE( !isValidName( QLatin1String(" ") ) );
  }

  SECTION("-")
  {
    REQUIRE( !isValidName( QLatin1String("-") ) );
  }

  SECTION("--")
  {
    REQUIRE( !isValidName( QLatin1String("--") ) );
  }

  SECTION("-a")
  {
    REQUIRE( !isValidName( QLatin1String("-a") ) );
  }

  SECTION("--a")
  {
    REQUIRE( !isValidName( QLatin1String("--a") ) );
  }

  SECTION("a")
  {
    REQUIRE( isValidName( QLatin1String("a") ) );
  }

  SECTION("ab")
  {
    REQUIRE( isValidName( QLatin1String("ab") ) );
  }

  SECTION("a-b")
  {
    REQUIRE( isValidName( QLatin1String("a-b") ) );
  }

  SECTION("/")
  {
    REQUIRE( !isValidName( QLatin1String("/") ) );
  }

  SECTION("a/b")
  {
    REQUIRE( !isValidName( QLatin1String("a/b") ) );
  }

  SECTION("=")
  {
    REQUIRE( !isValidName( QLatin1String("=") ) );
  }

  SECTION("=a")
  {
    REQUIRE( !isValidName( QLatin1String("=a") ) );
  }

  SECTION("a=b")
  {
    REQUIRE( !isValidName( QLatin1String("a=b") ) );
  }
}

TEST_CASE("ParserDefinitionOption")
{
  SECTION("short name, name and description")
  {
    ParserDefinitionOption option('h', QLatin1String("help"), QLatin1String("Help description") );
    REQUIRE( option.hasShortName() );
    REQUIRE( option.shortNameWithDash() == QLatin1String("-h") );
    REQUIRE( option.nameWithDashes() == QLatin1String("--help") );
    REQUIRE( option.description() == QLatin1String("Help description") );
  }

  SECTION("name and description")
  {
    ParserDefinitionOption option( QLatin1String("help"), QLatin1String("Help description") );
    REQUIRE( !option.hasShortName() );
    REQUIRE( option.nameWithDashes() == QLatin1String("--help") );
    REQUIRE( option.description() == QLatin1String("Help description") );
  }
}

TEST_CASE("ParserDefinitionPositionalArgument")
{
  SECTION("With syntax")
  {
    ParserDefinitionPositionalArgument argument( QLatin1String("source"), QLatin1String("Source file"), QLatin1String("[path]") );
    REQUIRE( argument.name() == QLatin1String("source") );
    REQUIRE( argument.description() == QLatin1String("Source file") );
    REQUIRE( argument.hasSyntax() );
    REQUIRE( argument.syntax() == QLatin1String("[path]") );
  }

  SECTION("With spaces")
  {
    ParserDefinitionPositionalArgument argument( QLatin1String(" source"), QLatin1String(" Source file"), QLatin1String(" [path]") );
    REQUIRE( argument.name() == QLatin1String("source") );
    REQUIRE( argument.description() == QLatin1String("Source file") );
    REQUIRE( argument.hasSyntax() );
    REQUIRE( argument.syntax() == QLatin1String("[path]") );
  }

  SECTION("No syntax")
  {
    ParserDefinitionPositionalArgument argument( QLatin1String("source"), QLatin1String("Source file") );
    REQUIRE( argument.name() == QLatin1String("source") );
    REQUIRE( argument.description() == QLatin1String("Source file") );
    REQUIRE( !argument.hasSyntax() );
  }
}

TEST_CASE("ParserDefinitionCommand_name")
{
  SECTION("No name")
  {
    ParserDefinitionCommand command;
    REQUIRE( !command.hasName() );
  }

  SECTION("copy")
  {
    ParserDefinitionCommand command( QLatin1String("copy") );
    REQUIRE( command.hasName() );
    REQUIRE( command.name() == QLatin1String("copy") );
  }
}

TEST_CASE("ParserDefinitionCommand_options")
{
  ParserDefinitionCommand command;

  SECTION("No option")
  {
    REQUIRE( !command.hasOptions() );
  }

  SECTION("force")
  {
    command.addOption( 'f', QLatin1String("force"), QLatin1String("Force command") );
    REQUIRE( command.hasOptions() );
    REQUIRE( command.options()[0].shortNameWithDash() == QLatin1String("-f") );
    REQUIRE( command.options()[0].nameWithDashes() == QLatin1String("--force") );
  }

  SECTION("help")
  {
    command.addHelpOption();
    REQUIRE( command.hasOptions() );
    REQUIRE( command.options()[0].shortNameWithDash() == QLatin1String("-h") );
    REQUIRE( command.options()[0].nameWithDashes() == QLatin1String("--help") );
  }
}

TEST_CASE("ParserDefinitionCommand_positionalArguments")
{
  ParserDefinitionCommand command;

  SECTION("No argument")
  {
    REQUIRE( !command.hasPositionalArguments() );
  }

  SECTION("source")
  {
    command.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file"), QLatin1String("[file]") );
    REQUIRE( command.hasPositionalArguments() );
    REQUIRE( command.positionalArguments()[0].name() == QLatin1String("source") );
    REQUIRE( command.positionalArguments()[0].description() == QLatin1String("Source file") );
    REQUIRE( command.positionalArguments()[0].syntax() == QLatin1String("[file]") );
  }
}

TEST_CASE("ParserDefinition_description")
{
  ParserDefinition parser;

  SECTION("No description")
  {
    REQUIRE( !parser.hasApplicationDescription() );
  }

  SECTION("With description")
  {
    parser.setApplicationDescription( QLatin1String("Application description") );
    REQUIRE( parser.hasApplicationDescription() );
    REQUIRE( parser.applicationDescription() == QLatin1String("Application description") );
  }
}

TEST_CASE("ParserDefinition_options")
{
  ParserDefinition parser;

  SECTION("No option")
  {
    REQUIRE( !parser.hasOptions() );
  }

  SECTION("force")
  {
    parser.addOption( 'f', QLatin1String("force"), QLatin1String("Force command") );
    REQUIRE( parser.hasOptions() );
    REQUIRE( parser.mainCommand().options()[0].shortNameWithDash() == QLatin1String("-f") );
    REQUIRE( parser.mainCommand().options()[0].nameWithDashes() == QLatin1String("--force") );
  }

  SECTION("help")
  {
    parser.addHelpOption();
    REQUIRE( parser.hasOptions() );
    REQUIRE( parser.mainCommand().options()[0].shortNameWithDash() == QLatin1String("-h") );
    REQUIRE( parser.mainCommand().options()[0].nameWithDashes() == QLatin1String("--help") );
  }
}

TEST_CASE("ParserDefinition_positionalArguments")
{
  ParserDefinition parser;

  SECTION("No argument")
  {
    REQUIRE( !parser.hasPositionalArguments() );
  }

  SECTION("source")
  {
    parser.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file"), QLatin1String("[file]") );
    REQUIRE( parser.hasPositionalArguments() );
    REQUIRE( parser.mainCommand().positionalArguments()[0].name() == QLatin1String("source") );
    REQUIRE( parser.mainCommand().positionalArguments()[0].description() == QLatin1String("Source file") );
    REQUIRE( parser.mainCommand().positionalArguments()[0].syntax() == QLatin1String("[file]") );
  }
}

TEST_CASE("ParserDefinition_subCommands")
{
  ParserDefinition parser;

  SECTION("No subCommand")
  {
    REQUIRE( !parser.hasSubCommands() );
  }

  SECTION("copy")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    parser.addSubCommand(copyCommand);
    REQUIRE( parser.hasSubCommands() );
    REQUIRE( parser.subCommands()[0].name() == QLatin1String("copy") );
  }
}


TEST_CASE("getUsageText")
{
  const QString applicationName = QLatin1String("parser-def-test");
  std::vector<ParserDefinitionOption> options;
  std::vector<ParserDefinitionPositionalArgument> arguments;
  std::vector<ParserDefinitionCommand> subCommands;

  SECTION("No arguments and no options")
  {
    const QString expectedText = QLatin1String("Usage: parser-def-test");;
    REQUIRE( getUsageText(applicationName, options, arguments, subCommands) == expectedText );
  }

  SECTION("Arguments")
  {
    ParserDefinitionPositionalArgument argument( QLatin1String("source"), QLatin1String("Path to the source file") );
    arguments.push_back(argument);

    const QString expectedText = QLatin1String("Usage: parser-def-test source");
    REQUIRE( getUsageText(applicationName, options, arguments, subCommands) == expectedText );
  }

  SECTION("Arguments with syntax")
  {
    ParserDefinitionPositionalArgument argument( QLatin1String("source"), QLatin1String("Path to the source file"), QLatin1String("[file]") );
    arguments.push_back(argument);

    const QString expectedText = QLatin1String("Usage: parser-def-test [file]");
    REQUIRE( getUsageText(applicationName, options, arguments, subCommands) == expectedText );
  }

  SECTION("Options")
  {
    ParserDefinitionOption option( QLatin1String("option-1"), QLatin1String("Option 1 description") );
    options.push_back(option);

    const QString expectedText = QLatin1String("Usage: parser-def-test [options]");
    REQUIRE( getUsageText(applicationName, options, arguments, subCommands) == expectedText );
  }

  SECTION("Options and arguments")
  {
    ParserDefinitionOption option( QLatin1String("option-1"), QLatin1String("Option 1 description") );
    options.push_back(option);
    ParserDefinitionPositionalArgument argument( QLatin1String("argument-1"), QLatin1String("Argument 1 description") );
    arguments.push_back(argument);

    const QString expectedText = QLatin1String("Usage: parser-def-test [options] argument-1");
    REQUIRE( getUsageText(applicationName, options, arguments, subCommands) == expectedText );
  }

  SECTION("sub-command")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    subCommands.push_back(copyCommand);

    const QString expectedText = QLatin1String("Usage: parser-def-test command");
    REQUIRE( getUsageText(applicationName, options, arguments, subCommands) == expectedText );
  }

  SECTION("Options and sub-command")
  {
    ParserDefinitionOption option( QLatin1String("option-1"), QLatin1String("Option 1 description") );
    options.push_back(option);
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    subCommands.push_back(copyCommand);

    const QString expectedText = QLatin1String("Usage: parser-def-test [options] command");
    REQUIRE( getUsageText(applicationName, options, arguments, subCommands) == expectedText );
  }
}

TEST_CASE("isBreackableChar")
{
  using Impl::isBreackableChar;

  SECTION("space")
  {
    REQUIRE( isBreackableChar( QLatin1Char(' ') ) );
  }

  SECTION("newline")
  {
    REQUIRE( isBreackableChar( QLatin1Char('\n') ) );
  }

  SECTION("a")
  {
    REQUIRE( !isBreackableChar( QLatin1Char('a') ) );
  }

  SECTION("A")
  {
    REQUIRE( !isBreackableChar( QLatin1Char('A') ) );
  }

  SECTION("1")
  {
    REQUIRE( !isBreackableChar( QLatin1Char('1') ) );
  }
}

TEST_CASE("findBreakPoint")
{
  using Impl::findBreakPoint;

  QString text;

  SECTION("empty,2")
  {
    REQUIRE( findBreakPoint(text.cbegin(), text.cend(), 2) == text.cend() );
  }

  SECTION("A,2")
  {
    text = QLatin1String("A");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend(), 2) == text.cend() );
  }

  SECTION("A,5")
  {
    text = QLatin1String("A");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 5) == text.cend() );
  }

  SECTION("AB,2")
  {
    text = QLatin1String("AB");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 2) == text.cend() );
  }

  SECTION("AB C,2")
  {
    text = QLatin1String("AB C");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 2) == text.cbegin()+2 );
  }

  SECTION("ABC,2")
  {
    text = QLatin1String("ABC");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 2) == text.cbegin()+1 );
  }

  SECTION("AB,5")
  {
    text = QLatin1String("AB");
    REQUIRE( findBreakPoint(text.cbegin(), text.cend() , 5) == text.cend() );
  }

}

TEST_CASE("breakText")
{
  using Impl::breakText;

  int leftPadLength = 5;
  int maxLength = 10;
  QString text;
  QString expectedText;
  QString result;

  SECTION("empty")
  {
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("A,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("A");
    expectedText = text;
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("AB,5,2")
  {
    leftPadLength = 5;
    maxLength = 2;
    text = QLatin1String("AB");
    expectedText = text;
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("AB C,5,2")
  {
    leftPadLength = 5;
    maxLength = 2;
    text = QLatin1String("AB C");
    expectedText = QLatin1String("AB\n     C");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABC,5,2")
  {
    leftPadLength = 5;
    maxLength = 2;
    text = QLatin1String("ABC");
    expectedText = QLatin1String("A-\n     BC");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("A B C D E F G H,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("A B C D E F G H");
    expectedText = QLatin1String("A B C D E\n     F G H");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABC\nDE,lp5,ml3")
  {
    leftPadLength = 5;
    maxLength = 3;
    text = QLatin1String("ABC\nDE");
    expectedText = QLatin1String("ABC\n     DE");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABCD EFG HIJK,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("ABCD EFG HIJK");
    expectedText = QLatin1String("ABCD EFG\n     HIJK");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABCD EFG HIJK,5,7")
  {
    leftPadLength = 5;
    maxLength = 7;
    text = QLatin1String("ABCD EFG HIJK");
    expectedText = QLatin1String("ABCD\n     EFG\n     HIJK");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABCDEFGHIJK,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("ABCDEFGHIJK");
    expectedText = QLatin1String("ABCDEFGHI-\n     JK");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABC DEFG HIJ KLMN")
  {
    leftPadLength = 5;
    maxLength = 3;
    text = QLatin1String("ABC DEFG HIJ KLMN");
    expectedText = QLatin1String("ABC\n     DE-\n     FG\n     HIJ\n     KL-\n     MN");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABCDEFG HIJ KLMNO,lp5,ml3")
  {
    leftPadLength = 5;
    maxLength = 3;
    text = QLatin1String("ABCDEFG HIJ KLMNO");
    expectedText = QLatin1String("AB-\n     CD-\n     EFG\n     HIJ\n     KL-\n     MNO");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("ABC DEFGHIJ KLM,lp5,ml3")
  {
    leftPadLength = 5;
    maxLength = 3;
    text = QLatin1String("ABC DEFGHIJ KLM");
    expectedText = QLatin1String("ABC\n     DE-\n     FG-\n     HIJ\n     KLM");
    result = breakText(text, leftPadLength, maxLength);
    REQUIRE( result == expectedText );
  }
}

TEST_CASE("wrapText")
{
  using Impl::wrapText;

  QString names;
  QString description;
  QString expectedText;
  int longestNamesStringLengt;
  int maxLength;
  QString result;

  SECTION("-h, --help,longest:10,ml:50")
  {
    names = QLatin1String("-h, --help");
    description = QLatin1String("Display this help");
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  -h, --help  Display this help");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( expectedText == result );
  }

  SECTION("-h, --help,longest:15,ml:50")
  {
    names = QLatin1String("-h, --help");
    description = QLatin1String("Display this help");
    longestNamesStringLengt = 15;
    maxLength = 50;

    expectedText = QLatin1String("  -h, --help       Display this help");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( expectedText == result );
  }

  SECTION("-h, --help,longest:10,ml:25")
  {
    names = QLatin1String("-h, --help");
    description = QLatin1String("Display this help");
    longestNamesStringLengt = 10;
    maxLength = 25;

    expectedText = QLatin1String("  -h, --help  Display\n"
                                 "              this help");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( expectedText == result );
  }

  SECTION("-h, --help,no description,longest:10,ml:50")
  {
    names = QLatin1String("-h, --help");
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  -h, --help");
    result = wrapText(names, longestNamesStringLengt, description, maxLength);
    REQUIRE( expectedText == result );
  }
}

TEST_CASE("wrapText_option")
{
  using Impl::wrapText;

  QString expectedText;
  int longestNamesStringLengt;
  int maxLength;
  QString result;

  SECTION("--help,longest:10,ml:50")
  {
    ParserDefinitionOption helpOption( QLatin1String("help"), QLatin1String("Display this help") );
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  --help      Display this help");
    result = wrapText(helpOption, longestNamesStringLengt, maxLength);
    REQUIRE( expectedText == result );
  }

  SECTION("-h, --help,longest:10,ml:50")
  {
    ParserDefinitionOption helpOption( 'h', QLatin1String("help"), QLatin1String("Display this help") );
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  -h, --help  Display this help");
    result = wrapText(helpOption, longestNamesStringLengt, maxLength);
    REQUIRE( expectedText == result );
  }
}

TEST_CASE("wrapText_argument")
{
  using Impl::wrapText;

  QString expectedText;
  int longestNamesStringLengt;
  int maxLength;
  QString result;

  SECTION("source,longest:10,ml:50")
  {
    ParserDefinitionPositionalArgument sourceArgument( QLatin1String("source"), QLatin1String("Source file") );
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  source      Source file");
    result = wrapText(sourceArgument, longestNamesStringLengt, maxLength);
    REQUIRE( expectedText == result );
  }
}

TEST_CASE("wrapText_command")
{
  using Impl::wrapText;

  QString expectedText;
  int longestNamesStringLengt;
  int maxLength;
  QString result;

  SECTION("copy:10,ml:50")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.setDescription( QLatin1String("Copy a file") );
    longestNamesStringLengt = 10;
    maxLength = 50;

    expectedText = QLatin1String("  copy        Copy a file");
    result = wrapText(copyCommand, longestNamesStringLengt, maxLength);
    REQUIRE( expectedText == result );
  }
}

TEST_CASE("findLongestOptionNamesStringLength")
{
  using Impl::findLongestOptionNamesStringLength;

  std::vector<ParserDefinitionOption> options;

  SECTION("--help")
  {
    options.emplace_back( QLatin1String("help"), QLatin1String("Display this help") );
    REQUIRE( findLongestOptionNamesStringLength(options) == 6 );
  }

  SECTION("-h, --help")
  {
    options.emplace_back( 'h', QLatin1String("help"), QLatin1String("Display this help") );
    REQUIRE( findLongestOptionNamesStringLength(options) == 10 );
  }

  SECTION("--help,--verbose")
  {
    options.emplace_back( QLatin1String("help"), QLatin1String("Display this help") );
    options.emplace_back( QLatin1String("verbose"), QLatin1String("Verbose") );
    REQUIRE( findLongestOptionNamesStringLength(options) == 9 );
  }
}

TEST_CASE("findLongestArgumentNamesStringLength")
{
  using Impl::findLongestArgumentNamesStringLength;

  std::vector<ParserDefinitionPositionalArgument> arguments;

  SECTION("source")
  {
    arguments.emplace_back( QLatin1String("source"), QLatin1String("Source description") );
    REQUIRE( findLongestArgumentNamesStringLength(arguments) == 6 );
  }

  SECTION("source,destination")
  {
    arguments.emplace_back( QLatin1String("source"), QLatin1String("Source description") );
    arguments.emplace_back( QLatin1String("destination"), QLatin1String("Destination description") );
    REQUIRE( findLongestArgumentNamesStringLength(arguments) == 11 );
  }
}

TEST_CASE("findLongestCommandNameStringLength")
{
  using Impl::findLongestCommandNameStringLength;

  std::vector<ParserDefinitionCommand> commands;

  SECTION("copy")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.setDescription( QLatin1String("Copy a file") );
    commands.push_back(copyCommand);

    REQUIRE( findLongestCommandNameStringLength(commands) == 4 );
  }

  SECTION("add,checkout")
  {
    ParserDefinitionCommand addCommand( QLatin1String("add") );
    addCommand.setDescription( QLatin1String("Add contents to the index") );
    commands.push_back(addCommand);

    ParserDefinitionCommand checkoutCommand( QLatin1String("checkout") );
    checkoutCommand.setDescription( QLatin1String("Switch to a branch") );
    commands.push_back(checkoutCommand);

    REQUIRE( findLongestCommandNameStringLength(commands) == 8 );
  }
}

TEST_CASE("getOptionsHelpText")
{
  std::vector<ParserDefinitionOption> options;
  QString expectedText;
  int maxLength = 80;
  QString result;

  SECTION("help")
  {
    options.emplace_back( 'h', QLatin1String("help"), QLatin1String("Display this help") );
    expectedText = QLatin1String("Options:\n"
                                 "  -h, --help  Display this help");
    result = getOptionsHelpText(options, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("help,long-option")
  {
    options.emplace_back( 'h', QLatin1String("help"), QLatin1String("Display this help") );
    options.emplace_back( 'l', QLatin1String("long-option"), QLatin1String("The long option") );
    expectedText = QLatin1String("Options:\n"
                                 "  -h, --help         Display this help\n"
                                 "  -l, --long-option  The long option");
    result = getOptionsHelpText(options, maxLength);
    REQUIRE( result == expectedText );
  }
}

TEST_CASE("getPositionalArgumentsHelpText")
{
  std::vector<ParserDefinitionPositionalArgument> arguments;
  QString expectedText;
  int maxLength = 80;
  QString result;

  SECTION("source")
  {
    arguments.emplace_back( QLatin1String("source"), QLatin1String("Source file") );
    expectedText = QLatin1String("Arguments:\n"
                                 "  source  Source file");
    result = getPositionalArgumentsHelpText(arguments, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("source,destination")
  {
    arguments.emplace_back( QLatin1String("source"), QLatin1String("Source file") );
    arguments.emplace_back( QLatin1String("destination"), QLatin1String("Destination file") );
    expectedText = QLatin1String("Arguments:\n"
                                 "  source       Source file\n"
                                 "  destination  Destination file");
    result = getPositionalArgumentsHelpText(arguments, maxLength);
    REQUIRE( result == expectedText );
  }
}

TEST_CASE("getAvailableSubCommandsHelpText")
{
  std::vector<ParserDefinitionCommand> subCommands;
  QString expectedText;
  int maxLength = 80;
  QString result;

  SECTION("copy")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    copyCommand.setDescription( QLatin1String("Copy a file") );
    subCommands.push_back(copyCommand);

    expectedText = QLatin1String("Commands:\n"
                                 "  copy  Copy a file");
    result = getAvailableSubCommandsHelpText(subCommands, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("add,checkout")
  {
    ParserDefinitionCommand addCommand( QLatin1String("add") );
    addCommand.setDescription( QLatin1String("Add contents to the index") );
    subCommands.push_back(addCommand);

    ParserDefinitionCommand checkoutCommand( QLatin1String("checkout") );
    checkoutCommand.setDescription( QLatin1String("Switch to a branch") );
    subCommands.push_back(checkoutCommand);

    expectedText = QLatin1String("Commands:\n"
                                 "  add       Add contents to the index\n"
                                 "  checkout  Switch to a branch");
    result = getAvailableSubCommandsHelpText(subCommands, maxLength);
    REQUIRE( result == expectedText );
  }
}
