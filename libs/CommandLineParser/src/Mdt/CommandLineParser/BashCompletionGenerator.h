/****************************************************************************
 **
 ** Copyright (C) 2011-2021 Philippe Steinmann.
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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_H

#include "BashCompletionGeneratorCommand.h"
#include "BashCompletionScriptFileWriteError.h"
#include "ParserDefinition.h"
#include "mdt_commandlineparser_export.h"
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <string>
#include <vector>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Helper class to generate Bash completion script
   *
   * If you have a parser definition based on ParserDefinition,
   * it can be used to create a bash completion generator:
   * \code
   * using namespace Mdt::CommandLineParser;
   *
   * ParserDefinition parserDefinition;
   * // Setup the parser definition, see Mdt::CommandLineParser::ParserDefinition
   *
   * auto generator = BashCompletionGenerator::fromParserDefinition(parserDefinition);
   * generator.generateScriptToFile("path/to/out/directory/");
   * \endcode
   *
   * It is also possible to define the generator:
   * \code
   * using namespace Mdt::CommandLineParser;
   *
   * BashCompletionGenerator generator;
   * generator.setApplicationName("mytool");
   *
   * BashCompletionGeneratorCommand mainCommand;
   * mainCommand.addPositionalArgument(ArgumentType::File, "source");
   * mainCommand.addOption('h', "help");
   * generator.setMainCommand(mainCommand);
   * \endcode
   *
   * \sa handleBashCompletion()
   * \sa https://iridakos.com/programming/2018/03/01/bash-programmable-completion-tutorial
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionGenerator
  {
    Q_DECLARE_TR_FUNCTIONS(BashCompletionGenerator)

   public:

    /*! \brief Set application name
     *
     * \pre \a name must not be empty
     */
    void setApplicationName(const QString & name);

    /*! \brief Get application name
     */
    const QString & applicationName() const noexcept
    {
      return mApplicationName;
    }

    /*! \brief Set the main command
     *
     * \pre \a command must not be empty
     *
     * \todo Maybe the main command could be empty ?
     */
    void setMainCommand(const BashCompletionGeneratorCommand & command);

    /*! \brief Get the main command
     */
    const BashCompletionGeneratorCommand & mainCommand() const noexcept
    {
      return mMainCommand;
    }

    /*! \brief Add a subcommand
     *
     * \pre \a command must not be empty
     * \pre \a command must have a name
     */
    void addSubCommand(const BashCompletionGeneratorCommand & command);

    /*! \brief Check if this generator has at least 1 subcommand
     */
    bool hasSubCommands() const noexcept
    {
      return !mSubCommands.empty();
    }

    /*! \brief Get subcommands
     */
    const std::vector<BashCompletionGeneratorCommand> & subCommands() const noexcept
    {
      return mSubCommands;
    }

    /*! \brief Generate the script
     *
     * \pre The application name must have been set
     * \sa setApplicationName()
     * \sa setMainCommand()
     */
    QString generateScript() const;

    /*! \brief Generate the script to a file in a directory
     *
     * \pre The application name must have been set
     * \sa setApplicationName()
     * \sa setMainCommand()
     * \pre \a directoryPath must not be empty
     * \warning if a file of the form applicationName-completion.bash exists in the given directory, it will be overwritten
     * \exception BashCompletionScriptFileWriteError
     *
     * \todo Rewrite with QFile or QFileSaver
     */
    void generateScriptToFile(const QString & directoryPath) const;

    /*! \overload
     */
    void generateScriptToFile(const std::string & directoryPath) const;

    /*! \brief Get a Bash completion generator from \a parserDefinition
     */
    static
    BashCompletionGenerator fromParserDefinition(const ParserDefinition & parserDefinition);

  private:

    QString mApplicationName;
    BashCompletionGeneratorCommand mMainCommand;
    std::vector<BashCompletionGeneratorCommand> mSubCommands;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_H
