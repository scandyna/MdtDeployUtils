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
#include "BashCompletionGeneratorOption.h"
#include "Mdt/DeployUtils/Algorithm.h"
#include <cassert>

void BashCompletionGenerator::setApplicationName(const std::string& name)
{
  assert( !name.empty() );

  mApplicationName = name;
}

void BashCompletionGenerator::setMainCommand(const BashCompletionGeneratorCommand& command)
{
  assert( !command.isEmpty() );

  mMainCommand.reset(new BashCompletionGeneratorCommand(command) );
}

void BashCompletionGenerator::addSubCommand(const BashCompletionGeneratorCommand& command)
{
  mSubCommands.push_back(command);
}

std::string BashCompletionGenerator::generateScript() const
{
  assert( !mApplicationName.empty() );
  assert( mMainCommand.get() != nullptr );
  assert( !mMainCommand->isEmpty() );

  using namespace std::string_literals;

  const std::string scriptInclude = "#/usr/bin/env bash";
  const std::string completeFunctionName = "_"s + mApplicationName + "_completions()";
  const std::string completeCommand = "complete -F _"s + mApplicationName + "_completions " + mApplicationName;

  const std::string completeFunction
    = completeFunctionName
    + "\n{\n"
    + generateMainCommandBlock(*mMainCommand)
    + generateSubCommandBlocksIfAny(mSubCommands)
    + "\n}";

  return scriptInclude + "\n\n" + completeFunction + "\n\n" + completeCommand + "\n";
}

void addCommandOptionToWordList(const BashCompletionGeneratorOption & option, std::vector<std::string> & wordList)
{
  if( option.hasShortName() ){
    wordList.push_back( option.shortNameWithDash() );
  }
  wordList.push_back( option.nameWithDashes() );
}

void BashCompletionGenerator::addCommandOptionsToWordList(const BashCompletionGeneratorCommand& command, std::vector<std::string> & wordList)
{
  for( const auto & option : command.options() ){
    addCommandOptionToWordList(option, wordList);
  }
}

void BashCompletionGenerator::addCommandArgumentsAndOptionsToWordList(const BashCompletionGeneratorCommand& command, std::vector<std::string> & wordList)
{
  Mdt::DeployUtils::appendToStdVector(command.arguments(), wordList);
  addCommandOptionsToWordList(command, wordList);
}

std::string BashCompletionGenerator::generateCompreplyUsingCompgenWithWordList(int level, const BashCompletionGeneratorCommand& command)
{
  using namespace std::string_literals;

  std::vector<std::string> wordList;
  addCommandArgumentsAndOptionsToWordList(command, wordList);
  const std::string wordListStr = Mdt::DeployUtils::joinToStdString(wordList, ' ');
  const std::string levelStr = std::to_string(level+1);

  return "    COMPREPLY=($(compgen -W \""s + wordListStr + "\" -- \"${COMP_WORDS[" + levelStr + "]}\"))";
}

std::string BashCompletionGenerator::generateAddCompreplyUsingCompgenForDirectoryCompletionIfEnabled(const BashCompletionGeneratorCommand & command)
{
  if( command.directoryCompletionEnabled() ){
    return "    COMPREPLY+=($(compgen -A directory))\n";
  }
  return "";
}

std::string BashCompletionGenerator::generateCommandBlock(int level, const BashCompletionGeneratorCommand& command, const std::string & comment)
{
  using namespace std::string_literals;

  const std::string blockComment = "  # "s + comment;

  return blockComment + "\n"
       + "  if [ \"${#COMP_WORDS[@]}\" == \""s + std::to_string(level+1) + "\" ]\n"
       + "  then\n"
       +      generateCompreplyUsingCompgenWithWordList(level, command) + "\n"
       +      generateAddCompreplyUsingCompgenForDirectoryCompletionIfEnabled(command)
       + "  fi";
}

std::string BashCompletionGenerator::generateMainCommandBlock(const BashCompletionGeneratorCommand& command)
{
  return generateCommandBlock(1, command, std::string("Arguments available for the ") + command.name() + " command");
}

std::string BashCompletionGenerator::generateSubCommandBlock(const BashCompletionGeneratorCommand& command)
{
  return generateCommandBlock(2, command, std::string("Arguments available for the ") + command.name() + " command");
}

std::string BashCompletionGenerator::generateSubCommandBlocksIfAny(const std::vector<BashCompletionGeneratorCommand>& commands)
{
  if( commands.empty() ){
    return "";
  }

  std::string blocks;
  for(const BashCompletionGeneratorCommand & command : commands){
    blocks += "\n\n" + generateSubCommandBlock(command);
  }

  return blocks;
}
