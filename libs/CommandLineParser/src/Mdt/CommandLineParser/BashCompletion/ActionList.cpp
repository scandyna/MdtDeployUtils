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
#include "ActionList.h"
#include "../Algorithm.h"
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

QString ActionList::toCompreplyString() const noexcept
{
  if( isEmpty() ){
    return QString();
  }

  const auto extract = [](const Action & action){
    return action.toCompreplyArrayItemString();
  };

  return QLatin1String("COMPREPLY=(") % joinToQString(mActions, ' ', extract) % QLatin1Char(')');
}

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{
