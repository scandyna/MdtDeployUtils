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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_H

#include "BashCompletionGeneratorCommand.h"
#include "BashCompletionScriptFileWriteError.h"
#include "mdt_commandlineparser_export.h"
#include <string>
#include <vector>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Helper class to generate Bash completion script
   *
   * Example:
   * \code
   * BashCompletionGenerator generator;
   * generator.setApplicationName("mytool");
   *
   * BashCompletionGeneratorCommand mainCommand;
   * mainCommand.addArgument("source");
   * mainCommand.setDirectoryCompletionEnabled(true);
   * mainCommand.addOption('h', "help");
   * generator.setMainCommand(mainCommand);
   * \endcode
   *
   * \sa https://iridakos.com/programming/2018/03/01/bash-programmable-completion-tutorial
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionGenerator
  {
  public:

    /*! \brief Set application name
     *
     * \pre \a name must not be empty
     */
    void setApplicationName(const std::string & name);

    /*! \brief Get application name
     */
    const std::string & applicationName() const noexcept
    {
      return mApplicationName;
    }

    /*! \brief Set the main command
     *
     * \pre \a command must not be empty
     * \todo Maybe the main command could be empty ?
     */
    void setMainCommand(const BashCompletionGeneratorCommand & command);

    /*! \brief Add a subcommand
     *
     * \pre \a command must not be empty
     * \pre \a command must have a name
     */
    void addSubCommand(const BashCompletionGeneratorCommand & command);

    /*! \brief Generate the script
     *
     * \pre The application name must have been set
     * \pre At least the main command must have been set
     * \sa setApplicationName()
     * \sa setMainCommand()
     */
    std::string generateScript() const;

    /*! \brief Generate the script to a file in a directory
     *
     * \pre The application name must have been set
     * \pre At least the main command must have been set
     * \sa setApplicationName()
     * \sa setMainCommand()
     * \pre \a directoryPath must not be empty
     * \warning if a file of the form applicationName-completion.bash exists in the given directory, it will be overwritten
     * \exception BashCompletionScriptFileWriteError
     */
    void generateScriptToFile(const std::string & directoryPath) const;

  private:

    std::string mApplicationName;
    BashCompletionGeneratorCommand mMainCommand;
    std::vector<BashCompletionGeneratorCommand> mSubCommands;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_H
