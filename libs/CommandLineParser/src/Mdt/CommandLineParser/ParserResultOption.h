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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_OPTION_H
#define MDT_COMMAND_LINE_PARSER_PARSER_RESULT_OPTION_H

#include "ParserDefinitionOption.h"
#include "mdt_commandlineparser_export.h"
#include <QString>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Option result from a Parser
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserResultOption
  {
   public:

    /*! \brief Construct a null option
     */
    explicit ParserResultOption() noexcept = default;

    /*! \brief Construct a option with a name
     *
     * \pre \a name must be a valid option name
     * \sa isValidName()
     */
    explicit ParserResultOption(const QString & name)
     : mName( name.trimmed() )
    {
      assert( isValidName(name) );
    }

    /*! \brief Copy construct a option from \a other
     */
    ParserResultOption(const ParserResultOption & other) = default;

    /*! \brief Copy assign \a other to this option
     */
    ParserResultOption & operator=(const ParserResultOption & other) = default;

    /*! \brief Move construct a option from \a other
     */
    ParserResultOption(ParserResultOption && other) noexcept = default;

    /*! \brief Move assign \a other to this option
     */
    ParserResultOption & operator=(ParserResultOption && other) noexcept = default;

    /*! \brief Get the name
     */
    const QString & name() const noexcept
    {
      return mName;
    }

    /*! \brief Set the value for this option
     */
    void setValue(const QString & value)
    {
      mValue = value;
    }

    /*! \brief Check if this option has a value
     */
    bool hasValue() const noexcept
    {
      return !mValue.isEmpty();
    }

    /*! \brief Get the value for this option
     */
    const QString & value() const noexcept
    {
      return mValue;
    }

    /*! \brief Check if \a name is a valid name for a option
     *
     * \sa ParserDefinitionOption::isValidName(name)
     */
    static
    bool isValidName(const QString & name)
    {
      return ParserDefinitionOption::isValidName(name);
    }

   private:

    QString mName;
    QString mValue;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_RESULT_OPTION_H
