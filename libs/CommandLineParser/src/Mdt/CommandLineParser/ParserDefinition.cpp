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
#include "ParserDefinition.h"
#include "ParserDefinition_Impl.h"
#include <QStringBuilder>
#include <cassert>

#include <QDebug>

namespace Mdt{ namespace CommandLineParser{

void ParserDefinition::setApplicationDescription(const QString &description)
{
  mApplicationDescription = description;
}

void ParserDefinition::addOption(const ParserDefinitionOption & option)
{
}

void ParserDefinition::addOption(const QString & name, const QString & description)
{
  assert( !name.trimmed().isEmpty() );
}

void ParserDefinition::addOption(char shortName, const QString & name, const QString & description)
{
  assert( !name.trimmed().isEmpty() );
}

void ParserDefinition::addHelpOption()
{
}

void ParserDefinition::addPositionalArgument(const QString & name, const QString & description, const QString & sytax)
{
  assert( !name.trimmed().isEmpty() );
}

void ParserDefinition::addSubCommand(const ParserDefinitionCommand & command)
{
  assert( !command.isEmpty() );
  assert( command.hasName() );
}

QString ParserDefinition::getHelpText() const
{
  QString helpText = getUsageText();

  return helpText;
}

QString ParserDefinition::getUsageText() const
{
  qDebug() << "build help text ...";
  
  QString usageText = QCoreApplication::applicationName();
  if( includeOptionsInHelpText() ){
    usageText += tr(" [options]");
  }


  return tr("Usage: %1").arg(usageText);
}

}} // namespace Mdt{ namespace CommandLineParser{
