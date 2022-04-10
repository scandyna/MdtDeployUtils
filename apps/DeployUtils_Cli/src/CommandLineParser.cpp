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
// #include <iostream>

using namespace Mdt::CommandLineParser;
using namespace Mdt::DeployUtils;

CommandLineParser::CommandLineParser(QObject *parent)
 : QObject(parent)
{
}

void CommandLineParser::process(const QStringList & arguments)
{
//   std::cout << "cmd args: " << arguments.join(QLatin1Char(' ')).toStdString() << std::endl;

  Parser parser( mParserDefinition.parserDefinition() );
  parser.parse(arguments);

  if( handleBashCompletion( parser.commandLine(), mParserDefinition.parserDefinition() ) ){
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

  const QStringList loggerBackendOptionValues = parserResult.getValues( mParserDefinition.loggerBackendOption() );
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

  const QStringList logLevelOptionValues = parserResult.getValues( mParserDefinition.logLevelOption() );
  if( !logLevelOptionValues.isEmpty() ){
    if( logLevelOptionValues.count() > 1 ){
      const QString message = tr("log-level option given more than once");
      throw CommandLineParseError(message);
    }
    const QString level = logLevelOptionValues.at(0);
    if( level == QLatin1String("ERROR") ){
      mLogLevel = LogLevel::Error;
    }else if( level == QLatin1String("WARNING") ){
      mLogLevel = LogLevel::Status;
    }else if( level == QLatin1String("NOTICE") ){
      mLogLevel = LogLevel::Status;
    }else if( level == QLatin1String("STATUS") ){
      mLogLevel = LogLevel::Status;
    }else if( level == QLatin1String("VERBOSE") ){
      mLogLevel = LogLevel::Verbose;
    }else if( level == QLatin1String("DEBUG") ){
      mLogLevel = LogLevel::Debug;
    }else if( level == QLatin1String("TRACE") ){
      mLogLevel = LogLevel::Debug;
    }else{
      const QString message = tr("given log level '%1' is not supported").arg(level);
      throw CommandLineParseError(message);
    }
  }
//   if( parserResult.isSet( mParserDefinition.verboseOption() ) ){
//     mVerboseOptionIsSet = true;
//   }

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

void CommandLineParser::parseOverwriteBehaviour(OverwriteBehavior & overwriteBehavior,
                                                const ParserResultCommand & resultCommand,
                                                const ParserDefinitionOption & option)
{
  const QStringList overwriteBehaviorValues = resultCommand.getValues(option);
  if( !overwriteBehaviorValues.isEmpty() ){
    if( overwriteBehaviorValues.count() > 1 ){
      const QString message = tr("%1 option given more than once")
                              .arg( option.name() );
      throw CommandLineParseError(message);
    }
    const QString overwriteBehaviorStr = overwriteBehaviorValues.at(0);
    if( overwriteBehaviorStr == QLatin1String("keep") ){
      overwriteBehavior = OverwriteBehavior::Keep;
    }else if( overwriteBehaviorStr == QLatin1String("overwrite") ){
      overwriteBehavior = OverwriteBehavior::Overwrite;
    }else if( overwriteBehaviorStr == QLatin1String("fail") ){
      overwriteBehavior = OverwriteBehavior::Fail;
    }else{
      const QString message = tr("unknown %1 '%2'")
                              .arg(option.name(),  overwriteBehaviorStr);
      throw CommandLineParseError(message);
    }
  }
}

QChar CommandLineParser::parsePathListSeparator(const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                                                const Mdt::CommandLineParser::ParserDefinitionOption & option)
{
  const QStringList values = resultCommand.getValues(option);
  if( values.isEmpty() ){
    return QLatin1Char(',');
  }
  if( values.count() != 1 ){
    const QString message = tr("%1 option given more than once")
                            .arg( option.name() );
    throw CommandLineParseError(message);
  }

  const QString separatorString = values.at(0);
  if( separatorString.size() != 1 ){
    const QString message = tr("%1 option expects a single char, given %2")
                            .arg(option.name(), separatorString);
    throw CommandLineParseError(message);
  }

  return separatorString.at(0);
}

QStringList CommandLineParser::parseSearchPrefixPathList(const ParserResultCommand & resultCommand,
                                                         const ParserDefinitionOption & option, const QChar & separator)
{
  const QStringList searchPrefixPathListValues = resultCommand.getValues(option);
  if( !searchPrefixPathListValues.isEmpty() ){
    if( searchPrefixPathListValues.count() > 1 ){
      const QString message = tr("%1 option given more than once")
                              .arg( option.name() );
      throw CommandLineParseError(message);
    }
    return searchPrefixPathListValues.at(0).split( separator, QString::SkipEmptyParts );
  }

  return QStringList();
}

CompilerLocationRequest CommandLineParser::parseCompilerLocation(const ParserResultCommand & resultCommand,
                                                                 const ParserDefinitionOption & option)
{
  CompilerLocationRequest location;
  const QStringList optionValues = resultCommand.getValues(option);

  if( optionValues.isEmpty() ){
    return location;
  }

  if( optionValues.count() > 1 ){
    const QString message = tr("%1 option given more than once")
                            .arg( option.name() );
    throw CommandLineParseError(message);
  }

  const QStringList compilerLocationKeyAndValue = optionValues.at(0).split( QLatin1Char('=') );

  if( compilerLocationKeyAndValue.count() != 2 ){
    const QString message = tr("%1: expected type=value syntax, got '%2'")
                            .arg( option.name(),  optionValues.at(0) );
    throw CommandLineParseError(message);
  }

  const QString compilerLocationType = compilerLocationKeyAndValue.at(0);
  QString compilerLocationValue = compilerLocationKeyAndValue.at(1);
  compilerLocationValue.remove( QLatin1Char('"') );

  if( compilerLocationType == QLatin1String("from-env") ){
    location.setType(CompilerLocationType::FromEnv);
    location.setValue(compilerLocationValue);
  }else if( compilerLocationType == QLatin1String("vc-install-dir") ){
    location.setType(CompilerLocationType::VcInstallDir);
    location.setValue(compilerLocationValue);
  }else if( compilerLocationType == QLatin1String("compiler-path") ){
    location.setType(CompilerLocationType::CompilerPath);
    location.setValue(compilerLocationValue);
  }else{
    const QString message = tr("unknown compiler-location type '%1'").arg(compilerLocationType);
    throw CommandLineParseError(message);
  }

  return location;
}

QString CommandLineParser::parseSingleValueOption(const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                                                  const Mdt::CommandLineParser::ParserDefinitionOption & option)
{
  const QStringList optionValues = resultCommand.getValues(option);

  if( optionValues.isEmpty() ){
    return QString();
  }
  assert( optionValues.count() >= 1 );

  if( optionValues.count() > 1 ){
    const QString message = tr("%1 option given more that once").arg( option.name() );
    throw CommandLineParseError(message);
  }
  assert( optionValues.count() == 1 );

  return optionValues.at(0);
}

void CommandLineParser::parseRuntimeDestination(QString & destination,
                                                const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                                                const Mdt::CommandLineParser::ParserDefinitionOption & option)
{
  const QString value = parseSingleValueOption(resultCommand, option);
  if( !value.isEmpty() ){
    destination = value;
  }
}

void CommandLineParser::parseLibraryDestination(QString & destination,
                                                const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                                                const Mdt::CommandLineParser::ParserDefinitionOption & option)
{
  const QString value = parseSingleValueOption(resultCommand, option);
  if( !value.isEmpty() ){
    destination = value;
  }
}

void CommandLineParser::parseQtPluginsSet(Mdt::DeployUtils::QtPluginsSet & set,
                                          const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                                          const Mdt::CommandLineParser::ParserDefinitionOption & option)
{
  const QString setString = parseSingleValueOption(resultCommand, option);
  if( setString.isEmpty() ){
    return;
  }

  const QStringList setItems = setString.split(QLatin1Char('|'), QString::SkipEmptyParts);
  for(const QString & setItem : setItems){
    const QStringList directoryPlugins = setItem.split(QLatin1Char(':'), QString::SkipEmptyParts);
    if( directoryPlugins.size() != 2 ){
      const QString msg = tr(
        "syntax error in --qt-plugins-set option. "
        "each item in the set must be of the form directoryName:name1,nameN. "
        "given: %1"
      ).arg(setItem);
      throw CommandLineParseError(msg);
    }
    const QString directoryName = directoryPlugins.at(0);
    const QStringList pluginsBaseNames = directoryPlugins.at(1).split(QLatin1Char(','), QString::SkipEmptyParts);
    set.set(directoryName, pluginsBaseNames);
  }
}

void CommandLineParser::processGetSharedLibrariesTargetDependsOn(const ParserResultCommand & resultCommand)
{
  if( resultCommand.isHelpOptionSet() ){
    showInfo( mParserDefinition.getGetSharedLibrariesTargetDependsOnHelpText() );
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
    showInfo( mParserDefinition.getCopySharedLibrariesTargetDependsOnHelpText() );
    std::exit(0);
  }

  const CopySharedLibrariesTargetDependsOnCommandLineParserDefinition & definition = mParserDefinition.copySharedLibrariesTargetDependsOn();

  parseOverwriteBehaviour( mCopySharedLibrariesTargetDependsOnRequest.overwriteBehavior, resultCommand, definition.overwriteBehaviorOption() );

  if( resultCommand.isSet( definition.removeRpathOption() ) ){
    mCopySharedLibrariesTargetDependsOnRequest.removeRpath = true;
  }

  const QChar pathListSeparator = parsePathListSeparator( resultCommand, definition.pathListSeparatorOption() );

  mCopySharedLibrariesTargetDependsOnRequest.searchPrefixPathList
   = parseSearchPrefixPathList( resultCommand, definition.searchPrefixPathListOption(), pathListSeparator );

  mCopySharedLibrariesTargetDependsOnRequest.compilerLocation
   = parseCompilerLocation( resultCommand, definition.compilerLocationOption() );

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

void CommandLineParser::processDeployApplicationCommand(const Mdt::CommandLineParser::ParserResultCommand & resultCommand)
{
  mCommand = CommandLineCommand::DeployApplication;

  if( resultCommand.isHelpOptionSet() ){
    showInfo( mParserDefinition.getDeployApplicationHelpText() );
    std::exit(0);
  }

  const DeployApplicationCommandLineParserDefinition & definition = mParserDefinition.deployApplication();

  parseOverwriteBehaviour( mDeployApplicationRequest.shLibOverwriteBehavior, resultCommand, definition.shLibOverwriteBehaviorOption() );

  if( resultCommand.isSet( definition.removeRpathOption() ) ){
    mDeployApplicationRequest.removeRpath = true;
  }

  const QChar pathListSeparator = parsePathListSeparator( resultCommand, definition.pathListSeparatorOption() );

  mDeployApplicationRequest.searchPrefixPathList
   = parseSearchPrefixPathList( resultCommand, definition.searchPrefixPathListOption(), pathListSeparator );

  mDeployApplicationRequest.compilerLocation
   = parseCompilerLocation( resultCommand, definition.compilerLocationOption() );

  parseRuntimeDestination( mDeployApplicationRequest.runtimeDestination, resultCommand, definition.runtimeDestinationOption() );

  parseLibraryDestination( mDeployApplicationRequest.libraryDestination, resultCommand, definition.libraryDestinationOption() );

  parseQtPluginsSet( mDeployApplicationRequest.qtPluginsSet, resultCommand, definition.qtPluginsSetOption() );

  if( resultCommand.positionalArgumentCount() != 2 ){
    const QString message = tr(
      "expected 2 (positional) arguments: target file and destination directory.\n"
      "given: %1"
    ).arg( resultCommand.positionalArguments().join( QLatin1Char(',') ) );
    throw CommandLineParseError(message);
  }

  mDeployApplicationRequest.targetFilePath = resultCommand.positionalArgumentAt(0);
  mDeployApplicationRequest.destinationDirectoryPath = resultCommand.positionalArgumentAt(1);
}
