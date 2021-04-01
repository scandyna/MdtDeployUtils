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
#include "Script.h"
#include "../BashCompletionParserQuery.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

Script::Script(const QString & applicationName) noexcept
 : mApplicationName(applicationName),
   mCompletionsFunction( applicationName, ScriptCaseStatementExpression( QLatin1String("$currentArgument") ) )
{
  assert( !applicationName.isEmpty() );

  mCompletionsFunction.addVariable( QLatin1String("cur"), QLatin1String("\"${COMP_WORDS[COMP_CWORD]}\"") );
  mCompletionsFunction.addVariable( QLatin1String("executable"), QLatin1String("\"$1\"") );

  const QString queryArgument = BashCompletionParserQuery::findCurrentArgumentString();
  mCompletionsFunction.addVariable( QLatin1String("currentArgument"),
                                    QLatin1String("$(\"$executable\" ") % queryArgument %  QLatin1String(" $COMP_CWORD $COMP_LINE)") );
}

QString Script::getCompleteCommandString() const noexcept
{
  return QLatin1String("complete -F ") % mCompletionsFunction.name() % QLatin1Char(' ') % mApplicationName;
}

QString Script::toString() const noexcept
{
  return QLatin1String("#/usr/bin/env bash\n\n")
       % mCompletionsFunction.toString() % QLatin1String("\n\n")
       % getCompleteCommandString();
}

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{
