/****************************************************************************
 **
 ** MdtDeployUtils - Tools to help deploy C/C++ application binaries and their dependencies.
 **
 ** Copyright (C) 2020-2020 Philippe Steinmann.
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

void generateBashCompletionScript(const std::string& filePath)
{
  assert( !filePath.empty() );

  CommandLineParser parser;
  Mdt::CommandLineParser::BashCompletionGenerator generator;

  generator.setApplicationName( QLatin1String("mdtdeployutils") );

//   const auto mainCommand = BashCompletionGeneratorCommand::mainCommandFromParser( parser.parser() );
//   generator.setMainCommand(mainCommand);

  std::cout << generator.generateScript().toLocal8Bit().toStdString() << std::endl;

  std::cout << "generating to " << filePath << std::endl;
}
