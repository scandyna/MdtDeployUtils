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
#include "ParserResultDebug.h"
#include "ParserResultOption.h"
#include "ParserResultCommand.h"
#include <QLatin1Char>
#include <QLatin1String>
#include <QStringBuilder>
#include <QStringList>

namespace Mdt{ namespace CommandLineParser{

QString toDebugString(const ParserResultOption & option)
{
  if( option.name().isEmpty() ){
    return QLatin1String("-null-");
  }
  if( option.hasValue() ){
    return option.name() % QLatin1Char('=') % option.value();
  }
  return option.name();
}

QString toDebugString(const ParserResult & result)
{
  QString str = QLatin1String("ParserResult:\n");

  if( result.hasOptions() ){
    str += QLatin1String(" options: ");
    for( const ParserResultOption & option : result.mainCommand().options() ){
      str += toDebugString(option) % QLatin1Char(' ');
    }
    str += QLatin1Char('\n');
  }

  if( result.hasPositionalArguments() ){
    str += QLatin1String(" arguments: ");
    for( const QString & argument : result.mainCommand().positionalArguments() ){
      str += argument % QLatin1Char(' ');
    }
    str += QLatin1Char('\n');
  }

  if( result.hasSubCommand() ){
    str += QLatin1String(" command: ") % result.subCommand().name() % QLatin1String(":");

    if( result.subCommand().hasOptions() ){
      str += QLatin1String("  options: ");
      for( const ParserResultOption & option : result.subCommand().options() ){
        str += toDebugString(option) % QLatin1Char(' ');
      }
      str += QLatin1Char('\n');
    }

    if( result.subCommand().hasPositionalArguments() ){
      str += QLatin1String("  arguments: ");
      for( const QString & argument : result.subCommand().positionalArguments() ){
        str += argument % QLatin1Char(' ');
      }
      str += QLatin1Char('\n');
    }

  }

  return str;
}

}} // namespace Mdt{ namespace CommandLineParser{
