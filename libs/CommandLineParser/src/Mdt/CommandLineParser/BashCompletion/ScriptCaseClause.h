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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_CASE_CLAUSE_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_CASE_CLAUSE_H

#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief Represents a Bash case clause
   */
  class MDT_COMMANDLINEPARSER_EXPORT ScriptCaseClause
  {
   public:

    /*! \brief Construct a default clause
     */
    explicit ScriptCaseClause() noexcept
     : mPattern( QLatin1String("*") )
    {
    }

    /*! \brief Construct a default clause with a command
     *
     * \pre \a command must be valid
     * \sa isValidCommand()
     */
    explicit ScriptCaseClause(const QString & command) noexcept
     : mPattern( QLatin1String("*") ),
       mCommand(command)
    {
    }

    /*! \brief Construct a clause with a pattern and a command
     *
     * \pre \a pattern and \a command must be valid
     * \sa isValidPattern()
     * \sa isValidCommand()
     */
    explicit ScriptCaseClause(const QString & pattern, const QString & command) noexcept
     : mPattern(pattern),
       mCommand(command)
    {
      assert( isValidPattern(pattern) );
      assert( isValidCommand(command) );
    }

    /*! \brief Get the pattern of this clause
     */
    const QString & pattern() const noexcept
    {
      return mPattern;
    }

    /*! \brief Check if this clause has a command
     */
    bool hasCommand() const noexcept
    {
      return !mCommand.isEmpty();
    }

    /*! \brief Get the command of this clause
     */
    const QString & command() const noexcept
    {
      return mCommand;
    }

    /*! \brief Get the string representation of this clause
     */
    QString toString() const noexcept;

    /*! \brief Returns true if \a pattern is valid
     *
     * To be valid, \a pattern must:
     * - Not be empty
     * - Not contain any )
     * - Not contain any *
     */
    static
    bool isValidPattern(const QString & pattern) noexcept
    {
      if( pattern.trimmed().isEmpty() ){
        return false;
      }
      if( pattern.contains( QChar::fromLatin1(')') ) ){
        return false;
      }

      return true;
    }

    /*! \brief Returns true if \a command is valid
     */
    static
    bool isValidCommand(const QString & command) noexcept
    {
      return !command.trimmed().isEmpty();
    }

   private:

    QString mPattern;
    QString mCommand;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_CASE_CLAUSE_H
