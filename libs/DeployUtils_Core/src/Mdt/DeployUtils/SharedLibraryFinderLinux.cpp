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
#include "SharedLibraryFinderLinux.h"
#include "SearchPathList.h"
#include "Mdt/DeployUtils/Impl/LibraryExcludeListLinux.h"

namespace Mdt{ namespace DeployUtils{

SharedLibraryFinderLinux::SharedLibraryFinderLinux(const Impl::AbstractIsExistingSharedLibrary & isExistingShLibOp, QObject *parent) noexcept
 : AbstractSharedLibraryFinder(isExistingShLibOp, parent)
{
}

void SharedLibraryFinderLinux::buildSearchPathList(const PathList & searchFirstPathPrefixList, ProcessorISA processorISA) noexcept
{
  PathList searchPathList;

  SearchPathList searchFirstPathList;
  searchFirstPathList.setIncludePathPrefixes(true);
  searchFirstPathList.setPathSuffixList({QLatin1String("lib"),QLatin1String("qt5/lib")});
  searchFirstPathList.setPathPrefixList(searchFirstPathPrefixList);

  searchPathList.appendPathList( searchFirstPathList.pathList() );
  searchPathList.appendPathList( PathList::getSystemLibraryKnownPathListLinux(processorISA) );
  searchPathList.removeNonExistingDirectories();

  setSearchPathList(searchPathList);
}

QString SharedLibraryFinderLinux::makeDirectoryFromRpathEntry(const BinaryDependenciesFile & originFile, const RPathEntry & rpathEntry) noexcept
{
//   assert( originFile.hasAbsoluteFilePath() );

  if( rpathEntry.isRelative() ){
    return QDir::cleanPath( originFile.absoluteDirectoryPath() + QLatin1Char('/') + rpathEntry.path() );
  }

  return rpathEntry.path();
}

BinaryDependenciesFileList SharedLibraryFinderLinux::doFindLibrariesAbsolutePath(BinaryDependenciesFile & file) const
{
  BinaryDependenciesFileList libraries;

  removeLibrariesInExcludeList(file);

  for( const QString & libraryName : file.dependenciesFileNames() ){
    libraries.push_back( findLibraryAbsolutePath(file, libraryName) );
  }

  return libraries;
}

void SharedLibraryFinderLinux::removeLibrariesInExcludeList(BinaryDependenciesFile & file) noexcept
{
  const auto pred = [](const QString & libraryName){
    return libraryExcludelistLinux.contains(libraryName);
  };

  file.removeDependenciesFileNames(pred);
}

}} // namespace Mdt{ namespace DeployUtils{
