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
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/Impl/ParserDefinition.h"
#include "Mdt/CommandLineArguments.h"
#include <QString>
#include <QLatin1String>
#include <vector>

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

  SECTION("value name")
  {
    ParserDefinitionOption option( QLatin1String("destination"), QLatin1String("Destination directory") );
    REQUIRE( !option.hasValueName() );
    REQUIRE( option.valueName().isEmpty() );
    option.setValueName( QLatin1String("directory") );
    REQUIRE( option.hasValueName() );
    REQUIRE( option.valueName() == QLatin1String("directory") );
  }

  SECTION("possible vales")
  {
    ParserDefinitionOption option( QLatin1String("overwrite-behavior"), QLatin1String("Overwrite behavior") );
    option.setValueName( QLatin1String("behavior") );
    REQUIRE( !option.hasPossibleValues() );
    option.setPossibleValues({QLatin1String("keep"),QLatin1String("fail"),QLatin1String("overwrite")});
    REQUIRE( option.hasPossibleValues() );
    REQUIRE( option.possibleValues() == QStringList({QLatin1String("keep"),QLatin1String("fail"),QLatin1String("overwrite")}) );
  }

  SECTION("default value")
  {
    ParserDefinitionOption option( QLatin1String("destination"), QLatin1String("Destination directory") );
    option.setValueName( QLatin1String("directory") );
    option.setDefaultValue( QLatin1String("/tmp") );
    REQUIRE( option.defaultValues() == QStringList{QLatin1String("/tmp")} );
  }

  SECTION("default values")
  {
    ParserDefinitionOption option( QLatin1String("destination"), QLatin1String("Destination directory") );
    option.setValueName( QLatin1String("directory") );
    option.setDefaultValues( QStringList{QLatin1String("/tmp"),QLatin1String("/usr/lib")} );
    REQUIRE( option.defaultValues() == QStringList{QLatin1String("/tmp"),QLatin1String("/usr/lib")} );
  }

  SECTION("valueType")
  {
    ParserDefinitionOption option( QLatin1String("destination"), QLatin1String("Destination directory") );
    REQUIRE( option.valueType() == ValueType::Unspecified );
    option.setValueType(ValueType::Directory);
    REQUIRE( option.valueType() == ValueType::Directory );
  }
}

TEST_CASE("ParserDefinitionPositionalArgument")
{
  SECTION("With syntax")
  {
    ParserDefinitionPositionalArgument argument( ValueType::Unspecified, QLatin1String("source"), QLatin1String("Source file"), QLatin1String("[path]") );
    REQUIRE( argument.valueType() == ValueType::Unspecified );
    REQUIRE( argument.name() == QLatin1String("source") );
    REQUIRE( argument.description() == QLatin1String("Source file") );
    REQUIRE( argument.hasSyntax() );
    REQUIRE( argument.syntax() == QLatin1String("[path]") );
  }

  SECTION("With spaces")
  {
    ParserDefinitionPositionalArgument argument( ValueType::Unspecified, QLatin1String(" source"), QLatin1String(" Source file"), QLatin1String(" [path]") );
    REQUIRE( argument.valueType() == ValueType::Unspecified );
    REQUIRE( argument.name() == QLatin1String("source") );
    REQUIRE( argument.description() == QLatin1String("Source file") );
    REQUIRE( argument.hasSyntax() );
    REQUIRE( argument.syntax() == QLatin1String("[path]") );
  }

  SECTION("No syntax")
  {
    ParserDefinitionPositionalArgument argument( ValueType::Unspecified, QLatin1String("source"), QLatin1String("Source file") );
    REQUIRE( argument.valueType() == ValueType::Unspecified );
    REQUIRE( argument.name() == QLatin1String("source") );
    REQUIRE( argument.description() == QLatin1String("Source file") );
    REQUIRE( !argument.hasSyntax() );
  }

  SECTION("Directory type")
  {
    ParserDefinitionPositionalArgument argument( ValueType::Directory, QLatin1String("source"), QLatin1String("Source file") );
    REQUIRE( argument.valueType() == ValueType::Directory );
    REQUIRE( argument.name() == QLatin1String("source") );
    REQUIRE( argument.description() == QLatin1String("Source file") );
  }

  SECTION("possible vales")
  {
    ParserDefinitionPositionalArgument argument( ValueType::Directory, QLatin1String("file-type"), QLatin1String("File type") );
    REQUIRE( !argument.hasPossibleValues() );
    argument.setPossibleValues({QLatin1String("executable"),QLatin1String("library"),QLatin1String("config")});
    REQUIRE( argument.hasPossibleValues() );
    REQUIRE( argument.possibleValues() == QStringList({QLatin1String("executable"),QLatin1String("library"),QLatin1String("config")}) );
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

TEST_CASE("ParserDefinition_applicationName")
{
  Mdt::CommandLineArguments arguments({"default-app-name"});
  QCoreApplication app( arguments.argumentCountRef(), arguments.argumentVector() );
  /*
   * Make sure we have exactly the app name we expect,
   * and not a variant (like .\default-app-name)
   */
  app.setApplicationName( QLatin1String("default-app-name") );
  REQUIRE( app.applicationName() == QLatin1String("default-app-name") );

  ParserDefinition parser;

  SECTION("default")
  {
    REQUIRE( parser.applicationName() == QLatin1String("default-app-name") );
  }

  SECTION("User defined")
  {
    parser.setApplicationName( QLatin1String("my-app") );
    REQUIRE( parser.applicationName() == QLatin1String("my-app") );
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

  SECTION("name")
  {
    parser.addPositionalArgument( QLatin1String("name"), QLatin1String("Name of the branch"), QLatin1String("[name]") );
    REQUIRE( parser.hasPositionalArguments() );
    REQUIRE( parser.mainCommand().positionalArguments()[0].valueType() == ValueType::Unspecified );
    REQUIRE( parser.mainCommand().positionalArguments()[0].name() == QLatin1String("name") );
    REQUIRE( parser.mainCommand().positionalArguments()[0].description() == QLatin1String("Name of the branch") );
    REQUIRE( parser.mainCommand().positionalArguments()[0].syntax() == QLatin1String("[name]") );
  }

  SECTION("source")
  {
    parser.addPositionalArgument( ValueType::File, QLatin1String("source"), QLatin1String("Source file"), QLatin1String("[file]") );
    REQUIRE( parser.hasPositionalArguments() );
    REQUIRE( parser.mainCommand().positionalArguments()[0].valueType() == ValueType::File );
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
    REQUIRE( parser.subCommandCount() == 0 );
  }

  SECTION("copy")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    parser.addSubCommand(copyCommand);
    REQUIRE( parser.hasSubCommands() );
    REQUIRE( parser.subCommandCount() == 1 );
    REQUIRE( parser.subCommands()[0].name() == QLatin1String("copy") );
  }
}

TEST_CASE("ParserDefinition_findSubCommandByName")
{
  ParserDefinition parser;

  SECTION("No sub-command")
  {
    const auto command = parser.findSubCommandByName( QLatin1String("copy") );
    REQUIRE( !command );
  }

  SECTION("copy,list")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    ParserDefinitionCommand listCommand( QLatin1String("list") );
    parser.addSubCommand(copyCommand);
    parser.addSubCommand(listCommand);

    SECTION("rem")
    {
      const auto command = parser.findSubCommandByName( QLatin1String("rem") );
      REQUIRE( !command );
    }

    SECTION("copy")
    {
      const auto command = parser.findSubCommandByName( QLatin1String("copy") );
      REQUIRE( (bool)command );
      REQUIRE( command->name() == QLatin1String("copy") );
    }

    SECTION("list")
    {
      const auto command = parser.findSubCommandByName( QLatin1String("list") );
      REQUIRE( (bool)command );
      REQUIRE( command->name() == QLatin1String("list") );
    }
  }
}

TEST_CASE("ParserDefinition_containsSubCommand")
{
  ParserDefinition parserDefinition;

  SECTION("No sub-command")
  {
    REQUIRE( !parserDefinition.containsSubCommand( QLatin1String("copy") ) );
  }

  SECTION("copy,list")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    ParserDefinitionCommand listCommand( QLatin1String("list") );
    parserDefinition.addSubCommand(copyCommand);
    parserDefinition.addSubCommand(listCommand);

    SECTION("rem")
    {
      REQUIRE( !parserDefinition.containsSubCommand( QLatin1String("rem") ) );
    }

    SECTION("copy")
    {
      REQUIRE( parserDefinition.containsSubCommand( QLatin1String("copy") ) );
    }

    SECTION("list")
    {
      REQUIRE( parserDefinition.containsSubCommand( QLatin1String("list") ) );
    }
  }
}

TEST_CASE("ParserDefinition_SubCommandIndexAccess")
{
  ParserDefinition parserDefinition;

  SECTION("No sub-command")
  {
    REQUIRE( parserDefinition.findSubCommandIndexByName( QLatin1String("copy") ) < 0 );
  }

  SECTION("copy,list")
  {
    ParserDefinitionCommand copyCommand( QLatin1String("copy") );
    ParserDefinitionCommand listCommand( QLatin1String("list") );
    parserDefinition.addSubCommand(copyCommand);
    parserDefinition.addSubCommand(listCommand);

    SECTION("Find rem")
    {
      REQUIRE( parserDefinition.findSubCommandIndexByName( QLatin1String("rem") ) < 0 );
    }

    SECTION("Find copy")
    {
      REQUIRE( parserDefinition.findSubCommandIndexByName( QLatin1String("copy") ) == 0 );
      REQUIRE( parserDefinition.subCommandAt(0).name() == QLatin1String("copy") );
    }

    SECTION("Find list")
    {
      REQUIRE( parserDefinition.findSubCommandIndexByName( QLatin1String("list") ) == 1 );
      REQUIRE( parserDefinition.subCommandAt(1).name() == QLatin1String("list") );
    }
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
    ParserDefinitionPositionalArgument argument( ValueType::File, QLatin1String("source"), QLatin1String("Path to the source file") );
    arguments.push_back(argument);

    const QString expectedText = QLatin1String("Usage: parser-def-test source");
    REQUIRE( getUsageText(applicationName, options, arguments, subCommands) == expectedText );
  }

  SECTION("Arguments with syntax")
  {
    ParserDefinitionPositionalArgument argument( ValueType::File, QLatin1String("source"), QLatin1String("Path to the source file"), QLatin1String("[file]") );
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
    ParserDefinitionPositionalArgument argument( ValueType::Unspecified, QLatin1String("argument-1"), QLatin1String("Argument 1 description") );
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
    REQUIRE( result == expectedText );
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

  SECTION("destination with value")
  {
    ParserDefinitionOption destinationOption( 'd', QLatin1String("destination"), QLatin1String("Destination directory") );
    destinationOption.setValueName( QLatin1String("directory") );
    options.push_back(destinationOption);
    expectedText = QLatin1String("Options:\n"
                                 "  -d, --destination <directory>  Destination directory");
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
    arguments.emplace_back( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    expectedText = QLatin1String("Arguments:\n"
                                 "  source  Source file");
    result = getPositionalArgumentsHelpText(arguments, maxLength);
    REQUIRE( result == expectedText );
  }

  SECTION("source,destination")
  {
    arguments.emplace_back( ValueType::File, QLatin1String("source"), QLatin1String("Source file") );
    arguments.emplace_back( ValueType::Directory, QLatin1String("destination"), QLatin1String("Destination file") );
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
