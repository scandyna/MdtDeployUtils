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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_CASE_STATEMENT_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_CASE_STATEMENT_H

#include "ScriptCaseClause.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <vector>
#include <cstddef>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief The expression of a ScriptCaseStatement
   */
  class MDT_COMMANDLINEPARSER_EXPORT ScriptCaseStatementExpression
  {
   public:

    ScriptCaseStatementExpression() = delete;

    /*! \brief Construct a expression
     *
     * \pre \a expression must not be empty
     */
    explicit ScriptCaseStatementExpression(const QString & expression) noexcept
     : mExpression(expression)
    {
      assert( !expression.isEmpty() );
    }

    /*! \brief Get the string representation of this expression
     */
    QString toString() const noexcept
    {
      return mExpression;
    }

   private:

    QString mExpression;
  };

  /*! \brief A case statement in a Bash completion script
   *
   * \todo Should check that adding a clause with
   *  a pattern that allready exists is prohibited
   */
  class MDT_COMMANDLINEPARSER_EXPORT ScriptCaseStatement
  {
   public:

    ScriptCaseStatement() = delete;

    /*! \brief Construct a case statement
     */
    explicit ScriptCaseStatement(const ScriptCaseStatementExpression & caseStatementExpression) noexcept
     : mCaseStatementExpression( caseStatementExpression.toString() )
    {
    }

    /*! \brief Add a clause to this script
     */
    void addClause(const ScriptCaseClause & clause) noexcept
    {
      mClauses.push_back(clause);
    }

    /*! \brief Add a clause to this statement
     *
     * \pre \a pattern and \a command must be valid
     * \sa ScriptCaseClause::isValidPattern()
     * \sa ScriptCaseClause::isValidCommand()
     */
    void addClause(const QString & pattern, const QString & command) noexcept
    {
      assert( ScriptCaseClause::isValidPattern(pattern) );
      assert( ScriptCaseClause::isValidCommand(command) );

      mClauses.emplace_back(pattern, command);
    }

    /*! \brief Add a default clause to this statement
     */
    void addDefaultClause() noexcept
    {
      mClauses.emplace_back();
    }

    /*! \brief Add a default clause with a command to this statement
     *
     * \pre \a command must be valid
     * \sa ScriptCaseClause::isValidCommand()
     */
    void addDefaultClause(const QString & command) noexcept
    {
      assert( ScriptCaseClause::isValidCommand(command) );

      mClauses.emplace_back(command);
    }

    /*! \brief Get the count of clauses in this statement
     */
    int clauseCount() const noexcept
    {
      return static_cast<int>( mClauses.size() );
    }

    /*! \brief Check if this statement is empty
     *
     * Returns true unless this statement has at least 1 clause
     */
    bool isEmpty() const noexcept
    {
      return mClauses.empty();
    }

    /*! \brief Get the clause at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < clauseCount() )
     */
    const ScriptCaseClause & clauseAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < clauseCount() );

      return mClauses[static_cast<size_t>(index)];
    }

    /*! \brief Get the string representation of this statement
     */
    QString toString() const noexcept;

   private:

    QString mCaseStatementExpression;
    std::vector<ScriptCaseClause> mClauses;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_CASE_STATEMENT_H
