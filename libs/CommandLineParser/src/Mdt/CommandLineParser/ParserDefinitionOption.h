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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_OPTION_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_OPTION_H

#include "Algorithm.h"
#include "ValueType.h"
#include "mdt_commandlineparser_export.h"
#include <QChar>
#include <QLatin1Char>
#include <QString>
#include <QLatin1String>
#include <QLatin1Char>
#include <QStringList>
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
    explicit ParserDefinitionOption(const QString & name, const QString & description)
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
    explicit ParserDefinitionOption(char shortName, const QString & name, const QString & description)
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

    /*! \brief Set the type of value this option may hold
     */
    void setValueType(ValueType type) noexcept
    {
      mValueType = type;
    }

    /*! \brief Get the type of value this option may hold
     */
    ValueType valueType() const noexcept
    {
      return mValueType;
    }

    /*! \brief Set the name of the expected value
     *
     * Options without a value assigned have a boolean-like behavior: either the user specifies --option or they don't.
     *
     * Options with a value name assigned, for example \a file,
     * will appear as -o, --output <file> in the help text.
     */
    void setValueName(const QString & valueName)
    {
      mValueName = valueName;
    }

    /*! \brief Check if this option hase a value name
     */
    bool hasValueName() const noexcept
    {
      return !mValueName.isEmpty();
    }

    /*! \brief Returns the name of the expected value
     *
     * If empty, the option doesn't take a value.
     */
    const QString & valueName() const noexcept
    {
      return mValueName;
    }

    /*! \brief Set the possible values
     *
     * \sa possibleValues()
     * \sa setDefaultValue()
     */
    void setPossibleValues(const QStringList & values) noexcept
    {
      mPossibleValues = values;
    }

    /*! \brief Check if this option has possible values
     */
    bool hasPossibleValues() const noexcept
    {
      return !mPossibleValues.isEmpty();
    }

    /*! \brief Get the possible values
     *
     * \sa setPossibleValues()
     */
    const QStringList & possibleValues() const noexcept
    {
      return mPossibleValues;
    }

    /*! \brief Sets the default value used for this option
     *
     * The default value is used if the user of the application does not specify the option on the command line.
     *
     * \sa setDefaultValues()
     * \sa defaultValues()
     */
    void setDefaultValue(const QString & value)
    {
      mDefaultValues = QStringList{value};
    }

    /*! \brief Sets the list of default values used for this option
     *
     * The default values are used if the user of the application does not specify the option on the command line.
     */
    void setDefaultValues(const QStringList & values)
    {
      mDefaultValues = values;
    }

    /*! \brief Check if this option has at least one default value
     */
    bool hasDefaultValues() const noexcept
    {
      return !mDefaultValues.isEmpty();
    }

    /*! \brief Get the default values for this option
     */
    const QStringList & defaultValues() const noexcept
    {
      return mDefaultValues;
    }

    /*! \brief Check if \a name is a valid short name for a option
     *
     * A valid short name is a letter
     */
    static
    bool isValidShortName(char name)
    {
      return QChar( QLatin1Char(name) ).isLetter();
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
    QString mValueName;
    QStringList mPossibleValues;
    QStringList mDefaultValues;
    ValueType mValueType = ValueType::Unspecified;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_OPTION_H
