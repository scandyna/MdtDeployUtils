/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiler binaries
 **
 ** Copyright (C) 2020-2023 Philippe Steinmann.
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
#include <Mdt/DeployUtils/ExecutableFileInstaller.h>
#include <Mdt/DeployUtils/Platform.h>
#include <QString>
#include <QTemporaryDir>
#include <iostream>
#include <cassert>

using namespace Mdt::DeployUtils;

int main(int argc, char **argv)
{
  assert( argc == 1 );

  QTemporaryDir destinationDir;
  if( !destinationDir.isValid() ){
    std::cerr << "could not create temp dir " << destinationDir.path().toStdString() << std::endl;
    return 1;
  }

  ExecutableFileInstaller installer( Platform::nativePlatform() );

  const auto executable = ExecutableFileToInstall::fromFilePath( QString::fromLocal8Bit(argv[0]) );
  RPath installRPath;
  installRPath.appendPath( QLatin1String("../lib") );

  installer.install(executable, destinationDir.path(), installRPath);

  return 0;
}
