/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2015-2021 Philippe Steinmann.
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
#include "BinaryDependencies.h"
#include "Mdt/DeployUtils/Impl/BinaryDependencies.h"
#include "ExecutableFileReader.h"
#include <QDir>
#include <cassert>

// #include <QDebug>
#include <iostream>

namespace Mdt{ namespace DeployUtils{

BinaryDependencies::BinaryDependencies(QObject* parent)
 : QObject(parent)
{
}

QStringList BinaryDependencies::findDependencies(const QFileInfo & binaryFilePath, const PathList & searchFirstPathPrefixList)
{
  using Impl::ExecutableFileInfo;
  using Impl::ExecutableFileInfoList;

  assert( !binaryFilePath.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()

  ExecutableFileInfoList dependencies;

  // open file
  ExecutableFileInfo target;
  target.fileName = binaryFilePath.fileName();
  target.directoryPath = binaryFilePath.absoluteDir().path();

  ExecutableFileReader reader;

  /// \todo fix platform
  const PathList searchPathList = Impl::buildSearchPathList( searchFirstPathPrefixList, Platform::nativePlatform() );

  std::cout << "searchPathList:\n" << searchPathList.toStringList().join(QLatin1Char('\n')).toStdString() << std::endl;

  Impl::findDependencies(target, dependencies, searchPathList, reader, Impl::isExistingSharedLibrary);

  return Impl::qStringListFromExecutableFileInfoList(dependencies);
}

}} // namespace Mdt{ namespace DeployUtils{
