// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "BinaryDependenciesResultLibrary.h"
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

BinaryDependenciesResultLibrary BinaryDependenciesResultLibrary::fromQFileInfo(const QFileInfo & fileInfo) noexcept
{
  assert( !fileInfo.fileName().trimmed().isEmpty() );

  return BinaryDependenciesResultLibrary(fileInfo);
}

}} // namespace Mdt{ namespace DeployUtils{
