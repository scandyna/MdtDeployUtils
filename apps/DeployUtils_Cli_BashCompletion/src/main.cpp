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
#include <iostream>
#include <string>
#include <cassert>

int main(int argc, char **argv)
{
  if(argc != 2){
    assert( argc >= 1 );
    std::cerr << argv[0] << ": missing file path" << std::endl;
    return 1;
  }
  assert( argc >= 1 );

  const std::string filePath = argv[1];

  /// \todo Exceptions
  generateBashCompletionScript(filePath);

  return 0;
}
