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
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

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
