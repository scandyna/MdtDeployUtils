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
#ifndef BASH_COMPLETION_GENERATOR_OPTION_H
#define BASH_COMPLETION_GENERATOR_OPTION_H

#include <string>
#include <cassert>

/*! \brief Command line option for BashCompletionGenerator
 */
class BashCompletionGeneratorOption
{
 public:

  /*! \brief Construct a command line option
   *
   * \pre \a shortName must be a character [a-z]
   * \pre \a name must not be empty
   * \pre \a name must not begin with a dash or a slash
   * \pre \a name must not contain any equal
   *
   * \todo enforce those precontidions
   */
  explicit BashCompletionGeneratorOption(char shortName, const std::string & name)
   : mShortName(shortName),
     mName(name)
  {
    assert( !name.empty() );
  }

  /*! \brief Construct a command line option
   *
   * \pre \a name must not be empty
   * \pre \a name must not begin with a dash or a slash
   * \pre \a name must not contain any equal
   *
   * \todo enforce those precontidions
   */
  explicit BashCompletionGeneratorOption(const std::string & name)
   : mShortName('\0'),
     mName(name)
  {
    assert( !name.empty() );
  }

  /*! \brief Check if this command line option has a short name
   */
  bool hasShortName() const noexcept
  {
    return mShortName != '\0';
  }

  /*! \brief Get the short name with a dash
   *
   * \pre must only be called if this command line option has a short name
   * \sa hasShortName()
   */
  std::string shortNameWithDash() const
  {
    assert( hasShortName() );

    return std::string("-") + mShortName;
  }

  /*! \brief Get the name with dashes
   */
  std::string nameWithDashes() const
  {
    return std::string("--") + mName;
  }

 private:

  char mShortName;
  std::string mName;
};

#endif // #ifndef BASH_COMPLETION_GENERATOR_OPTION_H
