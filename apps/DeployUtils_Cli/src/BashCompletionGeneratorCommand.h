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
#ifndef BASH_COMPLETION_GENERATOR_COMMAND_H
#define BASH_COMPLETION_GENERATOR_COMMAND_H

#include "BashCompletionGeneratorOption.h"
#include <string>
#include <vector>
#include <cassert>

/*! \brief Command for BashCompletionGenerator
 *
 * A command holds a list of arguments and options
 */
class BashCompletionGeneratorCommand
{
 public:

  BashCompletionGeneratorCommand() = delete;

  /*! \brief Construct a command
   *
   * \pre \a name must not be empty
   */
  BashCompletionGeneratorCommand(const std::string & name)
   : mName(name)
  {
    assert( !name.empty() );
  }

  /*! \brief Copy construct a command from \a other
   */
  BashCompletionGeneratorCommand(const BashCompletionGeneratorCommand & other) = default;

  /*! \brief Copy assign \a other to this command
   */
  BashCompletionGeneratorCommand & operator=(BashCompletionGeneratorCommand && other) = default;

  /*! \brief Move construct a command from \a other
   */
  BashCompletionGeneratorCommand(BashCompletionGeneratorCommand && other) = default;

  /*! \brief Move assign \a other to this command
   */
  BashCompletionGeneratorCommand & operator=(const BashCompletionGeneratorCommand & other) = default;

  /*! \brief Get the name of this command
   */
  const std::string & name() const noexcept
  {
    return mName;
  }

  /*! \brief Add a argument to this command
   *
   * \pre \a arg must not be empty
   */
  void addArgument(const std::string & arg)
  {
    assert( !arg.empty() );

    mArguments.push_back(arg);
  }

  /*! \brief Get the list of arguments of this command
   */
  const std::vector<std::string> & arguments() const noexcept
  {
    return mArguments;
  }

  /*! \brief Check if this command is empty
   *
   * Return true if this command has no arguments and no options
   */
  bool isEmpty() const noexcept
  {
    return mArguments.empty();
  }

  /*! \brief Add a command line option
   */
  void addOption(const BashCompletionGeneratorOption & option)
  {
    mOptions.push_back(option);
  }

  /*! \brief Add a command line option
   *
   * \pre \a shortName must be a character [a-z]
   * \pre \a name must not be empty
   * \pre \a name must not begin with a dash or a slash
   * \pre \a name must not contain any equal
   *
   * \todo enforce those precontidions
   */
  void addOption(char shortName, const std::string & name)
  {
    assert( !name.empty() );

    addOption( BashCompletionGeneratorOption(shortName, name) );
  }

  /*! \brief Add a command line option
   *
   * \pre \a name must not be empty
   * \pre \a name must not begin with a dash or a slash
   * \pre \a name must not contain any equal
   *
   * \todo enforce those precontidions
   */
  void addOption(const std::string & name)
  {
    assert( !name.empty() );

    addOption( BashCompletionGeneratorOption(name) );
  }

  /*! \brief Get the list of options of this command
   */
  const std::vector<BashCompletionGeneratorOption> & options() const noexcept
  {
    return mOptions;
  }

  /*! \brief Set the directory completion to \a enable for this command
   */
  void setDirectoryCompletionEnabled(bool enable) noexcept
  {
    mDirectoryCompletionEnabled = enable;
  }

  /*! \brief Check if directory completion is enabled for this command
   */
  bool directoryCompletionEnabled() const noexcept
  {
    return mDirectoryCompletionEnabled;
  }

 private:

  bool mDirectoryCompletionEnabled = false;
  std::string mName;
  std::vector<std::string> mArguments;
  std::vector<BashCompletionGeneratorOption> mOptions;
};

#endif // #ifndef BASH_COMPLETION_GENERATOR_COMMAND_H
