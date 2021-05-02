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
#include "MessageLogger.h"
#include "CommandLineCommand.h"
#include "Mdt/DeployUtils/OverwriteBehavior.h"
#include "Mdt/CommandLineParser/Parser.h"
#include "Mdt/CommandLineParser/ParserResult.h"
#include "Mdt/CommandLineParser/BashCompletionParser.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <cassert>

using namespace Mdt::CommandLineParser;
using namespace Mdt::DeployUtils;

CommandLineParser::CommandLineParser(QObject *parent)
 : QObject(parent)
{
  mParserDefinition.setApplicationName( QLatin1String("mdtdeployutils") );
  setApplicationDescription();

  mParserDefinition.addHelpOption();

  addGetSharedLibrariesTargetDependsOnCommand();

  mCopySharedLibrariesTargetDependsOnDefinition.setApplicationName( mParserDefinition.applicationName() );
  mCopySharedLibrariesTargetDependsOnDefinition.setup();
  mParserDefinition.addSubCommand( mCopySharedLibrariesTargetDependsOnDefinition.command() );

  addDeployApplicationCommand();
}

void CommandLineParser::process(const QStringList & arguments)
{
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
    showMessage( mParserDefinition.getHelpText() );
    return;
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

CommandLineParserResult CommandLineParser::processGetSharedLibrariesTargetDependsOn(const ParserResultCommand & resultCommand)
{
  if( resultCommand.isHelpOptionSet() ){
    showMessage( mParserDefinition.getSubCommandHelpText( resultCommand.name() ) );
    return CommandLineParserResult{};
  }

//   GetSharedLibrariesTargetDependsOnCommandLineParser parser;
// 
//   parser.process(arguments);

  return CommandLineParserResult{};
}

void CommandLineParser::processCopySharedLibrariesTargetDependsOn(const ParserResultCommand & resultCommand)
{
  mCommand = CommandLineCommand::CopySharedLibrariesTargetDependsOn;

  if( resultCommand.isHelpOptionSet() ){
    showMessage( mParserDefinition.getSubCommandHelpText( resultCommand.name() ) );
    return;
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

  if( resultCommand.positionalArgumentCount() != 2 ){
    const QString message = tr("expected 2 (positional) arguments: target file and destination directory");
    throw CommandLineParseError(message);
  }

  mCopySharedLibrariesTargetDependsOnRequest.targetFilePath = resultCommand.positionalArgumentAt(0);
  mCopySharedLibrariesTargetDependsOnRequest.destinationDirectoryPath = resultCommand.positionalArgumentAt(1);
}

CommandLineParserResult CommandLineParser::processDeployApplicationCommand(const Mdt::CommandLineParser::ParserResultCommand& resultCommand)
{
  if( resultCommand.isHelpOptionSet() ){
    showMessage( mParserDefinition.getSubCommandHelpText( resultCommand.name() ) );
    return CommandLineParserResult{};
  }

  return CommandLineParserResult{};
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
