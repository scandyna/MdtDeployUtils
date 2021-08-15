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
#include "BashCompletionGenerator.h"
#include "Impl/BashCompletionGenerator.h"
#include "BashCompletion/Script.h"
#include "Algorithm.h"
#include <QLatin1Char>
#include <QLatin1String>
#include <QStringBuilder>
#include <QByteArray>
#include <QTextStream>
#include <QFile>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

void BashCompletionGenerator::setApplicationName(const QString & name)
{
  assert( !name.trimmed().isEmpty() );

  mApplicationName = name;
}

void BashCompletionGenerator::setMainCommand(const BashCompletionGeneratorCommand& command)
{
  assert( !command.isEmpty() );

  mMainCommand = command;
}

void BashCompletionGenerator::addSubCommand(const BashCompletionGeneratorCommand& command)
{
  assert( !command.isEmpty() );
  assert( command.hasName() );

  mSubCommands.push_back(command);
}

QString BashCompletionGenerator::generateScript() const
{
  assert( !mApplicationName.isEmpty() );

  using namespace Impl;
  using BashCompletion::Script;

  Script script(mApplicationName);
  addCaseClausesToScript(*this, script);
  script.addDefaultClause();

  return script.toString();
}

void BashCompletionGenerator::generateScriptToFile(const QString & directoryPath) const
{
  assert( !directoryPath.trimmed().isEmpty() );
  assert( !mApplicationName.isEmpty() );

//   const QString filePath = directoryPath % QLatin1Char('/') % mApplicationName % QLatin1String("-completion.bash");
  const QString filePath = directoryPath % QLatin1Char('/') % mApplicationName;
  QFile file(filePath);

  if( !file.open(QIODevice::WriteOnly | QIODevice::Text) ){
    const QString what = tr("open file '%1' failed: %2").arg( filePath, file.errorString() );
    throw BashCompletionScriptFileWriteError(what);
  }

  QTextStream stream(&file);
  stream << generateScript();
  stream.flush();

  if( !file.flush() ){
    const QString what = tr("write file '%1' failed: %2").arg( filePath, file.errorString() );
    throw BashCompletionScriptFileWriteError(what);
  }

  file.close();
}

void BashCompletionGenerator::generateScriptToFile(const std::string & directoryPath) const
{
  return generateScriptToFile( QString::fromLocal8Bit( QByteArray::fromStdString(directoryPath) ) );
}

BashCompletionGenerator BashCompletionGenerator::fromParserDefinition(const ParserDefinition & parserDefinition)
{
  BashCompletionGenerator generator;

  generator.setApplicationName( parserDefinition.applicationName() );
  generator.mMainCommand = BashCompletionGeneratorCommand::fromParserDefinitionCommand( parserDefinition.mainCommand() );
  for( const auto & command : parserDefinition.subCommands() ){
    generator.mSubCommands.emplace_back( BashCompletionGeneratorCommand::fromParserDefinitionCommand(command) );
  }

  return generator;
}

}} // namespace Mdt{ namespace CommandLineParser{
