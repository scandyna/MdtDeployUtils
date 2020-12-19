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

TEST_CASE("Command_isEmpty")
{
  BashCompletionGeneratorCommand command( QLatin1String("command") );
  REQUIRE( command.isEmpty() );

  SECTION("arguments")
  {
    command.addArgument( QLatin1String("arg-a") );
    REQUIRE( !command.isEmpty() );
  }

  SECTION("options")
  {
    command.addOption( 'h', QLatin1String("help") );
    REQUIRE( !command.isEmpty() );
  }

  SECTION("arguments and options")
  {
    command.addArgument( QLatin1String("arg-a") );
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
  mainCommand.addArgument( QLatin1String("copy") );
  mainCommand.addOption( 'h', QLatin1String("help") );
  generator.setMainCommand(mainCommand);

  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  copyCommand.addArgument( QLatin1String("source") );
  copyCommand.addArgument( QLatin1String("destination") );
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
