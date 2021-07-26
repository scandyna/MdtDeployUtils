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
#include "CommandLineParser.h"
#include "CommandLineCommand.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/OverwriteBehavior.h"
#include "Mdt/CommandLineParser/Parser.h"
#include "Mdt/CommandLineParser/ParserResult.h"
#include "Mdt/CommandLineParser/BashCompletionParser.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <QLatin1Char>
#include <cassert>
#include <cstdlib>

// #include <QDebug>
#include <iostream>

using namespace Mdt::CommandLineParser;
using namespace Mdt::DeployUtils;

CommandLineParser::CommandLineParser(QObject *parent)
 : QObject(parent)
{
  mParserDefinition.setApplicationName( QLatin1String("mdtdeployutils") );
  setApplicationDescription();

  mParserDefinition.addHelpOption();

  const QString loggerBackendDescription = tr(
    "Message logger backend to use.\n"
    "Available backends:\n"
    "- console (the default): messages are printed to stdout and errors to stderr\n"
    "- cmake: like console, but imitates the CMake style messaging."
  );
  ParserDefinitionOption loggerBackendOption( QLatin1String("logger-backend"), loggerBackendDescription );
  loggerBackendOption.setValueName( QLatin1String("backend") );
  loggerBackendOption.setPossibleValues({QLatin1String("console"),QLatin1String("cmake")});
  mParserDefinition.addOption(loggerBackendOption);

  addGetSharedLibrariesTargetDependsOnCommand();

  mCopySharedLibrariesTargetDependsOnDefinition.setApplicationName( mParserDefinition.applicationName() );
  mCopySharedLibrariesTargetDependsOnDefinition.setup();
  mParserDefinition.addSubCommand( mCopySharedLibrariesTargetDependsOnDefinition.command() );

  addDeployApplicationCommand();
}

void CommandLineParser::process(const QStringList & arguments)
{
  std::cout << "cmd args: " << arguments.join(QLatin1Char(' ')).toStdString() << std::endl;

  Parser parser(mParserDefinition);
  parser.parse(arguments);

  if( handleBashCompletion(parser.commandLine(), mParserDefinition) ){
    return;
  }

  if( parser.hasError() ){
    throw CommandLineParseError( parser.errorText() );
  }

  const ParserResult parserResult = parser.toParserResult();

  if( parserResult.isHelpOptionSet() ){
    showInfo( mParserDefinition.getHelpText() );
    std::exit(0);
  }

  const QStringList loggerBackendOptionValues = parserResult.getValues( loggerBackendOption() );
  if( !loggerBackendOptionValues.isEmpty() ){
    if( loggerBackendOptionValues.count() > 1 ){
      const QString message = tr("logger-backend option given more than once");
      throw CommandLineParseError(message);
    }
    const QString backend = loggerBackendOptionValues.at(0);
    if( backend == QLatin1String("console") ){
      mMessageLoggerBackend = MessageLoggerBackend::Console;
    }else if( backend == QLatin1String("cmake") ){
      mMessageLoggerBackend = MessageLoggerBackend::CMake;
    }else{
      const QString message = tr("given backend '%1' is not supported").arg(backend);
      throw CommandLineParseError(message);
    }
  }

  const CommandLineCommand command = commandFromString( parserResult.subCommand().name() );
  switch(command){
    case CommandLineCommand::GetSharedLibrariesTargetDependsOn:
      processGetSharedLibrariesTargetDependsOn( parserResult.subCommand() );
      return;
    case CommandLineCommand::CopySharedLibrariesTargetDependsOn:
      processCopySharedLibrariesTargetDependsOn( parserResult.subCommand() );
      return;
    case CommandLineCommand::DeployApplication:
      processDeployApplicationCommand( parserResult.subCommand() );
      return;
    case CommandLineCommand::Unknown:
      break;
  }

  /* If the user typed a unknown sub-command,
   * the parser will put it as a positional argument
   */

  if( !parserResult.hasPositionalArguments() ){
    const QString message = tr("Expected at least a sub-command. Try mdtdeployutils --help.");
    throw CommandLineParseError(message);
  }

  const QString message = tr("Given command '%1' is not supported").arg( parserResult.positionalArgumentAt(0) );
  throw CommandLineParseError(message);
}

void CommandLineParser::processGetSharedLibrariesTargetDependsOn(const ParserResultCommand & resultCommand)
{
  if( resultCommand.isHelpOptionSet() ){
    showInfo( mParserDefinition.getSubCommandHelpText( resultCommand.name() ) );
    std::exit(0);
  }

//   GetSharedLibrariesTargetDependsOnCommandLineParser parser;
// 
//   parser.process(arguments);
}

void CommandLineParser::processCopySharedLibrariesTargetDependsOn(const ParserResultCommand & resultCommand)
{
  mCommand = CommandLineCommand::CopySharedLibrariesTargetDependsOn;

  if( resultCommand.isHelpOptionSet() ){
    showInfo( mParserDefinition.getSubCommandHelpText( resultCommand.name() ) );
    std::exit(0);
  }

  const QStringList overwriteBehaviorValues = resultCommand.getValues( mCopySharedLibrariesTargetDependsOnDefinition.overwriteBehaviorOption() );
  if( !overwriteBehaviorValues.isEmpty() ){
    if( overwriteBehaviorValues.count() > 1 ){
      const QString message = tr("overwrite-behavior option given more than once");
      throw CommandLineParseError(message);
    }
    const QString overwriteBehavior = overwriteBehaviorValues.at(0);
    if( overwriteBehavior == QLatin1String("keep") ){
      mCopySharedLibrariesTargetDependsOnRequest.overwriteBehavior = OverwriteBehavior::Keep;
    }else if( overwriteBehavior == QLatin1String("overwrite") ){
      mCopySharedLibrariesTargetDependsOnRequest.overwriteBehavior = OverwriteBehavior::Overwrite;
    }else if( overwriteBehavior == QLatin1String("fail") ){
      mCopySharedLibrariesTargetDependsOnRequest.overwriteBehavior = OverwriteBehavior::Fail;
    }else{
      const QString message = tr("unknown overwrite-behavior '%1'").arg(overwriteBehavior);
      throw CommandLineParseError(message);
    }
  }

  if( resultCommand.isSet( mCopySharedLibrariesTargetDependsOnDefinition.removeRpathOption() ) ){
    mCopySharedLibrariesTargetDependsOnRequest.removeRpath = true;
  }

  const QStringList searchPrefixPathListValues = resultCommand.getValues( mCopySharedLibrariesTargetDependsOnDefinition.searchPrefixPathListOption() );
  if( !searchPrefixPathListValues.isEmpty() ){
    if( searchPrefixPathListValues.count() > 1 ){
      const QString message = tr("search-prefix-path-list option given more than once");
      throw CommandLineParseError(message);
    }
    mCopySharedLibrariesTargetDependsOnRequest.searchPrefixPathList
      = searchPrefixPathListValues.at(0).split( QChar::fromLatin1(';'), QString::SkipEmptyParts );
  }

  processCopySharedLibrariesTargetDependsOnCompilerLocation( resultCommand.getValues( mCopySharedLibrariesTargetDependsOnDefinition.compilerLocationOption() ) );

  if( resultCommand.positionalArgumentCount() != 2 ){
    const QString message = tr(
      "expected 2 (positional) arguments: target file and destination directory.\n"
      "given: %1"
    ).arg( resultCommand.positionalArguments().join( QLatin1Char(',') ) );
    throw CommandLineParseError(message);
  }

  mCopySharedLibrariesTargetDependsOnRequest.targetFilePath = resultCommand.positionalArgumentAt(0);
  mCopySharedLibrariesTargetDependsOnRequest.destinationDirectoryPath = resultCommand.positionalArgumentAt(1);
}

void CommandLineParser::processCopySharedLibrariesTargetDependsOnCompilerLocation(const QStringList & compilerLocationValues)
{
  if( compilerLocationValues.isEmpty() ){
    return;
  }

  if( compilerLocationValues.count() > 1 ){
    const QString message = tr("compiler-location option given more than once");
    throw CommandLineParseError(message);
  }

  const QStringList compilerLocationKeyAndValue = compilerLocationValues.at(0).split( QLatin1Char('=') );

//   if( compilerLocationKeyAndValue.count() == 1 ){
//     const QString compilerLocationType = compilerLocationKeyAndValue.at(0);
//     if( compilerLocationType == QLatin1String("from-env") ){
//       mCopySharedLibrariesTargetDependsOnRequest.compilerLocationType = CompilerLocationType::FromEnv;
//       return;
//     }else{
//       const QString message = tr("unknown compiler-location type '%1'").arg(compilerLocationType);
//       throw CommandLineParseError(message);
//     }
//   }

  if( compilerLocationKeyAndValue.count() != 2 ){
    const QString message = tr("compiler-location: expected type=value syntax, got '%1'").arg( compilerLocationValues.at(0) );
    throw CommandLineParseError(message);
  }

  const QString compilerLocationType = compilerLocationKeyAndValue.at(0);
  if( compilerLocationType == QLatin1String("from-env") ){
    mCopySharedLibrariesTargetDependsOnRequest.compilerLocationType = CompilerLocationType::FromEnv;
    QString varName = compilerLocationKeyAndValue.at(1);
    varName.remove( QLatin1Char('"') );
    mCopySharedLibrariesTargetDependsOnRequest.compilerLocationValue = varName;
  }else if( compilerLocationType == QLatin1String("vc-install-dir") ){
    mCopySharedLibrariesTargetDependsOnRequest.compilerLocationType = CompilerLocationType::VcInstallDir;
    QString path = compilerLocationKeyAndValue.at(1);
    path.remove( QLatin1Char('"') );
    mCopySharedLibrariesTargetDependsOnRequest.compilerLocationValue = path;
  }else if( compilerLocationType == QLatin1String("compiler-path") ){
    mCopySharedLibrariesTargetDependsOnRequest.compilerLocationType = CompilerLocationType::CompilerPath;
    QString path = compilerLocationKeyAndValue.at(1);
    path.remove( QLatin1Char('"') );
    mCopySharedLibrariesTargetDependsOnRequest.compilerLocationValue = path;
  }else{
    const QString message = tr("unknown compiler-location type '%1'").arg(compilerLocationType);
    throw CommandLineParseError(message);
  }
}

void CommandLineParser::processDeployApplicationCommand(const Mdt::CommandLineParser::ParserResultCommand& resultCommand)
{
  if( resultCommand.isHelpOptionSet() ){
    showInfo( mParserDefinition.getSubCommandHelpText( resultCommand.name() ) );
    std::exit(0);
  }
}

void CommandLineParser::setApplicationDescription()
{
  const QString description = tr(
    "\nHelper tool to deploy binary targets and its dependencies, like shared libraries and plugins.\n"
    "This tool is divided in commands (subcommands). Each command has its own positional arguments and options.\n"
    "To get help, pass command --help (or command -h).\n"
    "Example:\n"
    "%1 %2 --help"
  ).arg( QCoreApplication::applicationName(), commandName(CommandLineCommand::GetSharedLibrariesTargetDependsOn) );

  mParserDefinition.setApplicationDescription(description);
}

void CommandLineParser::addGetSharedLibrariesTargetDependsOnCommand()
{
  mGetSharedLibrariesTargetDependsOnCommand.setName( commandName(CommandLineCommand::GetSharedLibrariesTargetDependsOn) );

  const QString description = tr(
    "Get shared libraries a target depends on.\n"
    "The list of shared libraries is returned HOW ??\n"
    "Example:\n"
    "%1 %2 /home/me/opt/libs/someLib.so"
  ).arg( mParserDefinition.applicationName(), mGetSharedLibrariesTargetDependsOnCommand.name() );
  mGetSharedLibrariesTargetDependsOnCommand.setDescription(description);

  mGetSharedLibrariesTargetDependsOnCommand.addPositionalArgument( ValueType::File, QLatin1String("target"), tr("Path to a executable or a shared library.") );
  mGetSharedLibrariesTargetDependsOnCommand.addHelpOption();

  mParserDefinition.addSubCommand(mGetSharedLibrariesTargetDependsOnCommand);
}

void CommandLineParser::addDeployApplicationCommand()
{
  mDeployApplicationCommand.setName( commandName(CommandLineCommand::DeployApplication) );

  const QString description = tr(
    "Deploy a application on the base of given executable.\n"
    "Example:\n"
    "%1 %2 ./myApp"
  ).arg( mParserDefinition.applicationName(), mDeployApplicationCommand.name() );
  mDeployApplicationCommand.setDescription(description);

  mDeployApplicationCommand.addPositionalArgument( ValueType::File, QLatin1String("executable"), tr("Path to the application  executable.") );
  mDeployApplicationCommand.addHelpOption();

  mParserDefinition.addSubCommand(mDeployApplicationCommand);
}
