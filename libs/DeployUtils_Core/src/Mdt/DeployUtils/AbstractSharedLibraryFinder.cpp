/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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

  return library;
}

bool AbstractSharedLibraryFinder::validateIsExistingValidSharedLibrary(const QFileInfo & libraryFile)
{
  assert( fileInfoIsAbsolutePath(libraryFile) );

  if( !mIsExistingValidShLibOp->isExistingValidSharedLibrary(libraryFile) ){
    return false;
  }
  if( !validateSpecificSharedLibrary(libraryFile) ){
    return false;
  }

  return true;
}

bool AbstractSharedLibraryFinder::validateSpecificSharedLibrary(const QFileInfo &)
{
  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
