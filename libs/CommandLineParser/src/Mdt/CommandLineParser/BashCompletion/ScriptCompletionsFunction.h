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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_COMPLETIONS_FUNCTION_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_COMPLETIONS_FUNCTION_H

#include "ScriptCompletionsFunctionVariable.h"
#include "ScriptCaseStatement.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QChar>
#include <vector>
#include <cstddef>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief The completions function in a Bash completion script
   */
  class MDT_COMMANDLINEPARSER_EXPORT ScriptCompletionsFunction
  {
   public:

    ScriptCompletionsFunction() = delete;

    /*! \brief Construct a function for given \a applicationName
     *
     * \pre \a applicationName must not be empty
     */
    explicit ScriptCompletionsFunction(const QString & applicationName,
                                       const ScriptCaseStatementExpression & caseStatementExpression) noexcept;

    /*! \brief Get the name of this function
     */
    QString name() const noexcept
    {
      return mName;
    }

    /*! \brief Add a variable to this function
     *
     * \pre \a name must not be empty
     * \pre \a initializationExpression must not be empty
     * \pre \a initializationExpression must not start with a \a =
     */
    void addVariable(const QString & name, const QString & initializationExpression) noexcept
    {
      assert( !name.isEmpty() );
      assert( !initializationExpression.isEmpty() );
      assert( !initializationExpression.trimmed().startsWith( QChar::fromLatin1('=') ) );

      mVariables.emplace_back(name, initializationExpression);
    }

    /*! \brief Get the count of variables in this function
     */
    int variableCount() const noexcept
    {
      return static_cast<int>( mVariables.size() );
    }

    /*! \brief Get the variable at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < variableCount() )
     */
    const ScriptCompletionsFunctionVariable & variableAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < variableCount() );

      return mVariables[static_cast<size_t>(index)];
    }

    /*! \brief Add a clause to this script
     */
    void addClause(const ScriptCaseClause & clause) noexcept
    {
      mCaseStatement.addClause(clause);
    }

    /*! \brief Add a clause to this function
     *
     * \pre \a pattern and \a command must be valid
     * \sa ScriptCaseClause::isValidPattern()
     * \sa ScriptCaseClause::isValidCommand()
     */
    void addClause(const QString & pattern, const QString & command) noexcept
    {
      assert( ScriptCaseClause::isValidPattern(pattern) );
      assert( ScriptCaseClause::isValidCommand(command) );

      mCaseStatement.addClause(pattern, command);
    }

    /*! \brief Add a default clause to this function
     */
    void addDefaultClause() noexcept
    {
      mCaseStatement.addDefaultClause();
    }

    /*! \brief Get the count of clauses in this function
     */
    int clauseCount() const noexcept
    {
      return mCaseStatement.clauseCount();
    }

    /*! \brief Get the clause at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < clauseCount() )
     */
    const ScriptCaseClause & clauseAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < clauseCount() );

      return mCaseStatement.clauseAt(index);
    }

    /*! \brief Get the variables bloc string representation
     */
    QString getVariablesBlockString() const noexcept;

    /*! \brief Get the string representation of this function
     */
    QString toString() const noexcept;

   private:

    QString mName;
    std::vector<ScriptCompletionsFunctionVariable> mVariables;
    ScriptCaseStatement mCaseStatement;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_COMPLETIONS_FUNCTION_H
