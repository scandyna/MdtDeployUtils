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
#ifndef BASH_COMPLETION_GENERATOR_H
#define BASH_COMPLETION_GENERATOR_H

#include "BashCompletionGeneratorCommand.h"
#include <string>
#include <vector>
#include <memory>

/*! \brief Helper class to generate Bash completion script
 *
 * Example:
 * \code
 * BashCompletionGenerator generator;
 * generator.setApplicationName("mytool");
 *
 * BashCompletionGeneratorCommand mainCommand("main-command");
 * mainCommand.addArgument("source");
 * mainCommand.setDirectoryCompletionEnabled(true);
 * mainCommand.addOption('h', "help");
 * generator.setMainCommand(mainCommand);
 * \endcode
 *
 * \sa https://iridakos.com/programming/2018/03/01/bash-programmable-completion-tutorial
 */
class BashCompletionGenerator
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
   */
  void setMainCommand(const BashCompletionGeneratorCommand & command);

  /*! \brief Add a subcommand
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

 private:

  static
  void addCommandOptionsToWordList(const BashCompletionGeneratorCommand & command, std::vector<std::string> & wordList);

  static
  void addCommandArgumentsAndOptionsToWordList(const BashCompletionGeneratorCommand & command, std::vector<std::string> & wordList);

  /// \todo should restrict level range
  static
  std::string generateCompreplyUsingCompgenWithWordList(int level, const BashCompletionGeneratorCommand & command);

  static
  std::string generateAddCompreplyUsingCompgenForDirectoryCompletionIfEnabled(const BashCompletionGeneratorCommand & command);

  /// \todo should restrict level range
  static
  std::string generateCommandBlock(int level, const BashCompletionGeneratorCommand & command, const std::string & comment);

  static
  std::string generateMainCommandBlock(const BashCompletionGeneratorCommand & command);

  static
  std::string generateSubCommandBlock(const BashCompletionGeneratorCommand & command);

  static
  std::string generateSubCommandBlocksIfAny(const std::vector<BashCompletionGeneratorCommand> & commands);

  std::string mApplicationName;
  std::unique_ptr<BashCompletionGeneratorCommand> mMainCommand;
  std::vector<BashCompletionGeneratorCommand> mSubCommands;
};

#endif // #ifndef BASH_COMPLETION_GENERATOR_H
