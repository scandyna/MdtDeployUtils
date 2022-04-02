/*******************************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#ifndef COMMON_COMMAND_LINE_PARSER_DEFINITION_OPTIONS_H
#define COMMON_COMMAND_LINE_PARSER_DEFINITION_OPTIONS_H

#include "Mdt/CommandLineParser/ParserDefinitionOption.h"
#include <QObject>
#include <QString>
#include <cassert>

/*! \brief Helper to create commonly used command-line options
 */
class CommonCommandLineParserDefinitionOptions : public QObject
{
  Q_OBJECT

 public:

  static
  Mdt::CommandLineParser::ParserDefinitionOption makeCompilerLocationOption() noexcept;

  static
  Mdt::CommandLineParser::ParserDefinitionOption makeSearchPrefixPathListOption() noexcept;

  static
  Mdt::CommandLineParser::ParserDefinitionOption makePathListSeparatorOption() noexcept;
};

#endif // #ifndef COMMON_COMMAND_LINE_PARSER_DEFINITION_OPTIONS_H
