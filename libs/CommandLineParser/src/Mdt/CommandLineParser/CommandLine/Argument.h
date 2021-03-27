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
#ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ARGUMENT_H
#define MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ARGUMENT_H

#include "mdt_commandlineparser_export.h"
#include <QString>
#include <boost/variant.hpp>
#include <vector>

namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

  /*! \brief The executable of the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT Executable
  {
    QString name;
  };

  /*!\brief A positional argument in the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT PositionalArgument
  {
    QString value;
  };

  /*! \brief A option in the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT Option
  {
    QString name;
    bool expectsValue = false;
  };

  /*! \brief A unknown option in the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT UnknownOption
  {
    QString name;
  };

  /*! \brief A list of short options in the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT ShortOptionList
  {
    std::vector<char> names;
    bool lastOptionExpectsValue = false;
  };

  /*! \brief A option value in the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT OptionValue
  {
    QString value;
  };

  /*! \brief A option with a value in the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT OptionWithValue
  {
    QString name;
    QString value;
  };

  /*! \brief A list of short options with the last one having a value
   */
  struct MDT_COMMANDLINEPARSER_EXPORT ShortOptionListWithLastHavingValue
  {
    std::vector<char> names;
    QString value;
  };

  /*! \brief A single dash in the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT SingleDash
  {
  };

  /*! \brief A double dash in the command line
   */
  struct MDT_COMMANDLINEPARSER_EXPORT DoubleDash
  {
  };

  /*!\brief The sub-command name in the command line
   */
  struct SubCommandName
  {
    QString name;
  };

  /*! \brief A argument of the command line
   */
  using Argument = boost::variant<
    Executable, PositionalArgument, SubCommandName,
    Option, UnknownOption, ShortOptionList, OptionValue, OptionWithValue,
    ShortOptionListWithLastHavingValue,
    SingleDash, DoubleDash>;

  /*! \brief List of arguments in the command line
   */
  using ArgumentList = std::vector<Argument>;

}}} // namespace Mdt{ namespace CommandLineParser{ namespace CommandLine{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_COMMAND_LINE_ARGUMENT_H
