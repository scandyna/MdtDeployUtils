/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#include "MsvcFinder.h"
#include <QLatin1String>
#include <QDir>
#include <cassert>

#include <QDebug>

namespace Mdt{ namespace DeployUtils{

MsvcFinder::MsvcFinder(QObject* parent)
 : AbstractCompilerFinderEngine(parent)
{
}

QString MsvcFinder::findMsvcRoot(const MsvcVersion & version)
{
  assert( !version.isNull() );
  
  return QString();
}

bool MsvcFinder::doIsSupportedCompiler(const QFileInfo & executablePath) const noexcept
{
  qDebug() << "MSVC impl, try " << executablePath.absoluteFilePath();

  return executablePath.baseName() == QLatin1String("cl");
}

void MsvcFinder::doFindFromCxxCompilerPath(const QFileInfo & executablePath)
{
  QDir dir = executablePath.absoluteDir();

  /*
   * Example of MSVC 2017 installation:
   * C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.12.25827\bin\Hostx64\x64\cl.exe
   */
  if( !cdUp(dir, 6) ){
    return;
  }

  mVcInstallDir = dir.absolutePath();
}

}} // namespace Mdt{ namespace DeployUtils{
