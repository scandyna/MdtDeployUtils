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
#include "Mdt/DeployUtils/SharedLibraryFinderLinux.h"
#include "BinaryDependenciesTestCommon.h"
#include <vector>
#include <string>

using namespace Mdt::DeployUtils;

inline
RPath makeRPathFromUtf8Paths(const std::vector<std::string> & paths)
{
  RPath rpath;

  for(const std::string & path : paths){
    rpath.appendPath( QString::fromStdString(path) );
  }

  return rpath;
}

inline
BinaryDependenciesFile findLibraryAbsolutePath(const BinaryDependenciesFile & originFile,
                                               const QString & libraryName,
                                               const PathList & searchPathList,
                                               TestIsExistingSharedLibrary & isExistingSharedLibraryOp)
{
  return SharedLibraryFinderLinux::findLibraryAbsolutePath(originFile, libraryName, searchPathList, isExistingSharedLibraryOp);
}

inline
BinaryDependenciesFileList findLibrariesAbsolutePath(BinaryDependenciesFile & file,
                                                     const PathList & searchPathList,
                                                     TestIsExistingSharedLibrary & isExistingSharedLibraryOp)
{
  return SharedLibraryFinderLinux::findLibrariesAbsolutePath(file, searchPathList, isExistingSharedLibraryOp);
}

inline
QString makeDirectoryFromRpathEntry(const BinaryDependenciesFile & originFile, const RPathEntry & rpathEntry) noexcept
{
  return SharedLibraryFinderLinux::makeDirectoryFromRpathEntry(originFile, rpathEntry);
}
