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
#ifndef BASH_COMPLETION_SCRIPT_FILE_WRITE_ERROR_H
#define BASH_COMPLETION_SCRIPT_FILE_WRITE_ERROR_H

#include <stdexcept>
#include <string>

/*! \brief Error thrown by BashCompletionGenerator
 */
class BashCompletionScriptFileWriteError : public std::runtime_error
{
 public:

  /*! \brief Constructor
    */
  explicit BashCompletionScriptFileWriteError(const std::string & what)
    : runtime_error(what)
  {
  }

};

#endif // #ifndef BASH_COMPLETION_SCRIPT_FILE_WRITE_ERROR_H
