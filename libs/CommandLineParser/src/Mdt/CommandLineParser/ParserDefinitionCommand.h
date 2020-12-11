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
#ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H
#define MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H

#include "ParserDefinitionOption.h"
#include "ParserDefinitionPositionalArgument.h"
#include "mdt_commandlineparser_export.h"
#include <QString>
#include <cassert>

namespace Mdt{ namespace CommandLineParser{

  /*! \brief Representation of a command in a ParserDefinition
   */
  class MDT_COMMANDLINEPARSER_EXPORT ParserDefinitionCommand
  {
   public:

    /*! \brief Construct a command
     */
    ParserDefinitionCommand() noexcept = default;

    /*! \brief Construct a command with a name
     *
     * \pre \a name must not be empty
     */
    ParserDefinitionCommand(const QString & name)
    {
      assert( !name.trimmed().isEmpty() );
    }

    /*! \brief Check if this command has a name
     */
    bool hasName() const noexcept
    {
//       return !mName.empty();
    }

    QString name() const
    {
    }

    /*! \brief Check if this command is empty
     *
     * Return true if this command has no arguments and no options
     */
    bool isEmpty() const noexcept
    {
//       return mArguments.empty() && mOptions.empty();
    }

    /*! \brief Set the description of this command
     */
    void setDescription(const QString & description)
    {
    }

    /*! \brief Add a option
     *
     * \todo precontidions ?
     */
    void addOption(const ParserDefinitionOption & option);

    /*! \brief Add a option
     *
     * \pre \a name must not be empty
     * \pre \a name must not begin with a dash or a slash
     * \pre \a name must not contain any equal
     *
     * \todo enforce those precontidions
     */
    void addOption(const QString & name, const QString & description);

    /*! \brief Add a option
     *
     * \pre \a shortName must be a letter [a-z]
     * \pre \a name must not be empty
     * \pre \a name must not begin with a dash or a slash
     * \pre \a name must not contain any equal
     *
     * \todo enforce those precontidions
     */
    void addOption(char shortName, const QString & name, const QString & description);

    /*! \brief Add the help option
     */
    void addHelpOption();

    /*! \brief Add a positional argument
     *
     * \pre \a name must not be empty
     */
    void addPositionalArgument(const QString & name, const QString & description, const QString & sytax = QString())
    {
      assert( !name.trimmed().isEmpty() );
    }

  };

}} // namespace Mdt{ namespace CommandLineParser{

#endif // #ifndef MDT_COMMAND_LINE_PARSER_PARSER_DEFINITION_COMMAND_H
