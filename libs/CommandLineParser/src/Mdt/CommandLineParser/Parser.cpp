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
#include "Parser.h"
#include "Impl/Parser.h"
#include "Impl/ParserResultFromCommandLine.h"
#include "ParserResultCommand.h"
#include <QString>
#include <QStringList>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

bool Parser::parse(const ParserDefinition & parserDefinition, const QStringList & arguments)
{
  Impl::ParseError error;

  mCommandLine.clear();

  if( !Impl::parse(arguments, parserDefinition, mCommandLine, error) ){
    mErrorText = error.errorText;
    return false;
  }

  return true;
}

ParserResult Parser::toParserResult() const noexcept
{
  return Impl::parserResultFromCommandLine(mCommandLine);
}

}} // namespace Mdt{ namespace CommandLineParser{
