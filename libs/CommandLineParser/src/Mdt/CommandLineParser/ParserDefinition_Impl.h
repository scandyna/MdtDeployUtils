/****************************************************************************
 **
 ** Copyright (C) 2011-2020 Philippe Steinmann.
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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_IMPL_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_IMPL_H

#include "ParserDefinition.h"
#include "ParserDefinitionOption.h"
#include "ParserDefinitionPositionalArgument.h"
#include <QString>
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QCoreApplication>
#include <cassert>
#include <vector>

namespace Mdt{ namespace CommandLineParser{

  /*! \internal
   */
  class ParserDefinition_Impl
  {
    Q_DECLARE_TR_FUNCTIONS(ParserDefinition_Impl)

   public:

    static
    QString getUsageText(const QString & applicationName, const std::vector<ParserDefinitionOption> & options, const std::vector<ParserDefinitionPositionalArgument> & arguments)
    {
      QString usageText = tr("Usage: %1").arg(applicationName);

      if( !options.empty() ){
        usageText += QLatin1String(" [") % tr("options") % QLatin1Char(']');
      }

      return usageText;
    }
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_IMPL_H
