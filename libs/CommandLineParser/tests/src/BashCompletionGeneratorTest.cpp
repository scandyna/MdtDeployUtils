/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
#include "Catch2QString.h"
#include "Mdt/CommandLineParser/BashCompletionGenerator.h"
#include "Mdt/CommandLineParser/Impl/BashCompletionGenerator.h"
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
  REQUIRE( command.positionalArguments().size() == 1 );
  REQUIRE( command.positionalArguments()[0].type() == ArgumentType::File );
  REQUIRE( command.positionalArguments()[0].name() == QLatin1String("source") );
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

TEST_CASE("BashCompletionGeneratorOption_fromParserDefinitionOption")
{
  ParserDefinitionOption verboseOptionDef( 'v', QLatin1String("verbose"), QLatin1String("Verbose option") );

  const auto verboseOption = BashCompletionGeneratorOption::fromParserDefinitionOption(verboseOptionDef);
  REQUIRE( verboseOption.shortNameWithDash() == QLatin1String("-v") );
  REQUIRE( verboseOption.nameWithDashes() == QLatin1String("--verbose") );
}

TEST_CASE("BashCompletionGeneratorPositionalArgument_fromParserDefinitionPositionalArgument")
{
  ParserDefinitionPositionalArgument defArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file") );

  const auto argument = BashCompletionGeneratorPositionalArgument::fromParserDefinitionPositionalArgument(defArgument);
  REQUIRE( argument.type() == ArgumentType::File );
  REQUIRE( argument.name() == QLatin1String("source") );
}

TEST_CASE("BashCompletionGeneratorCommand_fromParserDefinitionCommand")
{
  SECTION("No name (must not assert)")
  {
    ParserDefinitionCommand commandDef;

    const auto copyCommand = BashCompletionGeneratorCommand::fromParserDefinitionCommand(commandDef);
    REQUIRE( copyCommand.name().isEmpty() );
  }

  SECTION("With name, options and positional arguments")
  {
    ParserDefinitionCommand copyCommandDef( QLatin1String("copy") );
    copyCommandDef.setDescription( QLatin1String("Copy a file") );
    copyCommandDef.addPositionalArgument( ArgumentType::File, QLatin1String("source"), QLatin1String("Source file to copy") );
    copyCommandDef.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), QLatin1String("Destination directory") );
    copyCommandDef.addHelpOption();

    const auto copyCommand = BashCompletionGeneratorCommand::fromParserDefinitionCommand(copyCommandDef);
    REQUIRE( copyCommand.name() == QLatin1String("copy") );
    REQUIRE( copyCommand.positionalArguments().size() == 2 );
    REQUIRE( copyCommand.positionalArguments()[0].type() == ArgumentType::File );
    REQUIRE( copyCommand.positionalArguments()[0].name() == QLatin1String("source") );
    REQUIRE( copyCommand.positionalArguments()[1].type() == ArgumentType::Directory );
    REQUIRE( copyCommand.positionalArguments()[1].name() == QLatin1String("destination") );
    REQUIRE( copyCommand.options().size() == 1 );
    REQUIRE( copyCommand.options()[0].shortNameWithDash() == QLatin1String("-h") );
    REQUIRE( copyCommand.options()[0].nameWithDashes() == QLatin1String("--help") );
  }
}

TEST_CASE("fromParserDefinition")
{
  ParserDefinition parserDefinition;
  parserDefinition.setApplicationName( QLatin1String("myapp") );
  parserDefinition.setApplicationDescription( QLatin1String("My cool app") );
  parserDefinition.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("arg1"), QLatin1String("Some argument") );
  parserDefinition.addHelpOption();

  ParserDefinitionCommand copyCommand( QLatin1String("copy") );
  copyCommand.setDescription( QLatin1String("Copy a file") );
  copyCommand.addPositionalArgument( QLatin1String("source"), QLatin1String("Source file to copy") );
  copyCommand.addPositionalArgument( QLatin1String("destination"), QLatin1String("Destination directory") );
  copyCommand.addHelpOption();
  parserDefinition.addSubCommand(copyCommand);

  auto generator = BashCompletionGenerator::fromParserDefinition(parserDefinition);

  REQUIRE( generator.applicationName() == QLatin1String("myapp") );
  REQUIRE( generator.mainCommand().options().size() == 1 );
  REQUIRE( generator.mainCommand().options()[0].shortNameWithDash() == QLatin1String("-h") );
  REQUIRE( generator.mainCommand().options()[0].nameWithDashes() == QLatin1String("--help") );
  REQUIRE( generator.mainCommand().positionalArgumentCount() == 1 );

  REQUIRE( generator.subCommands().size() == 1 );
  REQUIRE( generator.subCommands()[0].name() == QLatin1String("copy") );
  REQUIRE( generator.subCommands()[0].options().size() == 1 );
  REQUIRE( generator.subCommands()[0].positionalArgumentCount() == 2 );
}

TEST_CASE("compreplyStatementFromCommandOptions")
{
  using Impl::compreplyStatementFromCommandOptions;

  BashCompletionGeneratorCommand command;
  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  SECTION("--help")
  {
    command.addOption( QLatin1String("help") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"--help\" -- \"$cur\"))");
    REQUIRE( compreplyStatementFromCommandOptions(command, curVariableName) == expectedResult );
  }

  SECTION("-h --help")
  {
    command.addOption( 'h', QLatin1String("help") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"-h --help\" -- \"$cur\"))");
    REQUIRE( compreplyStatementFromCommandOptions(command, curVariableName) == expectedResult );
  }
}

TEST_CASE("compgenActionNameFromArgumentType")
{
  using Impl::compgenActionNameFromArgumentType;

  SECTION("file")
  {
    REQUIRE( compgenActionNameFromArgumentType(ArgumentType::File) == QLatin1String("file") );
  }

  SECTION("directory")
  {
    REQUIRE( compgenActionNameFromArgumentType(ArgumentType::Directory) == QLatin1String("directory") );
  }

  SECTION("diretory or file")
  {
    REQUIRE( compgenActionNameFromArgumentType(ArgumentType::DirectoryOrFile) == QLatin1String("file") );
  }

  SECTION("Unspecified")
  {
    REQUIRE( compgenActionNameFromArgumentType(ArgumentType::Unspecified).isEmpty() );
  }
}

TEST_CASE("compreplyStatementFromPositionalArgument")
{
  using Impl::compreplyStatementFromPositionalArgument;

  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  SECTION("source file")
  {
    BashCompletionGeneratorPositionalArgument argument( ArgumentType::File, QLatin1String("source") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
    REQUIRE( compreplyStatementFromPositionalArgument(argument, curVariableName) == expectedResult );
  }

  SECTION("destination diretory")
  {
    BashCompletionGeneratorPositionalArgument argument( ArgumentType::Directory, QLatin1String("destination") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -A directory -- \"$cur\"))");
    REQUIRE( compreplyStatementFromPositionalArgument(argument, curVariableName) == expectedResult );
  }

  SECTION("destination diretory or file")
  {
    BashCompletionGeneratorPositionalArgument argument( ArgumentType::DirectoryOrFile, QLatin1String("destination") );
    expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
    REQUIRE( compreplyStatementFromPositionalArgument(argument, curVariableName) == expectedResult );
  }

  SECTION("somearg Unspecified")
  {
    BashCompletionGeneratorPositionalArgument argument( ArgumentType::Unspecified, QLatin1String("somearg") );
    expectedResult = QLatin1String("");
    REQUIRE( compreplyStatementFromPositionalArgument(argument, curVariableName) == expectedResult );
  }
}

TEST_CASE("compreplyStatementFromFirstPositionalArgumentInCommand")
{
  using Impl::compreplyStatementFromFirstPositionalArgumentInCommand;

  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  BashCompletionGeneratorCommand command;
  BashCompletionGeneratorPositionalArgument sourceFile( ArgumentType::File, QLatin1String("source") );
  BashCompletionGeneratorPositionalArgument someArg( ArgumentType::Unspecified, QLatin1String("somearg") );

  SECTION("command without option")
  {
    SECTION("source file")
    {
      command.addPositionalArgument(sourceFile);
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
      REQUIRE( compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName) == expectedResult );
    }

    SECTION("somearg Unspecified")
    {
      command.addPositionalArgument(someArg);
      REQUIRE( compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName).isEmpty() );
    }
  }

  SECTION("command with -h --help option")
  {
    command.addOption( 'h', QLatin1String("help") );

    SECTION("source file")
    {
      command.addPositionalArgument(sourceFile);
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -W \"-h --help\" -- \"$cur\"))");
      REQUIRE( compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName) == expectedResult );
    }

    SECTION("somearg Unspecified")
    {
      command.addPositionalArgument(someArg);
      expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"-h --help\" -- \"$cur\"))");
      REQUIRE( compreplyStatementFromFirstPositionalArgumentInCommand(command, curVariableName) == expectedResult );
    }
  }
}

TEST_CASE("compreplyStatementFromPositionalArgumentInCommand")
{
  using Impl::compreplyStatementFromPositionalArgumentInCommand;

  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  BashCompletionGeneratorCommand command;
  command.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
  command.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination") );
  command.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("somearg") );

  SECTION("command without option")
  {
    SECTION("source file")
    {
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -- \"$cur\"))");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 0, curVariableName) == expectedResult );
    }

    SECTION("destination diretory")
    {
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A directory -- \"$cur\"))");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 1, curVariableName) == expectedResult );
    }

    SECTION("somearg Unspecified")
    {
      expectedResult = QLatin1String("");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 2, curVariableName) == expectedResult );
    }
  }

  SECTION("command with -h --help option")
  {
    command.addOption( 'h', QLatin1String("help") );

    SECTION("source file")
    {
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A file -W \"-h --help\" -- \"$cur\"))");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 0, curVariableName) == expectedResult );
    }

    SECTION("destination diretory")
    {
      expectedResult = QLatin1String("COMPREPLY=($(compgen -A directory -- \"$cur\"))");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 1, curVariableName) == expectedResult );
    }

    SECTION("somearg Unspecified")
    {
      expectedResult = QLatin1String("");
      REQUIRE( compreplyStatementFromPositionalArgumentInCommand(command, 2, curVariableName) == expectedResult );
    }
  }
}

TEST_CASE("casePatternFromCommandPositionalArgument")
{
  using Impl::casePatternFromCommandPositionalArgument;

  BashCompletionGeneratorCommand command;
  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  SECTION("source file")
  {
    command.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
    expectedResult = QLatin1String("    source)\n      COMPREPLY=($(compgen -A file -- \"$cur\"))\n      ;;");
    REQUIRE( casePatternFromCommandPositionalArgument(command, 0, curVariableName) == expectedResult );
  }

  SECTION("somearg Unspecified")
  {
    command.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("somearg") );
    expectedResult = QLatin1String("");
    REQUIRE( casePatternFromCommandPositionalArgument(command, 0, curVariableName) == expectedResult );
  }
}

TEST_CASE("compreplyStatementForCommandCaseBlock")
{
  using Impl::compreplyStatementForCommandCaseBlock;

  BashCompletionGenerator generator;
  QString curVariableName = QLatin1String("cur");
  QString expectedResult;

  BashCompletionGeneratorCommand mainCommand;

  BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
  copyCommand.addOption( 'f', QLatin1String("force") );

  BashCompletionGeneratorCommand cutCommand( QLatin1String("cut") );
  cutCommand.addOption( QLatin1String("verbose") );

  SECTION("copy")
  {
    generator.addSubCommand(copyCommand);

    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"copy\" -- \"$cur\"))");
    REQUIRE( compreplyStatementForCommandCaseBlock(generator, curVariableName) == expectedResult );
  }

  SECTION("copy cut")
  {
    generator.addSubCommand(copyCommand);
    generator.addSubCommand(cutCommand);

    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"copy cut\" -- \"$cur\"))");
    REQUIRE( compreplyStatementForCommandCaseBlock(generator, curVariableName) == expectedResult );
  }

  SECTION("--verbose copy")
  {
    mainCommand.addOption( QLatin1String("verbose") );
    generator.setMainCommand(mainCommand);
    generator.addSubCommand(copyCommand);

    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"--verbose copy\" -- \"$cur\"))");
    REQUIRE( compreplyStatementForCommandCaseBlock(generator, curVariableName) == expectedResult );
  }

  SECTION("-h --help copy")
  {
    mainCommand.addOption( 'h', QLatin1String("help") );
    generator.setMainCommand(mainCommand);
    generator.addSubCommand(copyCommand);

    expectedResult = QLatin1String("COMPREPLY=($(compgen -W \"-h --help copy\" -- \"$cur\"))");
    REQUIRE( compreplyStatementForCommandCaseBlock(generator, curVariableName) == expectedResult );
  }
}

TEST_CASE("generateCompletionFunction")
{
  using Impl::generateCompletionFunction;

  BashCompletionGenerator generator;
  generator.setApplicationName( QLatin1String("mytool") );

  REQUIRE( !generateCompletionFunction(generator).isEmpty() );
}

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
  copyCommand.addOption( 'h', QLatin1String("help") );
  copyCommand.addOption( 'i', QLatin1String("interactive") );
  generator.addSubCommand(copyCommand);

  std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;

  const QString fileName = QLatin1String("mytool-completion.bash");
  REQUIRE( !fileExists(dir, fileName) );
  generator.generateScriptToFile( dir.path() );
  REQUIRE( fileExists(dir, fileName) );
}

TEST_CASE("GenerateScript_sandbox")
{
  BashCompletionGenerator generator;
  generator.setApplicationName( QLatin1String("myapp") );
  BashCompletionGeneratorCommand mainCommand;

  SECTION("Simple app")
  {
    std::cout << "Simple app ";

    SECTION("with positional argument")
    {
      std::cout << "with positional argument" << std::endl;

      mainCommand.addPositionalArgument( ArgumentType::File, QLatin1String("mainfile") );
      generator.setMainCommand(mainCommand);
      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }

    SECTION("with option")
    {
      std::cout << "with option" << std::endl;

      mainCommand.addOption( 'h', QLatin1String("help") );
      generator.setMainCommand(mainCommand);
      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }

    SECTION("with option and file positional argument")
    {
      std::cout << "with option and file positional argument" << std::endl;

      mainCommand.addOption( 'h', QLatin1String("help") );
      mainCommand.addPositionalArgument( ArgumentType::File, QLatin1String("mainfile") );
      generator.setMainCommand(mainCommand);
      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }

    SECTION("with option and Unspecified positional argument")
    {
      std::cout << "with option and Unspecified positional argument" << std::endl;

      mainCommand.addOption( 'h', QLatin1String("help") );
      mainCommand.addPositionalArgument( ArgumentType::Unspecified, QLatin1String("arg1") );
      generator.setMainCommand(mainCommand);
      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }
  }

  SECTION("App with sub-command")
  {
    std::cout << "App with sub-command ";

    SECTION("myapp [options] command [options] source destination")
    {
      std::cout << "myapp [options] command [options] source destination" << std::endl;

      mainCommand.addOption( 'h', QLatin1String("help") );
      generator.setMainCommand(mainCommand);
      BashCompletionGeneratorCommand copyCommand( QLatin1String("copy") );
      copyCommand.addOption( QLatin1String("force") );
      copyCommand.addPositionalArgument( ArgumentType::File, QLatin1String("source") );
      copyCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination") );
      generator.addSubCommand(copyCommand);

      std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
    }
  }
}
