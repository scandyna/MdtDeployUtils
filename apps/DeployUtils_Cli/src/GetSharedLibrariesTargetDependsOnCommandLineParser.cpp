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
#include "GetSharedLibrariesTargetDependsOnCommandLineParser.h"
#include "CommandLineCommand.h"
#include "MessageLogger.h"
#include <QCommandLineOption>
#include <QLatin1String>
#include <QStringList>
#include <QCoreApplication>

#include <QDebug>

#include <iostream>

GetSharedLibrariesTargetDependsOnCommandLineParser::GetSharedLibrariesTargetDependsOnCommandLineParser(QObject *parent)
 : QObject(parent)
{
  setApplicationDescription();
  mParser.addHelpOption();
}

void GetSharedLibrariesTargetDependsOnCommandLineParser::process(const QStringList & arguments)
{
//   if( mParser.parse(QStringList() << QCoreApplication::applicationName() << arguments) ){
  if( !mParser.parse(arguments) ){
    qDebug() << "parse failed: " << mParser.errorText();
//     qDebug() << mParser.helpText();
    showHelp();
    exit(1);
  }
  
  qDebug() << "Positional arguments: " << mParser.positionalArguments();
  
  if( mParser.isSet(QLatin1String("help")) ){
    showHelp();
    exit(0);
  }
  
//   mParser.process(arguments);
}

void GetSharedLibrariesTargetDependsOnCommandLineParser::setApplicationDescription()
{
  const QString message = tr(
    "Get shared libraries a target depends on.\n"
    "The list of shared libraries is returned HOW ??\n"
    "Example:\n"
    "%1 %2 /home/me/opt/libs/someLib.so"
  ).arg( QCoreApplication::applicationName(), commandName() );

  mParser.setApplicationDescription(message);
}

void GetSharedLibrariesTargetDependsOnCommandLineParser::showHelp()
{
  const QString message = tr(
    "Usage: %1 %2 [options] target\n"
    "%3\n\n"
    "Options:\n"
    "%4\n\n"
    "Arguments:\n"
    "%5"
  ).arg( QCoreApplication::applicationName(), commandName(), mParser.applicationDescription(), optionsHelpText(), argumentsHelpText() );

  showMessage(message);
}

QString GetSharedLibrariesTargetDependsOnCommandLineParser::commandName()
{
  return ::commandName(CommandLineCommand::GetSharedLibrariesTargetDependsOn);
}

QString GetSharedLibrariesTargetDependsOnCommandLineParser::optionsHelpText()
{
  return tr(
    "  -h, --help  Displays this help."
  );
}

QString GetSharedLibrariesTargetDependsOnCommandLineParser::argumentsHelpText()
{
  return tr(
    "  target      Path to a executable or a shared library."
  );
}

// void GetSharedLibrariesTargetDependsOnCommandLineParser::addHelpOption()
// {
//   QCommandLineOption option(
//     QStringList{QLatin1String("h"),QLatin1String("help")},
//     tr("Displays this helj jkjlkj ljklj p")
//   );
//   mParser.addOption(option);
// }
