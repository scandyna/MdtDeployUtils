/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
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
 ****************************************************************************/
#include "DeployUtilsBashCompletionGenerator.h"
#include "CommandLineParser.h"
#include "Mdt/CommandLineParser/BashCompletionGenerator.h"
#include "Mdt/CommandLineParser/BashCompletionGeneratorCommand.h"
#include <QLatin1String>
#include <cassert>

#include <iostream>

void generateBashCompletionScript(const std::string & directoryPath)
{
  assert( !directoryPath.empty() );

  CommandLineParser parser;

  const auto generator = Mdt::CommandLineParser::BashCompletionGenerator::fromParserDefinition( parser.parserDefinition() );

  std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;
  std::cout << "generating to " << directoryPath << std::endl;

  generator.generateScriptToFile(directoryPath);
}
