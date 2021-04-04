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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_POSITIONAL_ARGUMENT_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_POSITIONAL_ARGUMENT_H

#include "ValueType.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <QStringList>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Represents a positional argument for a ParserDefinition or a ParserDefinitionCommand
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserDefinitionPositionalArgument
  {
   public:

    ParserDefinitionPositionalArgument() = delete;

    /*! \brief Construct a argument with a value type, a name, a description and a optional syntax
     *
     * \pre \a name must not be empty
     */
    ParserDefinitionPositionalArgument(ValueType valueType, const QString & name, const QString & description, const QString & syntax = QString())
     : mValueType(valueType),
       mName( name.trimmed() ),
       mDescription( description.trimmed() ),
       mSyntax( syntax.trimmed() )
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Copy construct a argument from \a other
     */
    ParserDefinitionPositionalArgument(const ParserDefinitionPositionalArgument & other) = default;

    /*! \brief Copy assign \a other to this argument
     */
    ParserDefinitionPositionalArgument & operator=(const ParserDefinitionPositionalArgument & other) = default;

    /*! \brief Move construct a argument from \a other
     */
    ParserDefinitionPositionalArgument(ParserDefinitionPositionalArgument && other) = default;

    /*! \brief Move assign \a other to this argument
     */
    ParserDefinitionPositionalArgument & operator=(ParserDefinitionPositionalArgument && other) = default;

    /*! \brief Get the value type of this argument
     */
    ValueType valueType() const noexcept
    {
      return mValueType;
    }

    /*! \brief Get the name of this argument
     */
    const QString & name() const noexcept
    {
      return mName;
    }

    /*! \brief Get the length of the name
     */
    int nameLength() const noexcept
    {
      return mName.length();
    }

    /*! \brief Get the description of this argument
     */
    const QString & description() const noexcept
    {
      return mDescription;
    }

    /*! \brief Check if this argument has a syntax
     */
    bool hasSyntax() const noexcept
    {
      return !mSyntax.isEmpty();
    }

    /*! \brief Get the syntax of this argument
     */
    const QString & syntax() const noexcept
    {
      return mSyntax;
    }

    /*! \brief Set the possible values
     *
     * \sa possibleValues()
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

   private:

    ValueType mValueType;
    QString mName;
    QString mDescription;
    QString mSyntax;
    QStringList mPossibleValues;
  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_POSITIONAL_ARGUMENT_H
