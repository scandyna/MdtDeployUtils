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
#ifndef COMMAND_LINE_PARSER_DEFINITION_H
#define COMMAND_LINE_PARSER_DEFINITION_H

#include "CopySharedLibrariesTargetDependsOnCommandLineParserDefinition.h"
#include "Mdt/CommandLineParser/ParserDefinition.h"
#include "Mdt/CommandLineParser/ParserDefinitionOption.h"
#include <QObject>
#include <QString>
#include <cassert>

/*! \brief Command-line parser definition
 */
class CommandLineParserDefinition : public QObject
{
  Q_OBJECT

 public:

  /*! \brief Construct a command line parser
   */
  explicit CommandLineParserDefinition(QObject *parent = nullptr) noexcept;

  /*! \brief Get the helptext for the main command
   */
  QString getHelpText() const noexcept
  {
    return mParserDefinition.getHelpText();
  }

  /*! \brief Get the logger backend option
   */
  const Mdt::CommandLineParser::ParserDefinitionOption & loggerBackendOption() const noexcept
  {
    return mParserDefinition.optionAt(1);
  }

  /*! \brief Get the log-level option
   */
  const Mdt::CommandLineParser::ParserDefinitionOption & logLevelOption() const noexcept
  {
    return mParserDefinition.optionAt(2);
  }

  /*! \brief Get the help text for the "Get Shared Libraries Target Depends On" command
   */
  QString getGetSharedLibrariesTargetDependsOnHelpText() const noexcept;

  /*! \brief Get the help text for the "Copy Shared Libraries Target Depends On" command
   */
  QString getCopySharedLibrariesTargetDependsOnHelpText() const noexcept;

  /*! \brief Get the help text for the "Deploy Application" command
   */
  QString getDeployApplicationHelpText() const noexcept;

  /*! \brief Get the parser definition
   */
  const Mdt::CommandLineParser::ParserDefinition & parserDefinition() const noexcept
  {
    return mParserDefinition;
  }

  /*! \brief Get the "Copy Shared Libraries Target Depends On" command
   */
  const CopySharedLibrariesTargetDependsOnCommandLineParserDefinition & copySharedLibrariesTargetDependsOn() const noexcept
  {
    return mCopySharedLibrariesTargetDependsOnDefinition;
  }

 private:

  void setApplicationDescription();
  void addGetSharedLibrariesTargetDependsOnCommand();
  void addDeployApplicationCommand();

  Mdt::CommandLineParser::ParserDefinition mParserDefinition;
  CopySharedLibrariesTargetDependsOnCommandLineParserDefinition mCopySharedLibrariesTargetDependsOnDefinition;
  Mdt::CommandLineParser::ParserDefinitionCommand mGetSharedLibrariesTargetDependsOnCommand;
  Mdt::CommandLineParser::ParserDefinitionCommand mDeployApplicationCommand;
};

#endif // #ifndef COMMAND_LINE_PARSER_DEFINITION_H
