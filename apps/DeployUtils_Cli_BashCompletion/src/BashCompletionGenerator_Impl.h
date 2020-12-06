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
#ifndef BASH_COMPLETION_GENERATOR_IMPL_H
#define BASH_COMPLETION_GENERATOR_IMPL_H

#include "BashCompletionGenerator.h"
#include "BashCompletionGeneratorCommand.h"
#include "BashCompletionGeneratorOption.h"
#include "Mdt/DeployUtils/Algorithm.h"
#include <string>
#include <vector>

namespace BashCompletionGenerator_Impl{

  inline
  void addCommandOptionToWordList(const BashCompletionGeneratorOption & option, std::vector<std::string> & wordList)
  {
    if( option.hasShortName() ){
      wordList.push_back( option.shortNameWithDash() );
    }
    wordList.push_back( option.nameWithDashes() );
  }

  inline
  void addCommandOptionsToWordList(const BashCompletionGeneratorCommand& command, std::vector<std::string> & wordList)
  {
    for( const auto & option : command.options() ){
      addCommandOptionToWordList(option, wordList);
    }
  }

  inline
  void addCommandArgumentsAndOptionsToWordList(const BashCompletionGeneratorCommand& command, std::vector<std::string> & wordList)
  {
    Mdt::DeployUtils::appendToStdVector(command.arguments(), wordList);
    addCommandOptionsToWordList(command, wordList);
  }

  /// \todo should restrict level range
  inline
  std::string generateCompreplyUsingCompgenWithWordList(int level, const BashCompletionGeneratorCommand& command)
  {
    using namespace std::string_literals;

    std::vector<std::string> wordList;
    addCommandArgumentsAndOptionsToWordList(command, wordList);
    const std::string wordListStr = Mdt::DeployUtils::joinToStdString(wordList, ' ');
    const std::string levelStr = std::to_string(level);

    return "    COMPREPLY=($(compgen -W \""s + wordListStr + "\" -- \"${COMP_WORDS[" + levelStr + "]}\"))";
  }

  inline
  std::string generateAddCompreplyUsingCompgenForDirectoryCompletionIfEnabled(const BashCompletionGeneratorCommand & command)
  {
    if( command.directoryCompletionEnabled() ){
      return "    COMPREPLY+=($(compgen -A directory))\n";
    }
    return "";
  }

  inline
  std::string generateCommandBlockIfStatement(int level, const BashCompletionGeneratorCommand& command)
  {
    using namespace std::string_literals;

    std::string statement = "if [ \"${#COMP_WORDS[@]}\" == \""s + std::to_string(level+1) + "\" ]";
    if( (level > 1)&&(command.hasName()) ){
      statement += " && [ \"${COMP_WORDS[" + std::to_string(level-1) + "]}\" == \"" + command.name() + "\" ]";
    }

    return statement;
  }

  /// \todo should restrict level range
  inline
  std::string generateCommandBlock(int level, const BashCompletionGeneratorCommand& command, const std::string & comment)
  {
    using namespace std::string_literals;

    const std::string blockComment = "  # "s + comment;

    return blockComment + "\n"
        + "  " + generateCommandBlockIfStatement(level, command) + "\n"
        + "  then\n"
        +      generateCompreplyUsingCompgenWithWordList(level, command) + "\n"
        +      generateAddCompreplyUsingCompgenForDirectoryCompletionIfEnabled(command)
        + "  fi";
  }

  inline
  std::string generateMainCommandBlock(const BashCompletionGeneratorCommand& command)
  {
    return generateCommandBlock(1, command, std::string("Arguments available for the main command"));
  }

  inline
  std::string generateSubCommandBlock(const BashCompletionGeneratorCommand& command)
  {
    return generateCommandBlock(2, command, std::string("Arguments available for the ") + command.name() + " command");
  }

  inline
  std::string generateSubCommandBlocksIfAny(const std::vector<BashCompletionGeneratorCommand>& commands)
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

} // namespace BashCompletionGenerator_Impl{

#endif // #ifndef BASH_COMPLETION_GENERATOR_IMPL_H
