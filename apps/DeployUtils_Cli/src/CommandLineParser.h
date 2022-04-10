/*******************************************************************************************
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
 ***********************************************************************************************/
#ifndef MDT_COMMAND_LINE_PARSER_H
#define MDT_COMMAND_LINE_PARSER_H

#include "CommandLineCommand.h"
#include "CommandLineParseError.h"
#include "CommandLineParserDefinition.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/ParserDefinitionOption.h"
#include "Mdt/CommandLineParser/ParserDefinitionCommand.h"
#include "Mdt/CommandLineParser/ParserResultCommand.h"
#include "Mdt/DeployUtils/LogLevel.h"
#include "Mdt/DeployUtils/OverwriteBehavior.h"
#include "Mdt/DeployUtils/CompilerLocationRequest.h"
#include "Mdt/DeployUtils/CopySharedLibrariesTargetDependsOnRequest.h"
#include "Mdt/DeployUtils/DeployApplicationRequest.h"
#include <QObject>
#include <QStringList>
#include <QChar>
#include <cassert>

/*! \brief Message logger backend
 *
 * \todo Move to its own file
 */
enum class MessageLoggerBackend
{
  Console,  /*!< Use the ConsoleMessageLogger backend */
  CMake     /*!< Use the CMakeStyleMessageLogger backend */
};


/*! \brief Command line parser for mdtdeployutils
 */
class CommandLineParser : public QObject
{
  Q_OBJECT

 public:

  /*! \brief Construct a command line parser
   */
  explicit CommandLineParser(QObject *parent = nullptr);

  /*! \brief Process given arguments
   *
   * \exception CommandLineParseError
   */
  void process(const QStringList & arguments);

  /*! \brief Get the command that have been processed
   */
  CommandLineCommand processedCommand() const noexcept
  {
    return mCommand;
  }

  /*! \brief Get the choosen message logger backend
   */
  MessageLoggerBackend messageLoggerBackend() const noexcept
  {
    return mMessageLoggerBackend;
  }

  /*! \brief get the choosen log level
   */
  Mdt::DeployUtils::LogLevel logLevel() const noexcept
  {
    return mLogLevel;
  }

  /*! \brief Get the DTO to copy shared libraries a target depends on
   *
   * \pre processedCommand() must be CopySharedLibrariesTargetDependsOn
   */
  Mdt::DeployUtils::CopySharedLibrariesTargetDependsOnRequest copySharedLibrariesTargetDependsOnRequest() const noexcept
  {
    assert( processedCommand() == CommandLineCommand::CopySharedLibrariesTargetDependsOn );

    return mCopySharedLibrariesTargetDependsOnRequest;
  }

  /*! \brief Get the DTO to deploy a application
   *
   * \pre processedCommand() must be DeployApplication
   */
  const Mdt::DeployUtils::DeployApplicationRequest & deployApplicationRequest() const noexcept
  {
    assert( processedCommand() == CommandLineCommand::DeployApplication );

    return mDeployApplicationRequest;
  }

  /*! \brief Get the parser definition
   */
  const Mdt::CommandLineParser::ParserDefinition & parserDefinition() const noexcept
  {
    return mParserDefinition.parserDefinition();
  }

 private:

  static
  void parseOverwriteBehaviour(Mdt::DeployUtils::OverwriteBehavior & overwriteBehavior,
                               const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                               const Mdt::CommandLineParser::ParserDefinitionOption & option);

  static
  QChar parsePathListSeparator(const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                               const Mdt::CommandLineParser::ParserDefinitionOption & option);

  static
  QStringList parseSearchPrefixPathList(const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                                        const Mdt::CommandLineParser::ParserDefinitionOption & option, const QChar & separator);

  static
  Mdt::DeployUtils::CompilerLocationRequest parseCompilerLocation(const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                                                                  const Mdt::CommandLineParser::ParserDefinitionOption & option);

  static
  QString parseSingleValueOption(const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                                 const Mdt::CommandLineParser::ParserDefinitionOption & option);

  static
  void parseRuntimeDestination(QString & destination,
                               const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                               const Mdt::CommandLineParser::ParserDefinitionOption & option);

  static
  void parseLibraryDestination(QString & destination,
                               const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                               const Mdt::CommandLineParser::ParserDefinitionOption & option);

  static
  void parseQtPluginsSet(Mdt::DeployUtils::QtPluginsSet & set,
                         const Mdt::CommandLineParser::ParserResultCommand & resultCommand,
                         const Mdt::CommandLineParser::ParserDefinitionOption & option);

  void processGetSharedLibrariesTargetDependsOn(const Mdt::CommandLineParser::ParserResultCommand & resultCommand);
  void processCopySharedLibrariesTargetDependsOn(const Mdt::CommandLineParser::ParserResultCommand & resultCommand);
  void processDeployApplicationCommand(const Mdt::CommandLineParser::ParserResultCommand & resultCommand);

  CommandLineCommand mCommand = CommandLineCommand::Unknown;
  MessageLoggerBackend mMessageLoggerBackend = MessageLoggerBackend::Console;
  Mdt::DeployUtils::LogLevel mLogLevel = Mdt::DeployUtils::LogLevel::Status;
  Mdt::DeployUtils::CopySharedLibrariesTargetDependsOnRequest mCopySharedLibrariesTargetDependsOnRequest;
  Mdt::DeployUtils::DeployApplicationRequest mDeployApplicationRequest;
  CommandLineParserDefinition mParserDefinition;
};

#endif // #ifndef MDT_COMMAND_LINE_PARSER_H
