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

void breackText(QString & text, int leftPadLength, int maxLength)
{
  ParserDefinition_Impl::breackText(text, leftPadLength, maxLength);
}

QString wrapText(const QString & names, int longestName, const QString & description)
{
  return ParserDefinition_Impl::wrapText(names, longestName, description);
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

TEST_CASE("breackText")
{
  int leftPadLength = 5;
  int maxLength = 10;
  QString text;
  QString expectedText;

  SECTION("empty")
  {
    breackText(text, leftPadLength, maxLength);
    REQUIRE( text == expectedText );
  }

  SECTION("A,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("A");
    expectedText = text;
    breackText(text, leftPadLength, maxLength);
    REQUIRE( text == expectedText );
  }

  SECTION("A B C D E F G H,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("A B C D E F G H");
    expectedText = QLatin1String("A B C D E\n     F G H");
    breackText(text, leftPadLength, maxLength);
    
    qDebug() << "Breacked text: " << text;
    
    REQUIRE( text == expectedText );
  }

  SECTION("ABCD EFG HIJK,5,10")
  {
    leftPadLength = 5;
    maxLength = 10;
    text = QLatin1String("ABCD EFG HIJK");
    expectedText = QLatin1String("ABCD EFG\n     HIJK");
    breackText(text, leftPadLength, maxLength);
    
    qDebug() << "Breacked text: " << text;
    
    REQUIRE( text == expectedText );
  }

}

TEST_CASE("wrapText")
{
  QString names;
  QString description;
  QString expectedText;

  names = QLatin1String("-h, --help");
  description = QLatin1String("Display this help");
  expectedText = QLatin1String("  -h, --help  Display this help");
  REQUIRE( wrapText(names, names.length(), description) == expectedText );

  expectedText = QLatin1String("  -h, --help     Display this help");
  REQUIRE( wrapText(names, names.length()+3, description) == expectedText );

  REQUIRE( false );
}

TEST_CASE("wrapText_option")
{
  REQUIRE( false );
}

TEST_CASE("wrapText_argument")
{
  REQUIRE( false );
}

TEST_CASE("getOptionsHelpText")
{
  REQUIRE( false );
}

TEST_CASE("getPositionalArgumentsHelpText")
{
  REQUIRE( false );
}

TEST_CASE("getAvailableSubCommandsHelpText")
{
  REQUIRE( false );
}
