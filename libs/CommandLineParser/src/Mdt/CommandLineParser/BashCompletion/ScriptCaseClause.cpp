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
#include "ScriptCaseClause.h"
#include <QLatin1Char>
#include <QLatin1String>
#include <QStringBuilder>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

QString ScriptCaseClause::toString() const noexcept
{
  QString commandString;
  if( hasCommand() ){
    commandString = QLatin1String("      ") % mCommand % QLatin1Char('\n');
  }

  return QLatin1String("    ") % mPattern % QLatin1String(")\n")
       % commandString
       % QLatin1String("      ;;");
}

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{
