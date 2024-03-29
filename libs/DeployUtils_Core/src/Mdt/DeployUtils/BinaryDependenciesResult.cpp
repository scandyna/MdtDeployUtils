// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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

void BinaryDependenciesResult::addFoundLibrary(const QFileInfo & library, const RPath & rpath) noexcept
{
  assert( fileInfoIsAbsolutePath(library) );

  const auto resultLibrary = BinaryDependenciesResultLibrary::fromFoundLibrary(library, rpath);

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
    if( library.isToRedistribute() ){
      assert( library.isFound() );
      libraries.push_back(library);
    }
  }

  return libraries;
}

QStringList
getLibrariesToRedistributeFilePathList(const BinaryDependenciesResult & result) noexcept
{
  assert( result.isSolved() );

  QStringList libraries;

  for(const BinaryDependenciesResultLibrary & library : result){
    if( library.isToRedistribute() ){
      assert( library.isFound() );
      libraries.push_back( library.absoluteFilePath() );
    }
  }

  return libraries;
}

}} // namespace Mdt{ namespace DeployUtils{
