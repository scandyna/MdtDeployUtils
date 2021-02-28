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
#ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_COMMAND_LINE_ALGORITHM_H
#define MDT_COMMAND_LINE_PARSER_COMMAND_LINE_COMMAND_LINE_ALGORITHM_H

#include "CommandLine.h"
#include "Algorithm.h"

namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

  /*! \brief Find the sub-command argument in \a commandLine
   */
  inline
  ArgumentList::const_iterator findSubCommandNameArgument(const CommandLine & commandLine) noexcept
  {
    return findSubCommandNameArgument( commandLine.argumentList() );
  }

  /*! \brief Find the sub-command argument index in \a commandLine
   *
   * Returns the index where the sub-command is located,
   * or a value < 0 if no sub-command name exists.
   */
  inline
  int findSubCommandNameArgumentIndex(const CommandLine & commandLine) noexcept
  {
    return findSubCommandNameArgumentIndex( commandLine.argumentList() );
  }

  /*! \brief Find the sub-command name in \a commandLine
   *
   * Returns the name of the sub-command if it exists in \a commandLine ,
   * otherwise a empty string
   */
  inline
  QString findSubCommandName(const CommandLine & commandLine) noexcept
  {
    return findSubCommandName( commandLine.argumentList() );
  }

}}} // namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_COMMAND_LINE_ALGORITHM_H
