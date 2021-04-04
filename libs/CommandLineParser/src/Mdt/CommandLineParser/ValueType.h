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
#ifndef MDT_COMMAND_LINE_PARSER_VALUE_TYPE_H
#define MDT_COMMAND_LINE_PARSER_VALUE_TYPE_H

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Value type of a positional argument or a option value
   *
   * As example, a copy (programm or command of a programm)
   * could accept 2 positional arguments:
   * - source: path to a file
   * - destination: path to a directory
   *
   * Specify the argument's value type could help
   * a parser to enforce what should be passed.
   *
   * The argument type is also used for Bash completion.
   */
  enum class ValueType
  {
    Unspecified,      /*!< The value type is not specified */
    Directory,        /*!< Expect a directory */
    File,             /*!< Expect a file */
    DirectoryOrFile   /*!< Expect a directory or a file */
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_VALUE_TYPE_H
