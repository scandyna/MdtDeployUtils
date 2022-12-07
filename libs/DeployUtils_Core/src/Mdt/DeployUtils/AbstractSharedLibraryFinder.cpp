/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#include "AbstractSharedLibraryFinder.h"
#include "AbstractIsExistingValidSharedLibrary.h"
#include "FileInfoUtils.h"

// #include "QtSharedLibraryFile.h"

namespace Mdt{ namespace DeployUtils{

OperatingSystem AbstractSharedLibraryFinder::operatingSystem() const noexcept
{
  const OperatingSystem os = doOperatingSystem();

  assert(os != OperatingSystem::Unknown);

  return os;
}

bool AbstractSharedLibraryFinder::libraryShouldBeDistributed(const QString & libraryName) const noexcept
{
  assert( !libraryName.trimmed().isEmpty() );

  return doLibraryShouldBeDistributed(libraryName);
}

QFileInfo AbstractSharedLibraryFinder::findLibraryAbsolutePath(const QString & libraryName, const BinaryDependenciesFile & dependentFile)
{
  assert( !libraryName.trimmed().isEmpty() );
  assert( libraryShouldBeDistributed(libraryName) );

  const QFileInfo library = doFindLibraryAbsolutePath(libraryName, dependentFile);
  assert( fileInfoIsAbsolutePath(library) );

  performLibrarySpecificAction(library);

  return library;
}

BinaryDependenciesFileList AbstractSharedLibraryFinder::findLibrariesAbsolutePath(BinaryDependenciesFile & file)
{
  BinaryDependenciesFileList libraries;

  removeLibrariesToNotRedistribute(file);

  for( const QString & libraryName : file.dependenciesFileNames() ){
    const BinaryDependenciesFile library = findLibraryAbsolutePath_OLD(libraryName, file);

    performLibrarySpecificAction( library.fileInfo() );

    libraries.push_back(library);
  }

  return libraries;
}

bool AbstractSharedLibraryFinder::isExistingValidSharedLibrary(const QFileInfo & libraryFile) const
{
  assert( !libraryFile.filePath().isEmpty() ); // see doc of QFileInfo::absoluteFilePath()
  assert( libraryFile.isAbsolute() );

  if( !mIsExistingValidShLibOp.isExistingValidSharedLibrary(libraryFile) ){
    return false;
  }
  if( !isValidSpecificSharedLibrary(libraryFile) ){
    return false;
  }

  return true;
}

bool AbstractSharedLibraryFinder::isValidSpecificSharedLibrary(const QFileInfo &) const
{
  return true;
}

void AbstractSharedLibraryFinder::performLibrarySpecificAction(const QFileInfo &)
{
}

}} // namespace Mdt{ namespace DeployUtils{
