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
#include "Mdt/DeployUtils/CopySharedLibrariesTargetDependsOnRequest.h"
#include <QObject>
#include <QStringList>
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

  /*! \brief Get the DTO to copy shared libraries a target depends on
   *
   * \pre processedCommand() must be CopySharedLibrariesTargetDependsOn
   */
  Mdt::DeployUtils::CopySharedLibrariesTargetDependsOnRequest copySharedLibrariesTargetDependsOnRequest() const noexcept
  {
    assert( processedCommand() == CommandLineCommand::CopySharedLibrariesTargetDependsOn );

    return mCopySharedLibrariesTargetDependsOnRequest;
  }

  /*! \brief Get the parser definition
   */
  const Mdt::CommandLineParser::ParserDefinition & parserDefinition() const noexcept
  {
    return mParserDefinition.parserDefinition();
  }

 private:

  void processGetSharedLibrariesTargetDependsOn(const Mdt::CommandLineParser::ParserResultCommand & resultCommand);
  void processCopySharedLibrariesTargetDependsOn(const Mdt::CommandLineParser::ParserResultCommand & resultCommand);
  void processCopySharedLibrariesTargetDependsOnCompilerLocation(const QStringList & compilerLocationValues);
  void processDeployApplicationCommand(const Mdt::CommandLineParser::ParserResultCommand & resultCommand);

  CommandLineCommand mCommand = CommandLineCommand::Unknown;
  MessageLoggerBackend mMessageLoggerBackend = MessageLoggerBackend::Console;
  Mdt::DeployUtils::CopySharedLibrariesTargetDependsOnRequest mCopySharedLibrariesTargetDependsOnRequest;
  CommandLineParserDefinition mParserDefinition;
};

#endif // #ifndef MDT_COMMAND_LINE_PARSER_H
