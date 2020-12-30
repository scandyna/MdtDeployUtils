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
#include "Mdt/CommandLineParser/BashCompletionGenerator.h"
#include "Mdt/CommandLineParser/BashCompletionGenerator_Impl.h"
#include <QCommandLineParser>
#include <QTemporaryDir>
#include <QDir>
#include <QString>
#include <QLatin1String>
#include <QByteArray>
#include <QFileInfo>

#include <iostream>

using namespace Mdt::CommandLineParser;

bool fileExists(const QTemporaryDir & dir, const QString & fileName)
{
  const QString filePath = QDir::cleanPath(dir.path() + QLatin1String("/") + fileName);

  return QFileInfo::exists(filePath);
}

TEST_CASE("option")
{
  SECTION("short name and name")
  {
    BashCompletionGeneratorOption option( 'h', QLatin1String("help") );
    REQUIRE( option.hasShortName() );
    REQUIRE( option.shortNameWithDash() == QLatin1String("-h") );
    REQUIRE( option.nameWithDashes() == QLatin1String("--help") );
  }

  SECTION("name only")
  {
    BashCompletionGeneratorOption option( QLatin1String("help") );
    REQUIRE( !option.hasShortName() );
    REQUIRE( option.nameWithDashes() == QLatin1String("--help") );
  }
}

TEST_CASE("Argument")
{
  BashCompletionGeneratorPositionalArgument argument( ArgumentType::Directory, QLatin1String("destination") );
  REQUIRE( argument.type() == ArgumentType::Directory );
  REQUIRE( argument.name() == QLatin1String("destination") );
}

TEST_CASE("Command_arguments")
{
  BashCompletionGeneratorCommand command( QLatin1String("command") );
  command.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
  REQUIRE( command.arguments().size() == 1 );
  REQUIRE( command.arguments()[0].type() == ArgumentType::File );
  REQUIRE( command.arguments()[0].name() == QLatin1String("source") );
}

TEST_CASE("Command_isEmpty")
{
  BashCompletionGeneratorCommand command( QLatin1String("command") );
  REQUIRE( command.isEmpty() );

  SECTION("arguments")
  {
    command.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("arg-a") );
    REQUIRE( !command.isEmpty() );
  }

  SECTION("options")
  {
    command.addOption( 'h', QLatin1String("help") );
    REQUIRE( !command.isEmpty() );
  }

  SECTION("arguments and options")
  {
    command.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("arg-a") );
    command.addOption( 'h', QLatin1String("help") );
    REQUIRE( !command.isEmpty() );
  }
}

TEST_CASE("ApplicationName")
{
  BashCompletionGenerator generator;
  REQUIRE( generator.applicationName().isEmpty() );

  generator.setApplicationName( QLatin1String("mytool") );
  REQUIRE( generator.applicationName() == QLatin1String("mytool") );
}

// TEST_CASE("commandFromQCommandLineParser")
// {
//   QCommandLineParser parser;
//   parser.addHelpOption();
//   parser.addPositionalArgument( QLatin1String("source"), QLatin1String("source description") );
//   parser.addPositionalArgument( QLatin1String("destination"), QLatin1String("destination description") );
//   REQUIRE(  );
// 
//   SECTION("main command")
//   {
//     const auto command = BashCompletionGeneratorCommand::mainCommandFromParser(parser);
//     REQUIRE( command.arguments().size() == 2 );
//     REQUIRE( command.arguments()[0] == "source" );
//     REQUIRE( command.arguments()[1] == "destination" );
//     REQUIRE( command.options().size() == 1 );
//     REQUIRE( command.options()[0].hasShortName() );
//     REQUIRE( command.options()[0].shortNameWithDash() == "-h" );
//     REQUIRE( command.options()[0].nameWithDashes() == "--help" );
//   }
// 
//   SECTION("subCommand")
//   {
//     const auto command = BashCompletionGeneratorCommand::subCommandFromParser(parser, "copy");
//     REQUIRE( command.name() == "copy" );
//     REQUIRE( command.arguments().size() == 2 );
//     REQUIRE( command.arguments()[0] == "source" );
//     REQUIRE( command.arguments()[1] == "destination" );
//     REQUIRE( command.options().size() == 1 );
//     REQUIRE( command.options()[0].hasShortName() );
//     REQUIRE( command.options()[0].shortNameWithDash() == "-h" );
//     REQUIRE( command.options()[0].nameWithDashes() == "--help" );
//   }
// 
// }

TEST_CASE("generateScriptToFile")
{
  QTemporaryDir dir;
  REQUIRE( dir.isValid() );

  BashCompletionGenerator generator;
  generator.setApplicationName( QLatin1String("mytool") );

  BashCompletionGeneratorCommand mainCommand;
  mainCommand.addPositionalArgument( ArgumentType::File, QLatin1String("repo") );
  mainCommand.addOption( 'h', QLatin1String("help") );
  generator.setMainCommand(mainCommand);

  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  copyCommand.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
  copyCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination") );
  
  copyCommand.setDirectoryCompletionEnabled(true);
  
  copyCommand.addOption( 'h', QLatin1String("help") );
  copyCommand.addOption( 'i', QLatin1String("interactive") );
  generator.addSubCommand(copyCommand);

  std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;

  const QString fileName = QLatin1String("mytool-completion.bash");
  REQUIRE( !fileExists(dir, fileName) );
  generator.generateScriptToFile( dir.path() );
  REQUIRE( fileExists(dir, fileName) );
}

TEST_CASE("fromParserDefinitionOption")
{
  ParserDefinitionOption verboseOptionDef( 'v', QLatin1String("verbose"), QLatin1String("Verbose option") );
  const auto verboseOption = BashCompletionGeneratorOption::fromParserDefinitionOption(verboseOptionDef);
  REQUIRE( verboseOption.shortNameWithDash() == QLatin1String("-v") );
  REQUIRE( verboseOption.nameWithDashes() == QLatin1String("--verbose") );
}

TEST_CASE("fromParserDefinitionCommand")
{
  ParserDefinitionCommand copyCommandDef( QLatin1String("copy") );
  copyCommandDef.setDescription( QLatin1String("Copy a file") );
  copyCommandDef.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file to copy") );
  copyCommandDef.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
  copyCommandDef.addHelpOption();

  const auto copyCommand = BashCompletionGeneratorCommand::fromParserDefinitionCommand(copyCommandDef);
  REQUIRE( copyCommand.name() == QLatin1String("copy") );
  REQUIRE( copyCommand.arguments().size() == 2 );
  REQUIRE( copyCommand.arguments()[0].type() == ArgumentType::File );
  REQUIRE( copyCommand.arguments()[0].name() == QLatin1String("source") );
  REQUIRE( copyCommand.arguments()[1].type() == ArgumentType::Directory );
  REQUIRE( copyCommand.arguments()[1].name() == QLatin1String("destination") );
  REQUIRE( copyCommand.options().size() == 1 );
  REQUIRE( copyCommand.options()[0].shortNameWithDash() == QLatin1String("-h") );
  REQUIRE( copyCommand.options()[0].nameWithDashes() == QLatin1String("--help") );
  
  REQUIRE(false);
}

TEST_CASE("fromParserDefinition")
{
  ParserDefinition parserDefinition;
  parserDefinition.setApplicationName( QLatin1String("my-app") );
  parserDefinition.setApplicationDescription( QLatin1String("My cool app") );
  parserDefinition.addHelpOption();

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.setDescription( QLatin1String("Copy a file") );
  copyCommand.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file to copy") );
  copyCommand.addPositionalArgument( QLatin1String("destination"), QLatin1String("Destination directory") );
  copyCommand.addHelpOption();
  parserDefinition.addSubCommand(copyCommand);

  auto generator = BashCompletionGenerator::fromParserDefinition(parserDefinition);

  REQUIRE( generator.applicationName() == QLatin1String("my-app") );
  REQUIRE( generator.mainCommand().options().size() == 1 );
  REQUIRE( generator.mainCommand().options()[0].shortNameWithDash() == QLatin1String("-h") );
  REQUIRE( generator.mainCommand().options()[0].nameWithDashes() == QLatin1String("--help") );

  REQUIRE( generator.subCommands().size() == 1 );
  REQUIRE( generator.subCommands()[0].name() == QLatin1String("copy") );
  
  std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
  
  REQUIRE(false);
}

TEST_CASE("handleCompletion")
{
  using Impl::completionFindCurrentPositionalArgumentNameString;

  QStringList positionalArguments;

  ParserDefinition parserDefinition;
  parserDefinition.setApplicationName( QLatin1String("myapp") );
  parserDefinition.setApplicationDescription( QLatin1String("My cool app") );
  parserDefinition.addHelpOption();

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.setDescription( QLatin1String("Copy a file") );
  copyCommand.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file to copy") );
  copyCommand.addPositionalArgument( QLatin1String("destination"), QLatin1String("Destination directory") );
  copyCommand.addHelpOption();
  parserDefinition.addSubCommand(copyCommand);

  auto generator = BashCompletionGenerator::fromParserDefinition(parserDefinition);

  SECTION("empty")
  {
    REQUIRE( !generator.handleCompletion(positionalArguments) );
  }

  SECTION("completion-find-current-positional-argument-name")
  {
    positionalArguments = QStringList{completionFindCurrentPositionalArgumentNameString()};
    REQUIRE( !generator.handleCompletion(positionalArguments) );
  }

  SECTION("completion-find-current-positional-argument-name myapp")
  {
    positionalArguments = QStringList{completionFindCurrentPositionalArgumentNameString(),QLatin1String("myapp")};
    REQUIRE( generator.handleCompletion(positionalArguments) );
  }
}
