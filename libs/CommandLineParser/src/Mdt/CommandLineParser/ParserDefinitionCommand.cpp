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
#include "ParserDefinitionCommand.h"
#include "Impl/ParserDefinitionCommand.h"
#include <QStringBuilder>
#include <QLatin1Char>
#include <QLatin1String>

namespace Mdt{ namespace CommandLineParser{

QString ParserDefinitionCommand::getHelpText(const QString & applicationName) const
{
  QString helpText = getUsageText(applicationName);
  const QLatin1Char nl('\n');

  if( hasDescription() ){
    helpText += nl % description() % nl;
  }

  if( hasOptions() ){
    helpText += nl % getOptionsHelpText() % nl;
  }

  if( hasPositionalArguments() ){
    helpText += nl % getPositionalArgumentsHelpText() % nl;
  }

  return helpText;
}

QString ParserDefinitionCommand::getUsageText(const QString & applicationName) const
{
  return Impl::ParserDefinitionCommandHelp::getUsageText(applicationName, *this);
}

QString ParserDefinitionCommand::getOptionsHelpText() const
{
  return Impl::ParserDefinitionCommandHelp::getOptionsHelpText( mOptions, mHelpTextLineMaxLength );
}

QString ParserDefinitionCommand::getPositionalArgumentsHelpText() const
{
  return Impl::ParserDefinitionCommandHelp::getPositionalArgumentsHelpText( mPositionalArguments, mHelpTextLineMaxLength );
}

}} // namespace Mdt{ namespace CommandLineParser{
