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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_COMPLETIONS_FUNCTION_VARIABLE_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_COMPLETIONS_FUNCTION_VARIABLE_H

#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1Char>
#include <QLatin1String>
#include <QStringBuilder>
#include <QChar>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

  /*! \brief A variable for a ScriptCompletionsFunction
   */
  class MDT_COMMANDLINEPARSER_EXPORT ScriptCompletionsFunctionVariable
  {
   public:

    ScriptCompletionsFunctionVariable() = delete;

    /*! \brief Construct a variable
     *
     * \pre \a name must not be empty
     * \pre \a initializationExpression must not be empty
     * \pre \a initializationExpression must not start with a \a =
     */
    explicit ScriptCompletionsFunctionVariable(const QString & name, const QString & initializationExpression) noexcept
     : mName(name),
       mInitializationExpression(initializationExpression)
    {
      assert( !name.isEmpty() );
      assert( !initializationExpression.isEmpty() );
      assert( !initializationExpression.trimmed().startsWith( QChar::fromLatin1('=') ) );
    }

    /*! \brief Get the name of this variable
     */
    QString name() const noexcept
    {
      return mName;
    }

    /*! \brief Get the string representation of this variable
     */
    QString toString() const noexcept
    {
      return QLatin1String("  local ") % mName % QLatin1Char('=') % mInitializationExpression;
    }

   private:

    QString mName;
    QString mInitializationExpression;
  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_SCRIPT_COMPLETIONS_FUNCTION_VARIABLE_H
