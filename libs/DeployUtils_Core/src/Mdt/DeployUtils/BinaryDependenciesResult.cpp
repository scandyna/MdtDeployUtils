// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "BinaryDependenciesResult.h"
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

BinaryDependenciesResult::BinaryDependenciesResult(const QFileInfo & target)
 : mIsSolved(true),
   mTarget(target)
{
  assert( !mTarget.filePath().isEmpty() );
  assert( mTarget.isAbsolute() );
}

bool BinaryDependenciesResult::containsLibraryName(const QString & name) const noexcept
{
  assert( !name.trimmed().isEmpty() );

  const auto pred = [&name](const BinaryDependenciesResultLibrary & library){
    return QString::compare(library.libraryName(), name, Qt::CaseInsensitive) == 0;
  };

  const auto it = std::find_if(mEntries.cbegin(), mEntries.cend(), pred);

  return it != mEntries.cend();
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
