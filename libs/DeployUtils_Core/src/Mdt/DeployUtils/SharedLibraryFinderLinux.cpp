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
#include <QLatin1String>

namespace Mdt{ namespace DeployUtils{

SharedLibraryFinderLinux::SharedLibraryFinderLinux(const AbstractIsExistingValidSharedLibrary & isExistingValidShLibOp,
                                                   std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                                   QObject *parent) noexcept
 : SharedLibraryFinderCommon(isExistingValidShLibOp, qtDistributionDirectory, parent)
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

BinaryDependenciesFile SharedLibraryFinderLinux::findLibraryAbsolutePathByRPath(const QString & libraryName,
                                                                                const BinaryDependenciesFile & dependentFile) const
{
  assert( !libraryName.trimmed().isEmpty() );

  emit verboseMessage(
    tr(" searching %1 by RPath given in %2").arg( libraryName, dependentFile.fileName() )
  );

  for( const auto & rpathEntry : dependentFile.rPath() ){
    const QString directory = makeDirectoryFromRpathEntry(dependentFile, rpathEntry);
    const QFileInfo libraryFile(directory, libraryName);
    emit debugMessage(
      tr("  try %1").arg( libraryFile.absoluteFilePath() )
    );
    if( isExistingValidSharedLibrary(libraryFile) ){
      return BinaryDependenciesFile::fromQFileInfo(libraryFile);
    }
  }

  return BinaryDependenciesFile();
}

BinaryDependenciesFile SharedLibraryFinderLinux::findLibraryAbsolutePath_OLD(const QString & libraryName,
                                                                         const BinaryDependenciesFile & dependentFile) const
{
  assert( !libraryName.trimmed().isEmpty() );

  BinaryDependenciesFile library = findLibraryAbsolutePathByRPath(libraryName, dependentFile);

  if( library.isNull() ){
    library = findLibraryAbsolutePathBySearchPath(libraryName);
  }

  if( library.isNull() ){
    const QString message = tr("could not find the absolute path for %1")
                            .arg(libraryName);
    throw FindDependencyError(message);
  }

  return library;
}

bool SharedLibraryFinderLinux::doLibraryShouldBeDistributed(const QString & libraryName) const noexcept
{
  if( libraryIsInLocalExcludeList(libraryName) ){
    return false;
  }
  if( libraryIsInGeneratedExcludeList(libraryName) ){
    return false;
  }

  return true;
}

bool SharedLibraryFinderLinux::libraryIsInLocalExcludeList(const QString & libraryName) noexcept
{
  static
  const QStringList excludeList{
    /*
     * libdbus-1.so.3
     *
     * it is located in /lib/x86_64-linux-gnu (Ubuntu 18.04)
     * that lets think that it is strongly dependent of the system
     *
     * See also https://github.com/AppImage/pkg2appimage/issues/450
     */
    QLatin1String("libdbus-1.so.3"),
    /*
     * next libraries are installed in
     * /lib/x86_64-linux-gnu (Ubuntu 18.04)
     *
     * don't know if those must be redistributed or not,
     * should test that in some way
     */
    QLatin1String("libkeyutils.so.1"),
    QLatin1String("libbz2.so.1.0"),
    QLatin1String("liblzma.so.5"),
    QLatin1String("libudev.so.1")
  };

  return excludeList.contains(libraryName);
}

bool SharedLibraryFinderLinux::libraryIsInGeneratedExcludeList(const QString & libraryName) noexcept
{
  return libraryExcludelistLinux.contains(libraryName);
}

QFileInfo SharedLibraryFinderLinux::doFindLibraryAbsolutePath(const QString & libraryName, const BinaryDependenciesFile & dependentFile) const
{
  assert( !libraryName.trimmed().isEmpty() );

  /** \todo See if we keep using BinaryDependenciesFile or not here
   *
   * Seems better than a naked QFileInfo
   * \sa https://gitlab.com/scandyna/mdtdeployutils/-/issues/5 (BinaryDependenciesFile has to much attributes and is confusing)
   */

  BinaryDependenciesFile library = findLibraryAbsolutePathByRPath(libraryName, dependentFile);

  if( library.isNull() ){
    library = findLibraryAbsolutePathBySearchPath(libraryName);
  }

  if( library.isNull() ){
    const QString message = tr("could not find the absolute path for %1")
                            .arg(libraryName);
    throw FindDependencyError(message);
  }

  return QFileInfo( library.fileInfo() );
}

BinaryDependenciesFile SharedLibraryFinderLinux::findLibraryAbsolutePathBySearchPath(const QString & libraryName) const
{
  assert( !libraryName.trimmed().isEmpty() );

  emit verboseMessage(
    tr(" searching %1 in search path list").arg(libraryName)
  );

  for( const QString & directory : searchPathList() ){
    QFileInfo libraryFile(directory, libraryName);
    emit debugMessage(
      tr("  try %1").arg( libraryFile.absoluteFilePath() )
    );
    if( isExistingValidSharedLibrary(libraryFile) ){
      return BinaryDependenciesFile::fromQFileInfo(libraryFile);
    }
  }

  return BinaryDependenciesFile();
}

void SharedLibraryFinderLinux::removeLibrariesToNotRedistribute(BinaryDependenciesFile & file) const noexcept
{
  removeLibrariesInGeneratedExcludeList(file);
  removeLibrariesInLocalExcludeList(file);
}

void SharedLibraryFinderLinux::removeLibrariesInLocalExcludeList(BinaryDependenciesFile & file) noexcept
{

  const auto pred = [](const QString & libraryName){
    return libraryIsInLocalExcludeList(libraryName);
//     return excludeList.contains(libraryName);
  };

  file.removeDependenciesFileNames(pred);
}

void SharedLibraryFinderLinux::removeLibrariesInGeneratedExcludeList(BinaryDependenciesFile & file) noexcept
{
  const auto pred = [](const QString & libraryName){
    return libraryExcludelistLinux.contains(libraryName);
  };

  file.removeDependenciesFileNames(pred);
}

}} // namespace Mdt{ namespace DeployUtils{
