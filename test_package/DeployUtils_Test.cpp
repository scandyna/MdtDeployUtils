/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiler binaries
 **
 ** Copyright (C) 2020-2021 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include <Mdt/DeployUtils/ExecutableFileReader.h>
#include <QString>
#include <cassert>

int main(int argc, char **argv)
{
  using Mdt::DeployUtils::ExecutableFileReader;

  assert( argc == 1 );

  const QString executableFile = QString::fromLocal8Bit(argv[0]);
  int retVal = 1;

  ExecutableFileReader reader;
  reader.openFile(executableFile);
  if( reader.isExecutableOrSharedLibrary() ){
    retVal = 0;
  }else{
    retVal = 1;
  }
  reader.close();

  return retVal;
}
