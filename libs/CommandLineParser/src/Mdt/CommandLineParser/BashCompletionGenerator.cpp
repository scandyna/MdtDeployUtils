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
#include "BashCompletionGenerator.h"
#include "BashCompletionGenerator_Impl.h"
#include "Algorithm.h"
#include <QLatin1Char>
#include <QLatin1String>
#include <QStringBuilder>
#include <cassert>
#include <fstream>

#include <QByteArray>

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
  assert( !mMainCommand.isEmpty() );

//   using namespace std::string_literals;
  using namespace Impl;

  const QString scriptInclude = QLatin1String("#/usr/bin/env bash");
  const QString completeFunctionName = QLatin1Char('_') % mApplicationName % QLatin1String("_completions()");
  const QString completeCommand = QLatin1String("complete -F _") % mApplicationName % QLatin1String("_completions ") % mApplicationName;

  const QString completeFunction
    = completeFunctionName
    % QLatin1String("\n{\n")
    % generateMainCommandBlock(mMainCommand)
    % generateSubCommandBlocksIfAny(mSubCommands)
    % QLatin1String("\n}");

  return scriptInclude % QLatin1String("\n\n") % completeFunction % QLatin1String("\n\n") % completeCommand % QLatin1String("\n");
}

void BashCompletionGenerator::generateScriptToFile(const QString & directoryPath) const
{
  assert( !directoryPath.trimmed().isEmpty() );
  assert( !mApplicationName.isEmpty() );
  assert( !mMainCommand.isEmpty() );

  const QString filePath = directoryPath % QLatin1Char('/') % mApplicationName % QLatin1String("-completion.bash");
  std::ofstream stream(filePath.toLocal8Bit().toStdString(), std::ios_base::out | std::ios_base::trunc);
  if( stream.bad() || stream.fail() ){
    stream.close();
    const QString what = tr("open file '%1' failed").arg(filePath);
    throw BashCompletionScriptFileWriteError(what);
  }

  try{
    stream << generateScript().toLocal8Bit().toStdString();
    stream.close();
  }catch(...){
    stream.close();
    const QString what = tr("write file '%1' failed").arg(filePath);
    throw BashCompletionScriptFileWriteError(what);
  }
}

BashCompletionGenerator BashCompletionGenerator::fromParserDefinition(const ParserDefinition & parserDefinition)
{
  BashCompletionGenerator generator;

  return generator;
}

}} // namespace Mdt{ namespace CommandLineParser{
