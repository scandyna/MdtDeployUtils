/****************************************************************************
 **
 ** Copyright (C) 2011-2020 Philippe Steinmann.
 **
 ** This file is part of MdtApplication library.
 **
 ** MdtApplication is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** MdtApplication is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with MdtApplication.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_H

#include "ParserDefinitionCommand.h"
#include "ParserDefinitionOption.h"
#include "mdt_commandlineparser_export.h"
#include <QCoreApplication>
#include <QString>
#include <vector>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Helper class to define command line parsers
   *
   * \todo Should the usage without QCoreApplication be supported ?
   *
   * Example of a parser definition:
   * \code
   * Mdt::CommandLineParser::ParserDefinition parserDefinition;
   * parserDefinition.setApplicationDescription("Copy utility");
   * parserDefinition.addHelpOption();
   * parserDefinition.addPositionalArgument("source", "Source file to copy");
   * parserDefinition.addPositionalArgument("destination", "Destination directory");
   * parserDefinition.addOption('f', "force", "Overwrite existing files.");
   * \endcode
   *
   * Example of a parser definition that has subcommands:
   * \code
   * using namespace Mdt::CommandLineParser;
   *
   * ParserDefinition parserDefinition;
   * parserDefinition.setApplicationDescription("C++ package manager");
   * parserDefinition.addHelpOption();
   *
   * ParserDefinitionCommand installCommand("install");
   * installCommand.setDescription("Installs the requirements specified in a recipe");
   * installCommand.addPositionalArgument("reference", "Reference of the for user/channel");
   * installCommand.addHelpOption();
   *
   * ParserDefinitionOption installCommandBuildOption('b', "build");
   * installCommandBuildOption.setValueName("BUILD");
   * installCommandBuildOption.setDefaultValue("never");
   * installCommandBuildOption.setDescription("Choose if you want to build from sources.");
   * installCommand.addOption(installCommandBuildOption);
   *
   * parserDefinition.addSubCommand(installCommand);
   * \endcode
   *
   * To parse command line arguments, QCommandLineParser is a helpful tool.
   *
   * QCommandLineParser has no natural notion of sub-commands.
   * It is possible to create such sub-commands using a QCommandLineParser for each one.
   * A drawback is that the QCommandLineParser::helpText() will return the application name,
   * but the sub-command name is expected.
   *
   * Once QCommandLineParser has been set-up, the defined arguments and options cannot be get:
   * \code
   * QCommandLineParser parser;
   * parser.addPositionalArgument("source", QCoreApplication::translate("main", "Source file to copy."));
   * parser.addPositionalArgument("destination", QCoreApplication::translate("main", "Destination directory."));
   *
   * qDebug() << parser.positionalArguments();
   * \endcode
   *
   * Above code will not work, because we did not call %parse() or %process().
   * The returned arguments are those that are passed to the command line
   * and that are not recognized as options,
   * which is expected for parsing.
   *
   * For some cases, getting the arguments and options given can be useful,
   * for exaple to list available sub-commands in the help,
   * or to generate a Bash-completion script.
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserDefinition
  {
    Q_DECLARE_TR_FUNCTIONS(ParserDefinition)

   public:

    /*! \brief Set the application description
     */
    void setApplicationDescription(const QString &description);

    /*! \brief Get the application description
     */
    QString applicationDescription() const
    {
      return mApplicationDescription;
    }

    /*! \brief Add a option
     *
     * \todo precontidions ?
     */
    void addOption(const ParserDefinitionOption & option);

    /*! \brief Add a option
     *
     * \pre \a name must not be empty
     * \pre \a name must not begin with a dash or a slash
     * \pre \a name must not contain any equal
     *
     * \todo enforce those precontidions
     */
    void addOption(const QString & name, const QString & description);

    /*! \brief Add a option
     *
     * \pre \a shortName must be a letter [a-z]
     * \pre \a name must not be empty
     * \pre \a name must not begin with a dash or a slash
     * \pre \a name must not contain any equal
     *
     * \todo enforce those precontidions
     */
    void addOption(char shortName, const QString & name, const QString & description);

    /*! \brief Add the help option
     */
    void addHelpOption();

    /*! \brief Add a positional argument
     *
     * \pre \a name must not be empty
     */
    void addPositionalArgument(const QString & name, const QString & description, const QString & sytax = QString());

    /*! \brief Get the main command
     */
    const ParserDefinitionCommand & mainCommand() const noexcept
    {
      return mMainCommand;
    }

    /*! \brief Add a subcommand
     *
     * \pre \a command must not be empty
     * \pre \a command must have a name
     */
    void addSubCommand(const ParserDefinitionCommand & command);

    /*! \brief Get subcommands
     */
    const std::vector<ParserDefinitionCommand> & subCommands() const noexcept
    {
      return mSubCommands;
    }

    /*! \brief Get the help text
     */
    QString getHelpText() const;

   private:

    bool includeOptionsInHelpText() const noexcept
    {
      return true;
    }

    QString getUsageText() const;

    QString mApplicationDescription;
    ParserDefinitionCommand mMainCommand;
    std::vector<ParserDefinitionCommand> mSubCommands;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_H
