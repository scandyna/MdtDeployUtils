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
#include "ScriptCompletionsFunction.h"
#include "../Algorithm.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

ScriptCompletionsFunction::ScriptCompletionsFunction(const QString& applicationName,
                                                     const ScriptCaseStatementExpression & caseStatementExpression) noexcept
 : mCaseStatement(caseStatementExpression)
{
  assert( !applicationName.isEmpty() );

  mName = QLatin1Char('_') % applicationName % QLatin1String("_completions");
}

QString ScriptCompletionsFunction::getVariablesBlockString() const noexcept
{
  if( mVariables.empty() ){
    return QString();
  }

  const auto extract = [](const ScriptCompletionsFunctionVariable & variable){
    return variable.toString();
  };

  return joinToQString(mVariables, '\n', extract) % QLatin1String("\n\n");
}

QString ScriptCompletionsFunction::toString() const noexcept
{
  return mName % QLatin1String("()\n{\n")
       % getVariablesBlockString()
       % mCaseStatement.toString()
       % QLatin1String("\n}");
}

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{
