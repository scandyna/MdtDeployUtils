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
#include "CommandLineParser.h"
#include "MessageLogger.h"
#include "CommandLineCommand.h"
#include "GetSharedLibrariesTargetDependsOnCommandLineParser.h"
#include "CopySharedLibrariesTargetDependsOnCommandLineParser.h"
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
  setApplicationDescription();
//   mParser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
//   mParser.addHelpOption();

//   addCommandArgument();
  
  mParserDefinition.addHelpOption();
  addGetSharedLibrariesTargetDependsOnCommand();
  addCopySharedLibrariesTargetDependsOnCommand();
}

CommandLineParserResult CommandLineParser::process()
{
  Parser parser;
  const ParserResult parserResult = parser.parse( mParserDefinition, QCoreApplication::arguments() );

  if( parserResult.hasError() ){
    showErrorMessage( parserResult.errorText() );
    return CommandLineParserResult{};
  }
//   mParser.process(QCoreApplication::arguments());

  if( handleBashCompletion(parserResult, mParserDefinition) ){
//     qDebug() << "handling bash completion ...";
    return CommandLineParserResult{};
  }

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
    case CommandLineCommand::Unknown:
      break;
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
    qDebug() << mGetSharedLibrariesTargetDependsOnCommand.name() << ": help requested";
    /// \todo get help text on command
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
    qDebug() << mCopySharedLibrariesTargetDependsOnCommand.name() << ": help requested";
    /// \todo get help text on command
    return CommandLineParserResult{};
  }

//   CopySharedLibrariesTargetDependsOnCommandLineParser parser;
// 
//   parser.process(arguments);

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
  ).arg( QCoreApplication::applicationName(), commandName( CommandLineCommand::GetSharedLibrariesTargetDependsOn) );

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
  mGetSharedLibrariesTargetDependsOnCommand.setName( QLatin1String("get-shared-libraries-target-depends-on") );
//   ParserDefinitionCommand command( QLatin1String("get-shared-libraries-target-depends-on") );

  const QString description = tr(
    "Get shared libraries a target depends on.\n"
    "The list of shared libraries is returned HOW ??\n"
    "Example:\n"
    "%1 %2 /home/me/opt/libs/someLib.so"
  ).arg( mParserDefinition.applicationName(), mGetSharedLibrariesTargetDependsOnCommand.name() );
  mGetSharedLibrariesTargetDependsOnCommand.setDescription(description);

  mGetSharedLibrariesTargetDependsOnCommand.addPositionalArgument( ArgumentType::File, QLatin1String("target"), tr("Path to a executable or a shared library.") );
  mGetSharedLibrariesTargetDependsOnCommand.addHelpOption();

  mParserDefinition.addSubCommand(mGetSharedLibrariesTargetDependsOnCommand);
}

void CommandLineParser::addCopySharedLibrariesTargetDependsOnCommand()
{
  mCopySharedLibrariesTargetDependsOnCommand.setName( QLatin1String("copy-shared-libraries-target-depends-on") );
//   ParserDefinitionCommand command( QLatin1String("copy-shared-libraries-target-depends-on") );

  const QString description = tr(
    "Copy shared libraries a target depends on to a destination.\n"
    "Example:\n"
    "%1 %2 /home/me/dev/build/myapp/src/myapp /home/me/opt/myapp/lib/"
  ).arg( mParserDefinition.applicationName(), mCopySharedLibrariesTargetDependsOnCommand.name() );
  mCopySharedLibrariesTargetDependsOnCommand.setDescription(description);

  mCopySharedLibrariesTargetDependsOnCommand.addPositionalArgument( ArgumentType::File, QLatin1String("target"), tr("Path to a executable or a shared library.") );
  mCopySharedLibrariesTargetDependsOnCommand.addPositionalArgument( ArgumentType::Directory, QLatin1String("destination"), tr("Path to the destination directory.") );
  mCopySharedLibrariesTargetDependsOnCommand.addHelpOption();

  mParserDefinition.addSubCommand(mCopySharedLibrariesTargetDependsOnCommand);
}
