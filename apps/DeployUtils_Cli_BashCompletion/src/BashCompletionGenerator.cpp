/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2020 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "BashCompletionGenerator.h"
#include "BashCompletionGenerator_Impl.h"
#include "Mdt/DeployUtils/Algorithm.h"
#include <cassert>
#include <fstream>

void BashCompletionGenerator::setApplicationName(const std::string& name)
{
  assert( !name.empty() );

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

std::string BashCompletionGenerator::generateScript() const
{
  assert( !mApplicationName.empty() );
  assert( !mMainCommand.isEmpty() );

  using namespace std::string_literals;
  using namespace BashCompletionGenerator_Impl;

  const std::string scriptInclude = "#/usr/bin/env bash";
  const std::string completeFunctionName = "_"s + mApplicationName + "_completions()";
  const std::string completeCommand = "complete -F _"s + mApplicationName + "_completions " + mApplicationName;

  const std::string completeFunction
    = completeFunctionName
    + "\n{\n"
    + generateMainCommandBlock(mMainCommand)
    + generateSubCommandBlocksIfAny(mSubCommands)
    + "\n}";

  return scriptInclude + "\n\n" + completeFunction + "\n\n" + completeCommand + "\n";
}

void BashCompletionGenerator::generateScriptToFile(const std::string & directoryPath) const
{
  assert( !directoryPath.empty() );
  assert( !mApplicationName.empty() );
  assert( !mMainCommand.isEmpty() );

  const std::string filePath = directoryPath + "/" + mApplicationName + "-completion.bash";
  std::ofstream stream(filePath, std::ios_base::out | std::ios_base::trunc);
  if( stream.bad() || stream.fail() ){
    stream.close();
    const std::string what = "open file '" + filePath + "' failed";
    throw BashCompletionScriptFileWriteError(what);
  }

  try{
    stream << generateScript();
    stream.close();
  }catch(...){
    stream.close();
    const std::string what = "write file '" + filePath + "' failed";
    throw BashCompletionScriptFileWriteError(what);
  }
}
