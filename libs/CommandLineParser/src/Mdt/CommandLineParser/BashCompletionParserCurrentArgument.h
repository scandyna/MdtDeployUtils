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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_CURRENT_ARGUMENT_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_CURRENT_ARGUMENT_H

#include "BashCompletionParserCurrentArgumentType.h"
#include "mdt_commandlineparser_export.h"

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Holds informations about the current argument in a Bash completion
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionParserCurrentArgument
  {
   public:

    /*! \brief Set the current argument type
     */
    void setType(BashCompletionParserCurrentArgumentType type) noexcept
    {
      mType = type;
    }

    /*! \brief Get the current argument type
     */
    BashCompletionParserCurrentArgumentType type() const noexcept
    {
      return mType;
    }

    /*! \brief Set this argument as in the sub-command
     */
    void setInSubCommand(bool set) noexcept
    {
      mIsInSubCommand = set;
    }

    /*! \brief Check if this argument is a sub-command
     */
    bool isInSubCommand() const noexcept
    {
      return mIsInSubCommand;
    }

   private:

    BashCompletionParserCurrentArgumentType mType = BashCompletionParserCurrentArgumentType::Undefined;
    bool mIsInSubCommand = false;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_PARSER_CURRENT_ARGUMENT_H
