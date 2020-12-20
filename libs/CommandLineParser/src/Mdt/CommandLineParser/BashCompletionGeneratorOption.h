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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_OPTION_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_OPTION_H

#include "Algorithm.h"
#include "ParserDefinitionOption.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QLatin1Char>
#include <QLatin1String>
#include <cassert>


namespace Mdt{ namespace CommandLineParser{

  /*! \brief Command line option for BashCompletionGenerator
   */
  class MDT_COMMANDLINEPARSER_EXPORT BashCompletionGeneratorOption
  {
   public:

    /*! \brief Construct a command line option
     *
     * \pre \a shortName must be a character [a-z]
     * \pre \a name must not be empty
     * \pre \a name must not begin with a dash or a slash
     * \pre \a name must not contain any equal
     *
     * \todo enforce those precontidions
     */
    explicit BashCompletionGeneratorOption(char shortName, const QString & name)
    : mShortName(shortName),
      mName(name)
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Construct a command line option
     *
     * \pre \a name must not be empty
     * \pre \a name must not begin with a dash or a slash
     * \pre \a name must not contain any equal
     *
     * \todo enforce those precontidions
     */
    explicit BashCompletionGeneratorOption(const QString & name)
    : mShortName('\0'),
      mName(name)
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Check if this command line option has a short name
     */
    bool hasShortName() const noexcept
    {
      return mShortName != '\0';
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

    /*! \brief Get the name with dashes
     */
    QString nameWithDashes() const
    {
      return QLatin1String("--") + mName;
    }

    /*! \brief Get a generator option from \a option
     */
    static
    BashCompletionGeneratorOption fromParserDefinitionOption(const ParserDefinitionOption & option);

   private:

    char mShortName;
    QString mName;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_OPTION_H
