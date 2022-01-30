/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2022 Philippe Steinmann.
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
#include "DeployUtilsMain.h"
#include "Mdt/DeployUtils/LogLevel.h"
#include "Mdt/DeployUtils/MessageLogger.h"
#include "Mdt/DeployUtils/CMakeStyleMessageLogger.h"
#include "Mdt/DeployUtils/CopySharedLibrariesTargetDependsOn.h"
#include "Mdt/DeployUtils/CopySharedLibrariesTargetDependsOnRequest.h"
#include "Mdt/DeployUtils/DeployApplicationRequest.h"
#include "Mdt/DeployUtils/DeployApplication.h"
#include <QLatin1String>
#include <QCoreApplication>
#include <QObject>
#include <cassert>

using namespace Mdt::DeployUtils;

DeployUtilsMain::DeployUtilsMain(QObject* parent)
 : AbstractConsoleApplicationMainFunction(parent)
{
  QCoreApplication::setApplicationName( QLatin1String("mdtdeployutils") );
//   QCoreApplication::setApplicationVersion("0.0.1");
}

int DeployUtilsMain::runMain()
{
  MessageLogger messageLogger;
  CommandLineParser commandLineParser;

  try{
    commandLineParser.process( QCoreApplication::arguments() );
  }catch(const CommandLineParseError & error){
    showError(error);
    return 1;
  }catch(const std::exception & error){
    showError(error);
    return 1;
  }

  if( commandLineParser.messageLoggerBackend() == MessageLoggerBackend::CMake ){
    MessageLogger::setBackend<CMakeStyleMessageLogger>();
  }

//   assert( commandLineParser.processedCommand() != CommandLineCommand::Unknown );

  switch( commandLineParser.processedCommand() ){
    case CommandLineCommand::CopySharedLibrariesTargetDependsOn:
      copySharedLibrariesTargetDependsOn(commandLineParser);
      break;
    case CommandLineCommand::GetSharedLibrariesTargetDependsOn:
      /// \todo to implement
      break;
    case CommandLineCommand::DeployApplication:
      deployApplication(commandLineParser);
      break;
    case CommandLineCommand::Unknown:
      // Maybe just Bash completion
      return 0;
  }

  return 0;
}

void DeployUtilsMain::copySharedLibrariesTargetDependsOn(const CommandLineParser & commandLineParser)
{
  assert( commandLineParser.processedCommand() == CommandLineCommand::CopySharedLibrariesTargetDependsOn );

  const auto request = commandLineParser.copySharedLibrariesTargetDependsOnRequest();

  CopySharedLibrariesTargetDependsOn csltdo;

  const LogLevel logLevel = commandLineParser.logLevel();
  if( shouldOutputStatusMessages(logLevel) ){
    QObject::connect(&csltdo, &CopySharedLibrariesTargetDependsOn::statusMessage, MessageLogger::info);
  }
  if( shouldOutputVerboseMessages(logLevel) ){
    QObject::connect(&csltdo, &CopySharedLibrariesTargetDependsOn::verboseMessage, MessageLogger::info);
  }
  if( shouldOutputDebugMessages(logLevel) ){
    QObject::connect(&csltdo, &CopySharedLibrariesTargetDependsOn::debugMessage, MessageLogger::info);
  }

  csltdo.execute(request);
}

void DeployUtilsMain::deployApplication(const CommandLineParser & commandLineParser)
{
  assert( commandLineParser.processedCommand() == CommandLineCommand::DeployApplication );

  DeployApplication useCase;

  const DeployApplicationRequest request = commandLineParser.deployApplicationRequest();

  const LogLevel logLevel = commandLineParser.logLevel();
  if( shouldOutputStatusMessages(logLevel) ){
    QObject::connect(&useCase, &DeployApplication::statusMessage, MessageLogger::info);
  }
  if( shouldOutputVerboseMessages(logLevel) ){
    QObject::connect(&useCase, &DeployApplication::verboseMessage, MessageLogger::info);
  }
  if( shouldOutputDebugMessages(logLevel) ){
    QObject::connect(&useCase, &DeployApplication::debugMessage, MessageLogger::info);
  }

  useCase.execute(request);
}
