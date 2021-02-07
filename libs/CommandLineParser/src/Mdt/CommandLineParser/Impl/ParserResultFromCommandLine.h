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
#ifndef MDT_COMMAND_LINE_PARSER_IMPL_PARSER_RESULT_FROM_COMMAND_LINE_H
#define MDT_COMMAND_LINE_PARSER_IMPL_PARSER_RESULT_FROM_COMMAND_LINE_H

#include "../CommandLine/CommandLine.h"
#include "../CommandLine/Argument.h"
#include "../CommandLine/Algorithm.h"
#include "../Algorithm.h"
#include "../ParserResult.h"
#include "../ParserResultCommand.h"
#include <QString>
#include <QLatin1String>
#include <boost/variant.hpp>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace Impl{

  /*! \internal
   */
  using ArgumentListConstIterator = CommandLine::ArgumentList::const_iterator;

  /*! \internal
   */
  class AddArgumentToResultCommand : public boost::static_visitor<>
  {
   public:

    AddArgumentToResultCommand(ParserResultCommand & command) noexcept
     : mCommand(command)
    {
    }

    void operator()(const Mdt::CommandLineParser::CommandLine::Executable &) noexcept
    {
    }

    void operator()(const Mdt::CommandLineParser::CommandLine::PositionalArgument & argument)
    {
      mCommand.addPositionalArgument(argument.value);
    }

    void operator()(const Mdt::CommandLineParser::CommandLine::SingleDash &)
    {
      mCommand.addPositionalArgument( QLatin1String("-") );
    }

    void operator()(const Mdt::CommandLineParser::CommandLine::DoubleDash &) noexcept
    {
    }

    void operator()(const Mdt::CommandLineParser::CommandLine::Option & argument)
    {
      mCommand.addOption( ParserResultOption(argument.name) );
    }

    void operator()(const Mdt::CommandLineParser::CommandLine::OptionValue & argument)
    {
      assert( mCommand.hasOptions() );

      mCommand.setLastOptionValue(argument.value);
    }

    void operator()(const Mdt::CommandLineParser::CommandLine::OptionWithValue & argument)
    {
      ParserResultOption option(argument.name);
      option.setValue(argument.value);

      mCommand.addOption(option);
    }

    void operator()(const Mdt::CommandLineParser::CommandLine::SubCommandName &) noexcept
    {
    }

   private:

    ParserResultCommand & mCommand;
  };

  /*! \internal Get a parser result from \a commandLine
   */
  ParserResult parserResultFromCommandLine(const CommandLine::CommandLine & commandLine)
  {
    ParserResult parserResult;
    const auto & argumentList = commandLine.argumentList();

    ParserResultCommand mainCommand;
    AddArgumentToResultCommand addArgumentToMainCommand(mainCommand);
    auto subCommandPosIt = forEachUntil( argumentList.cbegin(), argumentList.cend(),
                                               boost::apply_visitor(addArgumentToMainCommand), CommandLine::isSubCommandNameArgument );
    parserResult.setMainCommand(mainCommand);

    if( subCommandPosIt != argumentList.cend() ){
      ParserResultCommand subCommand( getSubCommandName(*subCommandPosIt) );
      ++subCommandPosIt;

      AddArgumentToResultCommand addArgumentToSubCommand(subCommand);
      std::for_each( subCommandPosIt, argumentList.cend(), boost::apply_visitor(addArgumentToSubCommand) );

      parserResult.setSubCommand(subCommand);
    }

    return parserResult;
  }

}}} // namespace Mdt{ namespace CommandLineParser{ namespace Impl{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_IMPL_PARSER_RESULT_FROM_COMMAND_LINE_H
