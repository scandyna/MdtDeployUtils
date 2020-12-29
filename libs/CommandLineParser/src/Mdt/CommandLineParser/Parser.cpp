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
#include "Parser.h"
#include "Parser_Impl.h"
#include "ParserResultCommand.h"
#include <QCommandLineParser>
#include <QString>
#include <QStringList>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

ParserResult Parser::parse(const ParserDefinition & parserDefinition, const QStringList & arguments)
{
  ParserResult result;

  if( parserDefinition.hasSubCommands() ){
    QStringList mainCommandArguments;
    QStringList subCommandArguments;

    Impl::splitToMainAndSubCommandArguments(arguments, parserDefinition.subCommands(), mainCommandArguments, subCommandArguments);

    if( !Impl::parseMainCommandToResult(parserDefinition.mainCommand(), mainCommandArguments, result) ){
      return result;
    }

    if( subCommandArguments.isEmpty() ){
      return result;
    }
    assert( subCommandArguments.size() >= 2 );

    const auto subCommand = Impl::findSubCommandByName( parserDefinition.subCommands(), subCommandArguments.at(1) );
    // If sub-command is not found, its a bug in Impl::splitToMainAndSubCommandArguments()
    assert(subCommand);

    subCommandArguments.removeAt(1);
    Impl::parseSubCommandToResult(*subCommand, subCommandArguments, result);
  }else{
    Impl::parseMainCommandToResult(parserDefinition.mainCommand(), arguments, result);
  }

  return result;
}

}} // namespace Mdt{ namespace CommandLineParser{
