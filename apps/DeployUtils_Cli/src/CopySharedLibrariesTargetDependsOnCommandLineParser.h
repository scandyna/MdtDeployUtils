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
#ifndef COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_COMMAND_LINE_PARSER_H
#define COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_COMMAND_LINE_PARSER_H

#include <QObject>
#include <QCommandLineParser>
#include <QString>
#include <QStringList>

/*! \brief Command line parser for copy-shared-libraries-target-depends-on subcommand
 */
class CopySharedLibrariesTargetDependsOnCommandLineParser : public QObject
{
  Q_OBJECT

 public:

  /*! \brief Construct a command line parser
   */
  explicit CopySharedLibrariesTargetDependsOnCommandLineParser(QObject *parent = nullptr);

  /*! \brief Process arguments given to the command
   */
  void process(const QStringList & arguments);

 private:

  void setApplicationDescription();

  /*
   * QCommandLineParser's showHelp() and helpText() cannot be used
   * because they show the form:
   * Usage: mdtdeployutils [options]
   *
   * Internally, the application name given by QCoreApplication is used.
   */
  void showHelp();

  static
  QString commandName();

  static
  QString optionsHelpText();

  static
  QString argumentsHelpText();

  QCommandLineParser mParser;
};

#endif // #ifndef COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_COMMAND_LINE_PARSER_H
