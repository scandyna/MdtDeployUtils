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
#include "CopySharedLibrariesTargetDependsOnCommandLineParser.h"
#include "CommandLineCommand.h"
#include "MessageLogger.h"
#include <QCommandLineOption>
#include <QLatin1String>
#include <QStringList>
#include <QCoreApplication>

#include <QDebug>

#include <iostream>

CopySharedLibrariesTargetDependsOnCommandLineParser::CopySharedLibrariesTargetDependsOnCommandLineParser(QObject *parent)
 : QObject(parent)
{
  setApplicationDescription();
  mParser.addHelpOption();
}

void CopySharedLibrariesTargetDependsOnCommandLineParser::process(const QStringList & arguments)
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

QString CopySharedLibrariesTargetDependsOnCommandLineParser::commandName()
{
  return ::commandName(CommandLineCommand::CopySharedLibrariesTargetDependsOn);
}

void CopySharedLibrariesTargetDependsOnCommandLineParser::setApplicationDescription()
{
  const QString message = tr(
    "Copy shared libraries a target depends on to a destination.\n"
    "Example:\n"
    "%1 %2 /home/me/dev/build/myapp/src/myapp /home/me/opt/myapp/lib/"
  ).arg( QCoreApplication::applicationName(), commandName() );

  mParser.setApplicationDescription(message);
}

void CopySharedLibrariesTargetDependsOnCommandLineParser::showHelp()
{
  const QString message = tr(
    "Usage: %1 %2 [options] target destination\n"
    "%3\n\n"
    "Options:\n"
    "%4\n\n"
    "Arguments:\n"
    "%5"
  ).arg( QCoreApplication::applicationName(), commandName(), mParser.applicationDescription(), optionsHelpText(), argumentsHelpText() );

  showMessage(message);
}

QString CopySharedLibrariesTargetDependsOnCommandLineParser::optionsHelpText()
{
  return tr(
    "  -h, --help  Displays this help."
  );
}

QString CopySharedLibrariesTargetDependsOnCommandLineParser::argumentsHelpText()
{
  return tr(
    "  target      Path to a executable or a shared library.\n"
    "  destination Path to the destination directory."
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
