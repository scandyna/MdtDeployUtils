/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2022 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ***********************************************************************************************/
#ifndef COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_COMMAND_LINE_PARSER_DEFINITION_H
#define COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_COMMAND_LINE_PARSER_DEFINITION_H

#include "Mdt/CommandLineParser/ParserDefinitionCommand.h"
#include "Mdt/CommandLineParser/ParserDefinitionOption.h"
#include <QObject>
#include <QString>
#include <cassert>

/*! \brief Parser definition for CopySharedLibrariesTargetDependsOn
 */
class CopySharedLibrariesTargetDependsOnCommandLineParserDefinition : public QObject
{
  Q_OBJECT

 public:

  /*! \brief Construct a command line parser
   */
  explicit CopySharedLibrariesTargetDependsOnCommandLineParserDefinition(QObject *parent = nullptr) noexcept;

  /*! \brief Set application name
   */
  void setApplicationName(const QString & name) noexcept
  {
    mApplicationName = name;
  }

  /*! \brief Setup the definition
   *
   * \pre application name must have been set
   * \sa setApplicationName()
   */
  void setup() noexcept;

  /*! \brief Get the overwrite behavior Option
   *
   * \pre setup must have been done before
   * \sa setup()
   */
  const Mdt::CommandLineParser::ParserDefinitionOption & overwriteBehaviorOption() const noexcept
  {
    assert( mCommand.hasOptions() );

    return mCommand.optionAt(1);
  }

  /*! \brief Get the remove RPATH Option
   *
   * \pre setup must have been done before
   * \sa setup()
   */
  const Mdt::CommandLineParser::ParserDefinitionOption & removeRpathOption() const noexcept
  {
    assert( mCommand.hasOptions() );

    return mCommand.optionAt(2);
  }

  /*! \brief Get the search prefix path list option
   *
   * \pre setup must have been done before
   * \sa setup()
   */
  const Mdt::CommandLineParser::ParserDefinitionOption & searchPrefixPathListOption() const noexcept
  {
    assert( mCommand.hasOptions() );

    return mCommand.optionAt(3);
  }

  /*! \brief Get the compiler location option
   *
   * \pre setup must have been done before
   * \sa setup()
   */
  const Mdt::CommandLineParser::ParserDefinitionOption & compilerLocationOption() const noexcept
  {
    assert( mCommand.hasOptions() );

    return mCommand.optionAt(4);
  }

  /*! \brief Get the internal parser definition command
   */
  const Mdt::CommandLineParser::ParserDefinitionCommand & command() const noexcept
  {
    return mCommand;
  }

 private:

  QString mApplicationName;
  Mdt::CommandLineParser::ParserDefinitionCommand mCommand;
};

#endif // #ifndef COPY_SHARED_LIBRARIES_TARGET_DEPENDS_ON_COMMAND_LINE_PARSER_DEFINITION_H
