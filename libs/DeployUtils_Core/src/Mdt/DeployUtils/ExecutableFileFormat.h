/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_FORMAT_H
#define MDT_DEPLOY_UTILS_EXECUTABLE_FILE_FORMAT_H

namespace Mdt{ namespace DeployUtils{

  /*! \brief Executable file format
   */
  enum class ExecutableFileFormat
  {
    Elf,    /*!< Linux ELF executable */
    Pe,     /*!< Windows PE executable */
    Unknown /*!< Unknown format */
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_FORMAT_H
