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

  const auto it = findIteratorByLibraryName(name);

  return it != cend();
}

std::optional<BinaryDependenciesResultLibrary>
BinaryDependenciesResult::findLibraryByName(const QString & name) const noexcept
{
  assert( !name.trimmed().isEmpty() );

  const auto it = findIteratorByLibraryName(name);

  if( it == cend() ){
    return {};
  }

  return *it;
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

BinaryDependenciesResult::const_iterator
BinaryDependenciesResult::findIteratorByLibraryName(const QString & name) const noexcept
{
  const OperatingSystem os = mOs;
  const auto pred = [&name, os](const BinaryDependenciesResultLibrary & library){
    return Impl::BinaryDependencies::fileNamesAreEqual(library.libraryName(), name, os);
  };

  return std::find_if(mEntries.cbegin(), mEntries.cend(), pred);
}


std::vector<BinaryDependenciesResultLibrary>
getLibrariesToRedistribute(const BinaryDependenciesResult & result) noexcept
{
  assert( result.isSolved() );

  std::vector<BinaryDependenciesResultLibrary> libraries;

  for(const BinaryDependenciesResultLibrary & library : result){
    if( !library.shouldNotBeRedistributed() ){
      assert( library.isFound() );
      libraries.push_back(library);
    }
  }

  return libraries;
}

}} // namespace Mdt{ namespace DeployUtils{
