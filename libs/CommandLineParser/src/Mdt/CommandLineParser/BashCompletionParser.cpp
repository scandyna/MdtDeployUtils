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
#include "BashCompletionParser.h"
#include "BashCompletionParserQuery.h"
#include "Impl/BashCompletionParser.h"
#include <QString>
#include <iostream>

namespace Mdt{ namespace CommandLineParser{

bool handleBashCompletion(const CommandLine::CommandLine & commandLine, const ParserDefinition & parserDefinition)
{
  if( !BashCompletionParserQuery::isValidBashCompletionQuery(commandLine, parserDefinition) ){
    return false;
  }

  const QString currentArgument = Impl::completionFindCurrentArgument(commandLine, parserDefinition);
  std::cout << currentArgument.toStdString() << std::flush;

  return true;
}

}} // namespace Mdt{ namespace CommandLineParser{
