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
#include "ActionVariantAlgorithm.h"
#include <QStringBuilder>
#include <QLatin1String>
#include <QLatin1Char>

namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{

/*! \internal Visitor for actionVariantToCompreplyArrayItemString()
 */
class MDT_COMMANDLINEPARSER_EXPORT ActionVariantToCompreplyArrayItemString : public boost::static_visitor<QString>
{
  public:

  QString operator()(boost::blank) const noexcept
  {
    return QString();
  }

  QString operator()(const CompgenCommand & command) const noexcept
  {
    const QString compgenWord = QLatin1String("$cur");

    return QLatin1String("$(") % command.toCompgenCommandString(compgenWord) % QLatin1Char(')');
  }

  QString operator()(const CustomAction & action) const noexcept
  {
    return QLatin1String("$(") % action.action % QLatin1Char(')');
  }
};

QString actionVariantToCompreplyArrayItemString(const ActionVariant & action) noexcept
{
  return boost::apply_visitor(ActionVariantToCompreplyArrayItemString(), action);
}

}}} // namespace Mdt{ namespace CommandLineParser{ namespace BashCompletion{
