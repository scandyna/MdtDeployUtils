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
#include <QString>
#include <QLatin1String>
#include <QCoreApplication>

#include <QDebug>

CommandLineParser::CommandLineParser(QObject *parent)
 : QObject(parent)
{
  setApplicationDescription();
  mParser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
  mParser.addHelpOption();

  addCommandArgument();
}

CommandLineParserResult CommandLineParser::process()
{
  mParser.process(QCoreApplication::arguments());

  const QStringList positionalArguments = mParser.positionalArguments();
  qDebug() << "Positional arguments: " << positionalArguments;

  if( positionalArguments.count() < 1 ){
    showErrorMessage( tr("No command provided") );
    mParser.showHelp(1);
  }

  const CommandLineCommand command = commandFromString( positionalArguments.at(0) );
  switch(command){
    case CommandLineCommand::GetSharedLibrariesTargetDependsOn:
      return processGetSharedLibrariesTargetDependsOn(positionalArguments);
    case CommandLineCommand::CopySharedLibrariesTargetDependsOn:
      return processCopySharedLibrariesTargetDependsOn(positionalArguments);
    case CommandLineCommand::Unknown:
      break;
  }

  const QString message = tr("Given command '%1' is not supported").arg( positionalArguments.at(0) );
  showErrorMessage(message);
  mParser.showHelp(1);

  return CommandLineParserResult{};
}

CommandLineParserResult CommandLineParser::processGetSharedLibrariesTargetDependsOn(const QStringList & arguments)
{
  GetSharedLibrariesTargetDependsOnCommandLineParser parser;

  parser.process(arguments);

  return CommandLineParserResult{};
}

CommandLineParserResult CommandLineParser::processCopySharedLibrariesTargetDependsOn(const QStringList& arguments)
{
  CopySharedLibrariesTargetDependsOnCommandLineParser parser;

  parser.process(arguments);

  return CommandLineParserResult{};
}

void CommandLineParser::setApplicationDescription()
{
  const QString description = tr(
    "Helper tool to deploy binary targets and its dependencies, like shared libraries and plugins.\n"
    "This tool is divided in commands (subcommands). Each command has its own positional arguments and options.\n"
    "To get help, pass command --help (or command -h).\n"
    "Example:\n"
    "%1 %2 --help"
  ).arg( QCoreApplication::applicationName(), commandName( CommandLineCommand::GetSharedLibrariesTargetDependsOn) );

  mParser.setApplicationDescription(description);
}

void CommandLineParser::addCommandArgument()
{
  const QString description = tr("Command to execute. Available commands are:\n%1\n%2")
                              .arg( commandName( CommandLineCommand::GetSharedLibrariesTargetDependsOn),
                                    commandName( CommandLineCommand::CopySharedLibrariesTargetDependsOn) );

  mParser.addPositionalArgument( QLatin1String("command"), description, tr("command <command-args>") );
}
