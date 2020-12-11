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
#ifndef MDT_COMMAND_LINE_PARSER_H
#define MDT_COMMAND_LINE_PARSER_H

#include "Mdt/CommandLineParser/ParserDefinition.h"
#include <QObject>
#include <QCommandLineParser>
#include <QStringList>

/*! \brief \todo document
 */
struct CommandLineParserResult
{
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

  /*! \brief Process arguments given to the application
   */
  CommandLineParserResult process();

  /*! \brief Access the internal parser
   */
  const QCommandLineParser & parser() const noexcept
  {
    return mParser;
  }

 private:

  void setApplicationDescription();
  void addCommandArgument();
  void addGetSharedLibrariesTargetDependsOnCommand();
  void addCopySharedLibrariesTargetDependsOnCommand();
  CommandLineParserResult processGetSharedLibrariesTargetDependsOn(const QStringList & arguments);
  CommandLineParserResult processCopySharedLibrariesTargetDependsOn(const QStringList & arguments);

  Mdt::CommandLineParser::ParserDefinition mParserDefinition;
  QCommandLineParser mParser;
};

#endif // #ifndef MDT_COMMAND_LINE_PARSER_H
