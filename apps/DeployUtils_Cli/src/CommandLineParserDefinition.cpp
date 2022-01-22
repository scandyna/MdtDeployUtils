/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
 ***********************************************************************************************/
#include "CommandLineParserDefinition.h"
#include "CommandLineCommand.h"
#include <QLatin1String>

using namespace Mdt::CommandLineParser;

CommandLineParserDefinition::CommandLineParserDefinition(QObject *parent) noexcept
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

  const QString logLevelDescription = tr(
    "Set the log level.\n"
    "The log level is inspired by CMake. Available levels are:\n"
    "- ERROR: only output error messages\n"
    "- WARNING: the same as STATUS\n"
    "- NOTICE: the same as STATUS\n"
    "- STATUS (the default): output the main interesting messages that project users might be interested in,"
    " and higher level messages (like errors)\n"
    "- VERBOSE: output detailed informational messages intended for project users,"
    " and higher level messages (those defined by the STATUS level)\n"
    "- DEBUG: Detailed informational messages intended for developers working on the project itself,"
    " and higher level messages (those defined by the VERBOSE level)\n"
    "- TRACE: the same as DEBUG"
  );
  ParserDefinitionOption logLevelOption( QLatin1String("log-level"), logLevelDescription );
  logLevelOption.setValueName( QLatin1String("level") );
  logLevelOption.setPossibleValues({QLatin1String("STATUS"),QLatin1String("VERBOSE"),QLatin1String("DEBUG")});
  mParserDefinition.addOption(logLevelOption);

  addGetSharedLibrariesTargetDependsOnCommand();

  mCopySharedLibrariesTargetDependsOnDefinition.setApplicationName( mParserDefinition.applicationName() );
  mCopySharedLibrariesTargetDependsOnDefinition.setup();
  mParserDefinition.addSubCommand( mCopySharedLibrariesTargetDependsOnDefinition.command() );

  addDeployApplicationCommand();
}

QString CommandLineParserDefinition::getGetSharedLibrariesTargetDependsOnHelpText() const noexcept
{
  return mParserDefinition.getSubCommandHelpText( commandName(CommandLineCommand::GetSharedLibrariesTargetDependsOn) );
}

QString CommandLineParserDefinition::getCopySharedLibrariesTargetDependsOnHelpText() const noexcept
{
  return mParserDefinition.getSubCommandHelpText( commandName(CommandLineCommand::CopySharedLibrariesTargetDependsOn) );
}

QString CommandLineParserDefinition::getDeployApplicationHelpText() const noexcept
{
  return mParserDefinition.getSubCommandHelpText( commandName(CommandLineCommand::DeployApplication) );
}

void CommandLineParserDefinition::setApplicationDescription()
{
  const QString description = tr(
    "\nHelper tool to deploy binary targets and its dependencies, like shared libraries and plugins.\n"
    "This tool is divided in commands (subcommands). Each command has its own positional arguments and options.\n"
    "To get help, pass command --help (or command -h).\n"
    "Example:\n"
    "%1 %2 --help"
  ).arg( mParserDefinition.applicationName(), mCopySharedLibrariesTargetDependsOnDefinition.command().name() );

  mParserDefinition.setApplicationDescription(description);
}

void CommandLineParserDefinition::addGetSharedLibrariesTargetDependsOnCommand()
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

void CommandLineParserDefinition::addDeployApplicationCommand()
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
