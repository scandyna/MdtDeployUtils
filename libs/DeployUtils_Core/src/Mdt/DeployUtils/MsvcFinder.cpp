/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2022 Philippe Steinmann.
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
#include <QStringBuilder>
#include <QFileInfoList>
#include <cassert>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

MsvcFinder::MsvcFinder(QObject* parent)
 : AbstractCompilerFinderEngine(parent)
{
}

// QString MsvcFinder::findMsvcRoot(const MsvcVersion & version)
// {
//   assert( !version.isNull() );
//
//   return QString();
// }

bool MsvcFinder::doIsSupportedCompiler(const QFileInfo & executablePath) const noexcept
{
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

PathList MsvcFinder::doFindRedistDirectories(ProcessorISA cpu, BuildType buildType) const
{
  PathList pathList;

  pathList.appendPath( findRedistDirectory(cpu, buildType) );

  if( useReleaseRedist(buildType) ){
    pathList.appendPath( findWindowsUcrtRedistDirectory(cpu) );
  }

  return pathList;
}

QString MsvcFinder::findRedistDirectory(ProcessorISA cpu, BuildType buildType) const
{
  /*
   * Example of MSVC 2017 installation:
   * C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Redist\MSVC\14.12.25810\x64\Microsoft.VC<version>.CRT
   */

  /// \todo See https://github.com/dotnet/runtime/issues/40131

  /// \todo Should we distribute what is in onecore sub-dir if exists ?

  QDir dir = QDir( QDir::cleanPath( mVcInstallDir % QLatin1String("/Redist/MSVC") ) );
  if( !dir.exists() ){
    const QString msg = tr("could not find MSVC redist directory: '%1'").arg( dir.absolutePath() );
    throw FindCompilerError(msg);
  }

  QDir redistDir = findLatestVersionDirContainingDebugNonRedist(dir);
  assert( redistDir.exists() );

  if( useDebugRedist(buildType) ){
    if( !redistDir.cd( QLatin1String("debug_nonredist") ) ){
      const QString msg = tr("could not find debug_nonredist directory in '%1'").arg( redistDir.absolutePath() );
      throw FindCompilerError(msg);
    }
  }

  if( !redistDir.cd( processorISADirectoryName(cpu) ) ){
    const QString msg = tr("could not find '%1' directory in '%2'")
                        .arg( processorISADirectoryName(cpu), redistDir.absolutePath() );
    throw FindCompilerError(msg);
  }

  const QDir vcCrtDir = findLatestVcCrtDirectory(redistDir, buildType);
  assert( vcCrtDir.exists() );

  return vcCrtDir.absolutePath();
}

QString MsvcFinder::findWindowsUcrtRedistDirectory(ProcessorISA cpu) const
{
  /*
   * Example of Windows 10 ucrt installation:
   * C:\Program Files (x86)\Windows Kits\10\Redist\ucrt\DLLs\<arch>
   */

  QDir dir = QDir( QLatin1String("C:/Program Files (x86)/Windows Kits/10/Redist/ucrt/DLLs") );
  if( !dir.exists() ){
    const QString msg = tr("could not find Windows Universal C Runtime (ucrt) redist directory: '%1'").arg( dir.absolutePath() );
    throw FindCompilerError(msg);
  }

  if( !dir.cd( processorISADirectoryName(cpu) ) ){
    const QString msg = tr("could not find Windows Universal C Runtime (ucrt) redist directory: '%1'").arg( dir.absolutePath() );
    throw FindCompilerError(msg);
  }

  return dir.absolutePath();
}

bool MsvcFinder::isDirectoryContainingDebugNonRedist(const QFileInfo & fi) noexcept
{
  assert( fi.isDir() );

  const QFileInfo debugSubDirFi = QDir::cleanPath( fi.absoluteFilePath() % QLatin1String("/debug_nonredist") );
  if( !debugSubDirFi.isDir() ){
    return false;
  }
  if( !debugSubDirFi.exists() ){
    return false;
  }

  return true;
}

QDir MsvcFinder::findLatestVersionDirContainingDebugNonRedist(const QDir & dir)
{
  const QFileInfoList versionSubDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::Reversed);
  for(const QFileInfo & fi : versionSubDirs){
    if( isDirectoryContainingDebugNonRedist(fi) ){
      return fi.absoluteFilePath();
    }
  }

  // throw here because QDir() will return current path
  const QString msg = tr("could not find a redist version directory in: '%1'").arg( dir.absolutePath() );
  throw FindCompilerError(msg);
}

QDir MsvcFinder::findLatestVcCrtDirectory(const QDir & dir, BuildType buildType)
{
  assert( dir.exists() );
  assert( dir.isReadable() );

  QStringList filters;
  if( useDebugRedist(buildType) ){
    filters << QLatin1String("Microsoft.VC*.DebugCRT");
  }else{
    filters << QLatin1String("Microsoft.VC*.CRT");
  }

  const QFileInfoList subDirs = dir.entryInfoList(filters, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::Reversed);
  if( subDirs.isEmpty() ){
    // throw here because QDir() will return current path
    const QString msg = tr("could not find any VC CRT directory in: '%1'").arg( dir.absolutePath() );
    throw FindCompilerError(msg);
  }

  return subDirs.first().absoluteFilePath();
}

QString MsvcFinder::processorISADirectoryName(ProcessorISA cpu) noexcept
{
  switch(cpu){
    case ProcessorISA::X86_32:
      return QLatin1String("x86");
    case ProcessorISA::X86_64:
      return QLatin1String("x64");
    case ProcessorISA::Unknown:
      break;
  }

  return QString();
}

bool MsvcFinder::useDebugRedist(BuildType buildType) noexcept
{
  return buildType == BuildType::Debug;
}

bool MsvcFinder::useReleaseRedist(BuildType buildType) noexcept
{
  return !useDebugRedist(buildType);
}

}} // namespace Mdt{ namespace DeployUtils{
