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
#include <QCommandLineParser>

namespace Mdt{ namespace CommandLineParser{

ParserResult Parser::parse(const ParserDefinition & parserDefinition, const QStringList & arguments)
{
  ParserResult result;

  if( parserDefinition.hasSubCommands() ){
    // split arguments
    // setup main QCommandLineParser
    // parse main arguments
    // if has sub-command arguments:
    //  setup QCommandLineParser for the found sub-command
    //  parse sub-command arguments
  }else{
    QCommandLineParser qtParser;
    Impl::setupQtParser( qtParser, parserDefinition.mainCommand() );
    if( !qtParser.parse(arguments) ){
      result.setErrorText( qtParser.errorText() );
      return result;
    }
  }
  // At some time, construct the result

  return result;
}

}} // namespace Mdt{ namespace CommandLineParser{
