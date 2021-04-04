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
#include "ParserDefinition.h"
#include "Impl/ParserDefinition.h"
#include <QStringBuilder>
#include <QLatin1Char>
#include <QLatin1String>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

void ParserDefinition::setApplicationName(const QString & name)
{
  mApplicationName = name;
}

void ParserDefinition::setApplicationDescription(const QString &description)
{
  mApplicationDescription = description;
}

void ParserDefinition::addOption(const ParserDefinitionOption & option)
{
  mMainCommand.addOption(option);
}

void ParserDefinition::addOption(const QString & name, const QString & description)
{
  assert( ParserDefinitionOption::isValidName(name) );

  mMainCommand.addOption(name, description);
}

void ParserDefinition::addOption(char shortName, const QString & name, const QString & description)
{
  assert( ParserDefinitionOption::isValidShortName(shortName) );
  assert( ParserDefinitionOption::isValidName(name) );

  mMainCommand.addOption(shortName, name, description);
}

void ParserDefinition::addHelpOption()
{
  mMainCommand.addHelpOption();
}

void ParserDefinition::addPositionalArgument(const QString & name, const QString & description, const QString & syntax)
{
  assert( !name.trimmed().isEmpty() );

  mMainCommand.addPositionalArgument(name, description, syntax);
}

void ParserDefinition::addPositionalArgument(ValueType type, const QString & name, const QString & description, const QString & syntax)
{
  assert( !name.trimmed().isEmpty() );

  mMainCommand.addPositionalArgument(type, name, description, syntax);
}

void ParserDefinition::addSubCommand(const ParserDefinitionCommand & command)
{
  assert( command.hasName() );

  mSubCommands.push_back(command);
}

QString ParserDefinition::getHelpText() const
{
  QString helpText = getUsageText();
  const QLatin1Char nl('\n');

  if( hasApplicationDescription() ){
    helpText += nl % applicationDescription() % nl;
  }

  if( hasOptions() ){
    helpText += nl % getOptionsHelpText() % nl;
  }

  if( hasPositionalArguments() ){
    helpText += nl % getPositionalArgumentsHelpText() % nl;
  }

  if( hasSubCommands() ){
    helpText += nl % getAvailableSubCommandsHelpText() % nl;
  }

  return helpText;
}

QString ParserDefinition::getUsageText() const
{
  return ParserDefinition_Impl::getUsageText( applicationName(),
                                              mMainCommand.options(),
                                              mMainCommand.positionalArguments(),
                                              mSubCommands );
}

QString ParserDefinition::getOptionsHelpText() const
{
  return ParserDefinition_Impl::getOptionsHelpText( mMainCommand.options(), mHelpTextLineMaxLength );
}

QString ParserDefinition::getPositionalArgumentsHelpText() const
{
  return ParserDefinition_Impl::getPositionalArgumentsHelpText( mMainCommand.positionalArguments(), mHelpTextLineMaxLength );
}

QString ParserDefinition::getAvailableSubCommandsHelpText() const
{
  return ParserDefinition_Impl::getAvailableSubCommandsHelpText(mSubCommands, mHelpTextLineMaxLength);
}

}} // namespace Mdt{ namespace CommandLineParser{
