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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_H

#include "ScriptCompletionsFunction.h"
#include "ScriptCaseClause.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief Helper structure to create a Bash completion script
   */
  class MDT_COMMANDLINEPARSER_EXPORT Script
  {
   public:

    /*! \brief Construct a script for given \a applicationName
     *
     * \pre \a applicationName must not be empty
     */
    explicit Script(const QString & applicationName) noexcept;

    /*! \brief Add a clause to this script
     */
    void addClause(const ScriptCaseClause & clause) noexcept
    {
      mCompletionsFunction.addClause(clause);
    }

    /*! \brief Add a clause to this script
     *
     * \pre \a pattern and \a command must be valid
     * \sa ScriptCaseClause::isValidPattern()
     * \sa ScriptCaseClause::isValidCommand()
     */
    void addClause(const QString & pattern, const QString & command) noexcept
    {
      assert( ScriptCaseClause::isValidPattern(pattern) );
      assert( ScriptCaseClause::isValidCommand(command) );

      mCompletionsFunction.addClause(pattern, command);
    }

    /*! \brief Add a default clause to this script
     */
    void addDefaultClause() noexcept
    {
      mCompletionsFunction.addDefaultClause();
    }

    /*! \brief Get the count of clauses in this script
     */
    int clauseCount() const noexcept
    {
      return mCompletionsFunction.clauseCount();
    }

    /*! \brief Get the clause at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < clauseCount() )
     */
    const ScriptCaseClause & clauseAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < clauseCount() );

      return mCompletionsFunction.clauseAt(index);
    }

    /*! \brief Get the completions function
     */
    const ScriptCompletionsFunction & completionsFunction() const noexcept
    {
      return mCompletionsFunction;
    }

    /*! \brief Get the complete command string
     */
    QString getCompleteCommandString() const noexcept;

    /*! \brief Get the string representation of this script
     */
    QString toString() const noexcept;

   private:

    QString mApplicationName;
    ScriptCompletionsFunction mCompletionsFunction;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_H
