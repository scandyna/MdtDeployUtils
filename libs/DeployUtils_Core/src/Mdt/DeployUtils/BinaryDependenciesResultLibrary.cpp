// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "BinaryDependenciesResultLibrary.h"
#include "FileInfoUtils.h"
#include <QLatin1String>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

bool BinaryDependenciesResultLibrary::isFound() const noexcept
{
  if( mFile.path().isEmpty() ){
    return false;
  }
  if( mFile.path() == QLatin1String(".") ){
    return false;
  }

  return true;
}

bool BinaryDependenciesResultLibrary::shouldNotBeRedistributed() const noexcept
{
  return mNotRedistrbute;
}

QString BinaryDependenciesResultLibrary::absoluteFilePath() const noexcept
{
  assert( isFound() );

  return mFile.absoluteFilePath();
}

BinaryDependenciesResultLibrary BinaryDependenciesResultLibrary::fromQFileInfo(const QFileInfo & fileInfo) noexcept
{
  assert( fileInfoHasFileName(fileInfo) );

  return BinaryDependenciesResultLibrary(fileInfo);
}

BinaryDependenciesResultLibrary
BinaryDependenciesResultLibrary::libraryToNotRedistrbuteFromFileInfo(const QFileInfo & fileInfo) noexcept
{
  assert( fileInfoHasFileName(fileInfo) );

  BinaryDependenciesResultLibrary library(fileInfo);
  library.mNotRedistrbute = true;

  return library;
}

}} // namespace Mdt{ namespace DeployUtils{
