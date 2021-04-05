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

#include "GetSharedLibrariesTargetDependsOnCommandLineParser.h"
#include "CopySharedLibrariesTargetDependsOnCommandLineParser.h"

#include "Mdt/CommandLineParser/ParserDefinitionOption.h"
#include "Mdt/CommandLineParser/Parser.h"
#include "Mdt/CommandLineParser/ParserResult.h"
#include "Mdt/CommandLineParser/BashCompletionParser.h"


#include <QString>
#include <QLatin1String>
#include <QCoreApplication>

#include <QDebug>

#include <cassert>

#include <iostream>

// using Mdt::CommandLineParser::ParserDefinitionCommand;
using namespace Mdt::CommandLineParser;

CommandLineParser::CommandLineParser(QObject *parent)
 : QObject(parent)
{
  mParserDefinition.setApplicationName( QLatin1String("mdtdeployutils") );
  setApplicationDescription();
//   mParser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
//   mParser.addHelpOption();

//   addCommandArgument();
  
  mParserDefinition.addHelpOption();
  addGetSharedLibrariesTargetDependsOnCommand();
  addCopySharedLibrariesTargetDependsOnCommand();
  addDeployApplicationCommand();
}

CommandLineParserResult CommandLineParser::process()
{
  Parser parser(mParserDefinition);
//   const ParserResult parserResult = parser.parse( mParserDefinition, QCoreApplication::arguments() );
  parser.parse( QCoreApplication::arguments() );

  if( handleBashCompletion(parser.commandLine(), mParserDefinition) ){
//     qDebug() << "handling bash completion ...";
    return CommandLineParserResult{};
  }

  if( parser.hasError() ){
    showErrorMessage( parser.errorText() );
    return CommandLineParserResult{};
  }

  const ParserResult parserResult = parser.toParserResult();

//   if( parserResult.hasError() ){
//     showErrorMessage( parserResult.errorText() );
//     return CommandLineParserResult{};
//   }
//   mParser.process(QCoreApplication::arguments());

//   if( handleBashCompletion(parserResult, mParserDefinition) ){
// //     qDebug() << "handling bash completion ...";
//     return CommandLineParserResult{};
//   }

  if( parserResult.isHelpOptionSet() ){
    showMessage( mParserDefinition.getHelpText() );
    return CommandLineParserResult{};
  }

//   const QStringList positionalArguments = mParser.positionalArguments();
//   qDebug() << "Positional arguments: " << positionalArguments;

  // Try parse completion arguments
//   if( positionalArguments.count() >= 1 ){
//     if( positionalArguments.at(0) == QLatin1String("completion-find-current-positional-argument-name") ){
//       std::cout << "return-name-of-argument" << std::flush;
//       return CommandLineParserResult{};
//     }
//   }

//   if( positionalArguments.count() < 1 ){
//     showErrorMessage( tr("No command provided") );
//     
//     showMessage(QLatin1String("***** Begin Mdt def help *******"));
//     
//     showMessage( mParserDefinition.getHelpText() );
//     
//     showMessage(QLatin1String("***** Begin Qt help *******"));
//     
//     
//     mParser.showHelp(1);
//   }
  
  const CommandLineCommand command = commandFromString( parserResult.subCommand().name() );
  switch(command){
    case CommandLineCommand::GetSharedLibrariesTargetDependsOn:
      return processGetSharedLibrariesTargetDependsOn( parserResult.subCommand() );
    case CommandLineCommand::CopySharedLibrariesTargetDependsOn:
      return processCopySharedLibrariesTargetDependsOn( parserResult.subCommand() );
    case CommandLineCommand::DeployApplication:
      return processDeployApplicationCommand( parserResult.subCommand() );
    case CommandLineCommand::Unknown:
      break;
  }

  if( !parserResult.hasPositionalArguments() ){
    const QString message = tr("Expected at least a command.");
    showErrorMessage(message);
    return CommandLineParserResult{};
  }
//   const QString message = tr("Given command '%1' is not supported").arg( parserResult.subCommand().name() );
  const QString message = tr("Given command '%1' is not supported").arg( parserResult.positionalArgumentAt(0) );
  showErrorMessage(message);
//   mParser.showHelp(1);

  return CommandLineParserResult{};
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

CommandLineParserResult CommandLineParser::processCopySharedLibrariesTargetDependsOn(const ParserResultCommand & resultCommand)
{
  if( resultCommand.isHelpOptionSet() ){
    showMessage( mParserDefinition.getSubCommandHelpText( resultCommand.name() ) );
    return CommandLineParserResult{};
  }

//   CopySharedLibrariesTargetDependsOnCommandLineParser parser;
// 
//   parser.process(arguments);

  return CommandLineParserResult{};
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
//   mParser.setApplicationDescription(description);
}

// void CommandLineParser::addCommandArgument()
// {
//   const QString description = tr("Command to execute. Available commands are:\n%1\n%2  Longlong long long Longlong long longLonglong long longLonglong long longLonglong long longLonglong long longLonglong long longLonglong long longLonglong long longLonglong long longLonglong long longLonglong long longLonglong long longLonglong long long")
//                               .arg( commandName( CommandLineCommand::GetSharedLibrariesTargetDependsOn),
//                                     commandName( CommandLineCommand::CopySharedLibrariesTargetDependsOn) );
// 
//   
// 
//   mParser.addPositionalArgument( QLatin1String("command"), description, tr("command <command-args>") );
//   
//   mParser.addPositionalArgument( QLatin1String("command-2"), description, tr("command-2 <command-2-args>") );
// }

void CommandLineParser::addGetSharedLibrariesTargetDependsOnCommand()
{
  mGetSharedLibrariesTargetDependsOnCommand.setName( commandName(CommandLineCommand::GetSharedLibrariesTargetDependsOn) );
//   ParserDefinitionCommand command( QLatin1String("get-shared-libraries-target-depends-on") );

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

void CommandLineParser::addCopySharedLibrariesTargetDependsOnCommand()
{
  mCopySharedLibrariesTargetDependsOnCommand.setName( commandName(CommandLineCommand::CopySharedLibrariesTargetDependsOn) );
//   ParserDefinitionCommand command( QLatin1String("copy-shared-libraries-target-depends-on") );

  const QString description = tr(
    "Copy shared libraries a target depends on to a destination.\n"
    "Example:\n"
    "%1 %2 /home/me/dev/build/myapp/src/myapp /home/me/opt/myapp/lib/"
  ).arg( mParserDefinition.applicationName(), mCopySharedLibrariesTargetDependsOnCommand.name() );
  mCopySharedLibrariesTargetDependsOnCommand.setDescription(description);

  mCopySharedLibrariesTargetDependsOnCommand.addPositionalArgument( ValueType::File, QLatin1String("target"), tr("Path to a executable or a shared library.") );
  mCopySharedLibrariesTargetDependsOnCommand.addPositionalArgument( ValueType::Directory, QLatin1String("destination"), tr("Path to the destination directory.") );
  mCopySharedLibrariesTargetDependsOnCommand.addHelpOption();

  const QString overwriteBehaviorOptionDescription = tr(
    "Behavior to adopt when a shared library allready exists at the destination location.\n"
    "Note: if the source and destination locations are the same for a shared library, "
    "the library is allways kept as is, regardless of this option.\n"
    "Possible values are: keep, overwrite or fail.\n"
    "keep: the destination library will not be changed at all.\n"
    "overwrite: the destination library will be replaced.\n"
    "fail: the command will fail if the destination library allready exists."
  );
  ParserDefinitionOption overwriteBehaviorOption( QLatin1String("overwrite-behavior"), overwriteBehaviorOptionDescription );
  overwriteBehaviorOption.setValueName( QLatin1String("behavior") );
  overwriteBehaviorOption.setPossibleValues({QLatin1String("keep"),QLatin1String("overwrite"),QLatin1String("fail")});
  mCopySharedLibrariesTargetDependsOnCommand.addOption(overwriteBehaviorOption);

  const QString removePathOptionDescription = tr(
    "By default, on platform that supports rpath, the rpath informations is set to $ORIGIN for each shared library that has been copied.\n"
    "With this option, the rpath informations are removed for each shared library that has been copied.\n"
    "This option is ignored on platforms that do not support rpath."
  );
  mCopySharedLibrariesTargetDependsOnCommand.addOption( QLatin1String("remove-rpath"), removePathOptionDescription );

  mParserDefinition.addSubCommand(mCopySharedLibrariesTargetDependsOnCommand);
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
