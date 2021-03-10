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
#include "CompgenCommand.h"
#include "CompgenArgumentAlgorithm.h"
#include <QStringBuilder>
#include <QLatin1String>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

QString CompgenCommand::toCompgenCommandString(const QString & word) const noexcept
{
  return QLatin1String("compgen ") % compgenArgumentListToString(mArgumentList) % QLatin1String("-- \"$cur\"");
}

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{