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
#ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_OPTION_H
#define MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_OPTION_H

#include "Algorithm.h"
#include "ParserDefinitionOption.h"
#include "BashCompletion/Action.h"
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
     * \pre \a shortName must be a valid option short name
     * \sa isValidShortName()
     * \pre \a name must be a valid option name
     * \sa isValidName()
     */
    explicit BashCompletionGeneratorOption(char shortName, const QString & name)
    : mShortName(shortName),
      mName(name)
    {
      assert( isValidShortName(shortName) );
      assert( isValidName(name) );
    }

    /*! \brief Construct a command line option
     *
     * \pre \a name must be a valid option name
     * \sa isValidName()
     */
    explicit BashCompletionGeneratorOption(const QString & name)
    : mShortName('\0'),
      mName(name)
    {
      assert( isValidName(name) );
    }

    /*! \brief Check if this command line option has a short name
     */
    bool hasShortName() const noexcept
    {
      return mShortName != '\0';
    }

    /*! \brief Get the short name
     *
     * \pre must only be called if this command line option has a short name
     * \sa hasShortName()
     */
    char shortName() const noexcept
    {
      assert( hasShortName() );

      return mShortName;
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

    /*! \brief Get the name
     */
    QString name() const noexcept
    {
      return mName;
    }

    /*! \brief Get the name with dashes
     */
    QString nameWithDashes() const
    {
      return QLatin1String("--") + mName;
    }

    /*! \brief Set the action to complete the value of this option
     *
     * By default, the action is set regarding the parser definition option.
     */
    void setAction(const BashCompletion::Action & action) noexcept
    {
      mAction = action;
    }

    /*! \brief Check if this option has a action attached
     */
    bool hasAction() const noexcept
    {
      return mAction.isDefined();
    }

    /*! \brief Access the action of this option
     */
    const BashCompletion::Action & action() const noexcept
    {
      return mAction;
    }

    /*! \brief Check if \a name is a valid short name for a option
     *
     * \sa ParserDefinitionOption::isValidShortName()
     */
    static
    bool isValidShortName(char name)
    {
      return ParserDefinitionOption::isValidShortName(name);
    }

    /*! \brief Check if \a name is a valid name for a option
     *
     * \sa ParserDefinitionOption::isValidName()
     */
    static
    bool isValidName(const QString & name)
    {
      return ParserDefinitionOption::isValidName(name);
    }

    /*! \brief Get a generator option from \a option
     */
    static
    BashCompletionGeneratorOption fromParserDefinitionOption(const ParserDefinitionOption & option);

   private:

    char mShortName;
    QString mName;
    BashCompletion::Action mAction;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_BASH_COMPLETION_GENERATOR_OPTION_H
