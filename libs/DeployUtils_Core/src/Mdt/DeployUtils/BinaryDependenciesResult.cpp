// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "BinaryDependenciesResult.h"
#include "Impl/BinaryDependencies/FileComparison.h"
#include "FileInfoUtils.h"
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

BinaryDependenciesResult::BinaryDependenciesResult(const QFileInfo & target, OperatingSystem os)
 : mIsSolved(true),
   mOs(os),
   mTarget(target)
{
  assert( fileInfoIsAbsolutePath(mTarget) );
  assert( mOs != OperatingSystem::Unknown );
}

bool BinaryDependenciesResult::containsLibraryName(const QString & name) const noexcept
{
  assert( !name.trimmed().isEmpty() );

  const OperatingSystem os = mOs;
  const auto pred = [&name, os](const BinaryDependenciesResultLibrary & library){
    return Impl::BinaryDependencies::fileNamesAreEqual(library.libraryName(), name, os);
  };

  const auto it = std::find_if(mEntries.cbegin(), mEntries.cend(), pred);

  return it != mEntries.cend();
}

void BinaryDependenciesResult::addFoundLibrary(const QFileInfo & library) noexcept
{
  assert( fileInfoIsAbsolutePath(library) );

  const auto resultLibrary = BinaryDependenciesResultLibrary::fromQFileInfo(library);

  if( !containsLibraryName( resultLibrary.libraryName() ) ){
    mEntries.push_back(resultLibrary);
  }
}

void BinaryDependenciesResult::addNotFoundLibrary(const QFileInfo & library) noexcept
{
  assert( fileInfoHasFileName(library) );

  const auto resultLibrary = BinaryDependenciesResultLibrary::fromQFileInfo(library);
  assert( !resultLibrary.isFound() );

  mIsSolved = false;
  if( !containsLibraryName( resultLibrary.libraryName() ) ){
    mEntries.push_back(resultLibrary);
  }
}

void BinaryDependenciesResult::addLibraryToNotRedistribute(const QFileInfo & library) noexcept
{
  assert( fileInfoHasFileName(library) );

  const auto resultLibrary = BinaryDependenciesResultLibrary::libraryToNotRedistrbuteFromFileInfo(library);

  if( !containsLibraryName( resultLibrary.libraryName() ) ){
    mEntries.push_back(resultLibrary);
  }
}

void BinaryDependenciesResult::addLibrary(const BinaryDependenciesFile & library) noexcept
{
  assert( !library.isNull() );

  const auto resultLibrary = BinaryDependenciesResultLibrary::fromQFileInfo( library.fileInfo() );

  if( !containsLibraryName( resultLibrary.libraryName() ) ){
    mEntries.push_back(resultLibrary);
  }

  if( !resultLibrary.isFound() ){
    mIsSolved = false;
  }
}

}} // namespace Mdt{ namespace DeployUtils{
