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
#include "CommandLine/Algorithm.h"
#include "ParserResultCommand.h"
#include <QString>
#include <QStringList>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

bool Parser::parse(const QStringList & arguments)
{
  Impl::ParseError error;

  mCommandLine.clear();

  if( !Impl::parse(arguments, mParserDefinition, mCommandLine, error) ){
    mErrorText = error.errorText;
    return false;
  }

  return true;
}

QStringList Parser::getUnknownOptionNames() const noexcept
{
  using CommandLine::isUnknownOption;
  using CommandLine::getOptionName;

  QStringList unknownOptionNames;

  for( const auto & argument : mCommandLine.argumentList() ){
    if( isUnknownOption(argument) ){
      unknownOptionNames.append( getOptionName(argument) );
    }
  }

  return unknownOptionNames;
}

ParserResult Parser::toParserResult() const noexcept
{
  return Impl::parserResultFromCommandLine(mCommandLine, mParserDefinition);
}

}} // namespace Mdt{ namespace CommandLineParser{
