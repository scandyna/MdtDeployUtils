/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2022 Philippe Steinmann.
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
#include "PathList.h"
#include <QList>
#include <QtGlobal>
#include <QByteArray>
#include <QLibraryInfo>
#include <QDir>
#include <QChar>
#include <algorithm>
#include <cassert>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{

void PathList::appendPath(const QString& path)
{
  assert( !path.trimmed().isEmpty() );

  mList.removeAll(path);
  mList.append(path);
}

void PathList::appendPathList(const PathList & pathList)
{
  for(const auto & path : pathList){
    if(!path.trimmed().isEmpty()){
      appendPath(path);
    }
  }
}

void PathList::prependPath(const QString& path)
{
  assert( !path.trimmed().isEmpty() );

  mList.removeAll(path);
  mList.prepend(path);
}

void PathList::prependPathList(const PathList& pathList)
{
  const PathList existingPaths = *this;
  clear();
  for(const auto & path : pathList){
    if(!path.trimmed().isEmpty()){
      appendPath(path);
    }
  }
  appendPathList(existingPaths);
}

void PathList::removeNonExistingDirectories() noexcept
{
  const auto pred = [](const QString & path){
    return !QDir(path).exists();
  };

  mList.erase( std::remove_if(mList.begin(), mList.end(), pred) , mList.end() );
}

void PathList::clear()
{
  mList.clear();
}

bool PathList::containsPath(const QString & path) const
{
  return mList.contains(path);
}

PathList PathList::getSystemExecutablePathList()
{
  PathList pathList;

  const auto pathEnv = QString::fromLocal8Bit( qgetenv("PATH") );
#ifdef Q_OS_WIN
  const auto separator = QChar::fromLatin1(';');
#else
  const auto separator = QChar::fromLatin1(':');
#endif
  const auto rawPathList = pathEnv.split(separator, QString::SkipEmptyParts);
  for(const auto & path : rawPathList){
    pathList.appendPath( QDir::cleanPath(path) );
  }

  return pathList;
}

PathList PathList::getSystemLibraryPathList()
{
  /*
   * QLibraryInfo helps to find where Qt libraries are installed (I guess)
   * For the other, we guess what system library paths could exist
   */
#ifdef Q_OS_UNIX
  PathList pathList = getSystemLibraryKnownPathListLinux( Platform::nativeProcessorISA() );
#endif // #ifdef Q_OS_UNIX
#ifdef Q_OS_WIN
//  PathList pathList = getSystemLibraryKnownPathListWindows();
//  pathList.appendPathList( getSystemExecutablePathList() );
  PathList pathList = getSystemExecutablePathList();
#endif // #ifdef Q_OS_WIN
  pathList.appendPath( QLibraryInfo::location(QLibraryInfo::LibrariesPath) );

  return pathList;
}

PathList PathList::getSystemLibraryKnownPathList(const Platform & platform)
{
  assert( !platform.isNull() );

  switch( platform.operatingSystem() ){
    case OperatingSystem::Linux:
      return getSystemLibraryKnownPathListLinux( platform.processorISA() );
    case OperatingSystem::Windows:
      return getSystemLibraryKnownPathListWindows( platform.processorISA() );
    case OperatingSystem::Unknown:
      return PathList();
  }

  return PathList();
}

PathList PathList::getSystemLibraryKnownPathListLinux(ProcessorISA processorISA) noexcept
{
  assert( processorISA != ProcessorISA::Unknown );

  switch(processorISA){
    case ProcessorISA::X86_32:
      return PathList{
        QString::fromLatin1("/usr/lib"),
        QString::fromLatin1("/usr/lib32"),
        QString::fromLatin1("/usr/libx32"),
        QString::fromLatin1("/lib"),
        QString::fromLatin1("/lib32"),
        QString::fromLatin1("/libx32")
      };
    case ProcessorISA::X86_64:
      return PathList{
        QString::fromLatin1("/usr/lib"),
        QString::fromLatin1("/usr/lib/x86_64-linux-gnu"),
        QString::fromLatin1("/lib"),
        QString::fromLatin1("/lib64"),
      };
    case ProcessorISA::Unknown:
      break;
  }

  return PathList();
}

/*
 * See: https://stackoverflow.com/questions/949959/why-do-64-bit-dlls-go-to-system32-and-32-bit-dlls-to-syswow64-on-64-bit-windows
 */
PathList PathList::getSystemLibraryKnownPathListWindows(ProcessorISA processorISA) noexcept
{
  assert( processorISA != ProcessorISA::Unknown );

  switch(processorISA){
    case ProcessorISA::X86_32:
      return PathList{
        QString::fromLatin1("/windows/syswow64"),
        QString::fromLatin1("/windows/system32"),
        QString::fromLatin1("/windows/system")
      };
    case ProcessorISA::X86_64:
      return PathList{
        QString::fromLatin1("/windows/system32"),
        QString::fromLatin1("/windows/system")
      };
    case ProcessorISA::Unknown:
      break;
  }

  return PathList();
}

}} // namespace Mdt{ namespace DeployUtils{
