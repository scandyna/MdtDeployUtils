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
#ifndef MDT_COMMAND_LINE_PARSER_IMPL_PARSER_DEFINITION_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_IMPL_PARSER_DEFINITION_COMMAND_H

#include "ParserDefinitionHelpUtils.h"
#include "../ParserDefinitionCommand.h"
#include <QCoreApplication>
#include <QString>
#include <QStringBuilder>
#include <QLatin1String>
#include <QLatin1Char>
#include <vector>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{ namespace Impl{

  /*! \internal For functions that require tr()
   */
  class ParserDefinitionCommandHelp
  {
    Q_DECLARE_TR_FUNCTIONS(ParserDefinitionCommandHelp)

   public:

    static
    QString getUsageText(const QString & applicationName, const ParserDefinitionCommand & command)
    {
      QString usageText = tr("Usage: %1 %2").arg( applicationName, command.name() );

      if( command.hasOptions() ){
        usageText += QLatin1String(" [") % tr("options") % QLatin1Char(']');
      }

      if( command.hasPositionalArguments() ){
        usageText += QLatin1Char(' ') % argumentListToUsageString( command.positionalArguments() );
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

  };

}}} // namespace Mdt{ namespace CommandLineParser{ namespace Impl{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_IMPL_PARSER_DEFINITION_COMMAND_H
