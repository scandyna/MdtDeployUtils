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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_PARSER_RESULT_COMMAND_H

#include "ParserResultOption.h"
#include "ParserDefinitionOption.h"
#include "ParserResultPositionalArgument.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1String>
#include <QStringList>
#include <vector>
#include <algorithm>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Command result from a Parser
   *
   * 
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserResultCommand
  {
   public:

    /*! \brief Add \a option to this result
     */
    void addOption(const ParserResultOption & option)
    {
      mOptions.push_back(option);
    }

    /*! \brief Check if \a option is set in this result command
     */
    bool isSet(const ParserDefinitionOption & option) const
    {
      return isSet( option.name() );
    }

    /*! \brief Check if \a optionName is set in this result command
     */
    bool isSet(const QString & optionName) const
    {
      const auto pred = [&optionName](const ParserResultOption & option){
        return option.name() == optionName;
      };
      const auto it = std::find_if(mOptions.cbegin(), mOptions.cend(), pred);

      return it != mOptions.cend();
    }

//     /*! \brief Check if \a optionShortName is set in this result command
//      */
//     bool isSet(char optionShortName) const
//     {
//     }

    /*! \brief Check if the help option is set in this result command
     */
    bool isHelpOptionSet() const
    {
      return isSet( QLatin1String("help") );
    }

    /*! \brief Get the count of arguments of this result command
     */
    int positionalArgumentCount() const noexcept
    {
      return mPositionalArguments.count();
    }

    /*! \brief Add \a argument to this result command
     */
    void addPositionalArgument(const QString & argument)
    {
      mPositionalArguments.append(argument);
    }

    /*! \brief Check if this command has a positional argument at \a index
     *
     * \pre \a index must be >= 0
     */
    bool hasPositionalArgumentAt(int index) const noexcept
    {
      assert( index >= 0 );

      return index < positionalArgumentCount();
    }

    /*! \brief Get the positional argument at \a index
     *
     * \pre \a index must be in valid range ( 0 <= \a index < argumentCount() )
     */
    const QString & positionalArgumentAt(int index) const noexcept
    {
      assert( index >= 0 );
      assert( index < positionalArgumentCount() );

      return mPositionalArguments.at(index);
    }

   private:

    std::vector<ParserResultOption> mOptions;
    QStringList mPositionalArguments;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_COMMAND_H
