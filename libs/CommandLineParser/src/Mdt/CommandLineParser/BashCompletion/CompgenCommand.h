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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_COMMAND_H

#include "CompgenAction.h"
#include "CompgenWordList.h"
#include "CompgenArgument.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QStringList>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief Represents a Bash built-in compgen command for completion script generation
   *
   * A compgen command is composed of a list of arguments.
   *
   * Example:
   * \code
   * CompgenCommand command;
   * command.addAction(CompgenAction::ListFiles);
   * command.addWordList({"-h","--help","copy","cut"});
   * \endcode
   * the compgen command of above example will be:
   * \code
   * compgen -A file -W "-h --help copy cut"
   * \endcode
   *
   * \sa https://www.gnu.org/software/bash/manual/html_node/Programmable-Completion-Builtins.html
   */
  class MDT_COMMANDLINEPARSER_EXPORT CompgenCommand
  {
   public:

    /*! \brief Add a action to this command
     */
    void addAction(CompgenAction action) noexcept
    {
      mArgumentList.emplace_back(action);
    }

    /*! \brief Add a list of words
     */
    void addWordList(const QStringList & wordList) noexcept
    {
      mArgumentList.emplace_back( CompgenWordList{wordList} );
    }

    /*! \brief Returns the string representation of this command
     *
     * The \a word will be in the passed as \a word to the compgen command:
     * \code
     * const QString commandString = command.toCompgenCommandString("$cur");
     * \endcode
     * will return:
     * \code
     * "compgen .... -- "$cur""
     * \endcode
     */
    QString toCompgenCommandString(const QString & word) const noexcept;

   private:

    CompgenArgumentList mArgumentList;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_COMPGEN_COMMAND_H
