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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_IMPL_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_IMPL_H

#include "BashCompletionGenerator.h"
#include "BashCompletionGeneratorCommand.h"
#include "BashCompletionGeneratorOption.h"
#include "Algorithm.h"
#include <QString>
#include <QStringList>
#include <QLatin1String>
#include <QChar>
#include <QLatin1Char>
#include <QStringBuilder>

namespace Mdt{ namespace CommandLineParser{

  namespace Impl{

    inline
    void addCommandOptionToWordList(const BashCompletionGeneratorOption & option, QStringList & wordList)
    {
      if( option.hasShortName() ){
        wordList.push_back( option.shortNameWithDash() );
      }
      wordList.push_back( option.nameWithDashes() );
    }

    inline
    void addCommandOptionsToWordList(const BashCompletionGeneratorCommand& command, QStringList & wordList)
    {
      for( const auto & option : command.options() ){
        addCommandOptionToWordList(option, wordList);
      }
    }

    inline
    void addCommandArgumentsAndOptionsToWordList(const BashCompletionGeneratorCommand& command, QStringList & wordList)
    {
      wordList.append( command.arguments() );
      addCommandOptionsToWordList(command, wordList);
    }

    /// \todo should restrict level range
    inline
    QString generateCompreplyUsingCompgenWithWordList(int level, const BashCompletionGeneratorCommand & command)
    {
      QStringList wordList;
      addCommandArgumentsAndOptionsToWordList(command, wordList);
      const QString wordListStr = wordList.join( QLatin1Char(' ') );
      const QString levelStr = QString::number(level);

      return QLatin1String("    COMPREPLY=($(compgen -W \"") % wordListStr % QLatin1String("\" -- \"${COMP_WORDS[") % levelStr % QLatin1String("]}\"))");
    }

    inline
    QString generateAddCompreplyUsingCompgenForDirectoryCompletionIfEnabled(const BashCompletionGeneratorCommand & command)
    {
      if( command.directoryCompletionEnabled() ){
        return QLatin1String("    COMPREPLY+=($(compgen -A directory))\n");
      }
      return QString();
    }

    inline
    QString generateCommandBlockIfStatement(int level, const BashCompletionGeneratorCommand & command)
    {
      QString statement = QLatin1String("if [ \"${#COMP_WORDS[@]}\" == \"") % QString::number(level+1) % QLatin1String("\" ]");
      if( (level > 1)&&(command.hasName()) ){
        statement += QLatin1String(" && [ \"${COMP_WORDS[") % QString::number(level-1) % QLatin1String("]}\" == \"") % command.name() % QLatin1String("\" ]");
      }

      return statement;
    }

    /// \todo should restrict level range
    inline
    QString generateCommandBlock(int level, const BashCompletionGeneratorCommand & command, const QString & comment)
    {
      const QString blockComment = QLatin1String("  # ") % comment;
      const QChar nl = QLatin1Char('\n');

      return blockComment % nl
          % QLatin1String("  ") % generateCommandBlockIfStatement(level, command) % nl
          % QLatin1String("  then\n")
          %      generateCompreplyUsingCompgenWithWordList(level, command) % nl
          %      generateAddCompreplyUsingCompgenForDirectoryCompletionIfEnabled(command)
          % QLatin1String("  fi");
    }

    inline
    QString generateMainCommandBlock(const BashCompletionGeneratorCommand& command)
    {
      return generateCommandBlock( 1, command, QLatin1String("Arguments available for the main command") );
    }

    inline
    QString generateSubCommandBlock(const BashCompletionGeneratorCommand& command)
    {
      return generateCommandBlock( 2, command, QLatin1String("Arguments available for the ") % command.name() % QLatin1String(" command") );
    }

    inline
    QString generateSubCommandBlocksIfAny(const std::vector<BashCompletionGeneratorCommand>& commands)
    {
      if( commands.empty() ){
        return QString();
      }

      QString blocks;
      for(const BashCompletionGeneratorCommand & command : commands){
        blocks += QLatin1String("\n\n") % generateSubCommandBlock(command);
      }

      return blocks;
    }

  } // namespace Impl{

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_IMPL_H
