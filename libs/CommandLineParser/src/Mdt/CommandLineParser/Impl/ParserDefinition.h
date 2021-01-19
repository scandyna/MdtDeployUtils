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
#ifndef MDT_COMMAND_LINE_PARSER_IMPL_PARSER_DEFINITION_H
#define MDT_COMMAND_LINE_PARSER_IMPL_PARSER_DEFINITION_H

#include "../ParserDefinition.h"
#include "../ParserDefinitionOption.h"
#include "../ParserDefinitionPositionalArgument.h"
#include "../ParserDefinitionCommand.h"
#include "../Algorithm.h"
#include "ParserDefinitionHelpUtils.h"
#include <QString>
#include <QLatin1String>
#include <QChar>
#include <QLatin1Char>
#include <QStringBuilder>
#include <QCoreApplication>
#include <cassert>
#include <vector>
#include <iterator>
#include <algorithm>

namespace Mdt{ namespace CommandLineParser{

  namespace Impl{

    /*! \internal
     */
    static
    QString wrapText(const ParserDefinitionCommand & command, int longestNamesStringLength, int maxLength)
    {
      return wrapText( command.name(), longestNamesStringLength, command.description(), maxLength );
    }

    /*! \internal Find the longest name in \a commands
     *
     * \pre \a commands must not be empty
     */
    static
    int findLongestCommandNameStringLength(const std::vector<ParserDefinitionCommand> & commands) noexcept
    {
      assert( !commands.empty() );

      const auto lessThan = [](const ParserDefinitionCommand & a, const ParserDefinitionCommand & b)
      {
        return a.nameLength() < b.nameLength();
      };
      const auto it = std::max_element(commands.cbegin(), commands.cend(), lessThan);
      assert( it != commands.cend() );

      return it->nameLength();
    }

  } // namespace Impl{

  /*! \internal For functions that require tr()
   */
  class ParserDefinition_Impl
  {
    Q_DECLARE_TR_FUNCTIONS(ParserDefinition_Impl)

   public:

    static
    QString getUsageText(const QString & applicationName, const std::vector<ParserDefinitionOption> & options,
                         const std::vector<ParserDefinitionPositionalArgument> & arguments,
                         const std::vector<ParserDefinitionCommand> & subCommands)
    {
      QString usageText = tr("Usage: %1").arg(applicationName);

      if( !options.empty() ){
        usageText += QLatin1String(" [") % tr("options") % QLatin1Char(']');
      }

      if( !arguments.empty() ){
        usageText += QLatin1Char(' ') % Impl::argumentListToUsageString(arguments);
      }

      if( !subCommands.empty() ){
        usageText += QLatin1Char(' ') % tr("command");
      }

      return usageText;
    }

    /*! \brief Get options help text
     *
     * \pre \a options must not be empty
     */
    static
    QString getOptionsHelpText(const std::vector<ParserDefinitionOption> & options, int maxLength)
    {
      assert( !options.empty() );

      const int longestNamesStringLength = Impl::findLongestOptionNamesStringLength(options);
      QString text = tr("Options:");
      for(const auto & option : options){
        text += QLatin1Char('\n') + Impl::wrapText(option, longestNamesStringLength, maxLength);
      }

      return text;
    }

    /*! \brief Get positional arguments help text
     *
     * \pre \a arguments must not be empty
     */
    static
    QString getPositionalArgumentsHelpText(const std::vector<ParserDefinitionPositionalArgument> & arguments, int maxLength)
    {
      assert( !arguments.empty() );

      const int longestNamesStringLength = Impl::findLongestArgumentNamesStringLength(arguments);
      QString text = tr("Arguments:");
      for(const auto & argument : arguments){
        text += QLatin1Char('\n') + Impl::wrapText(argument, longestNamesStringLength, maxLength);
      }

      return text;
    }

    static
    QString getAvailableSubCommandsHelpText(const std::vector<ParserDefinitionCommand> & subCommands, int maxLength)
    {
      const int longestNameStringLength = Impl::findLongestCommandNameStringLength(subCommands);
      QString text = tr("Commands:");
      for(const auto & command : subCommands){
        text += QLatin1Char('\n') + Impl::wrapText(command, longestNameStringLength, maxLength);
      }

      return text;
    }
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_IMPL_PARSER_DEFINITION_H
