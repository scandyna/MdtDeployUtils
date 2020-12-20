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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_OPTION_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_OPTION_H

#include "Algorithm.h"
#include "mdt_commandlineparser_export.h"
#include <QChar>
#include <QString>
#include <QLatin1String>
#include <QLatin1Char>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Command line option for a ParserDefinition or a ParserDefinitionCommand
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserDefinitionOption
  {
   public:

    ParserDefinitionOption() = delete;

    /*! \brief Construct a option with a name and a description
     *
     * \pre \a name must be a valid option name
     * \sa isValidName()
     */
    ParserDefinitionOption(const QString & name, const QString & description)
     : mName( name.trimmed() ),
       mDescription(description)
    {
      assert( isValidName(name) );
    }

    /*! \brief Construct a option with a short name, a name and a description
     *
     * \pre \a shortName must be a valid option short name
     * \pre \a name must be a valid option name
     * \sa isValidShortName()
     * \sa isValidName()
     */
    ParserDefinitionOption(char shortName, const QString & name, const QString & description)
     : mShortName(shortName),
       mName( name.trimmed() ),
       mDescription(description)
    {
      assert( isValidShortName(shortName) );
      assert( isValidName(name) );
    }

    /*! \brief Copy construct a option from \a other
     */
    ParserDefinitionOption(const ParserDefinitionOption & other) = default;

    /*! \brief Copy assign \a other to this option
     */
    ParserDefinitionOption & operator=(const ParserDefinitionOption & other) = default;

    /*! \brief Move construct a option from \a other
     */
    ParserDefinitionOption(ParserDefinitionOption && other) = default;

    /*! \brief Move assign \a other to this option
     */
    ParserDefinitionOption & operator=(ParserDefinitionOption && other) = default;

    /*! \brief Check if this command line option has a short name
     */
    bool hasShortName() const noexcept
    {
      return mShortName != '\0';
    }

    /*! \brief Get the short name
     *
     * \sa shortNameWithDash()
     */
    char shortName() const noexcept
    {
      return mShortName;
    }

    /*! \brief Get the name
     *
     * \sa nameWithDashes()
     */
    const QString & name() const noexcept
    {
      return mName;
    }

    /*! \brief Get the short name with a dash
     *
     * \pre must only be called if this command line option has a short name
     * \sa hasShortName()
     */
    QString shortNameWithDash() const
    {
      assert( hasShortName() );

      return qStringFromChars('-', mShortName);
    }

    /*! \brief Get the length of the name
     */
    int nameLength() const noexcept
    {
      return mName.length();
    }

    /*! \brief Get the name with dashes
     */
    QString nameWithDashes() const
    {
      return QLatin1String("--") + mName;
    }

    /*! \brief Get the description of this option
     */
    const QString & description() const noexcept
    {
      return mDescription;
    }

    /*! \brief Check if \a name is a valid short name for a option
     *
     * A valid short name is a letter
     */
    static
    bool isValidShortName(char name)
    {
      return QChar::isLetter(name);
    }

    /*! \brief Check if \a name is a valid name for a option
     *
     * A valid option name must:
     * - Not be empty
     * - Not begin with a dash
     * - Not contain any slash
     * - Not contain any equal
     */
    static
    bool isValidName(const QString & name)
    {
      const QString cleanName = name.trimmed();
      if( cleanName.isEmpty() ){
        return false;
      }
      if( cleanName.startsWith( QLatin1Char('-') ) ){
        return false;
      }
      if( cleanName.contains( QLatin1Char('/') ) ){
        return false;
      }
      if( cleanName.contains( QLatin1Char('=') ) ){
        return false;
      }

      return true;
    }

   private:

    char mShortName = '\0';
    QString mName;
    QString mDescription;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_OPTION_H
