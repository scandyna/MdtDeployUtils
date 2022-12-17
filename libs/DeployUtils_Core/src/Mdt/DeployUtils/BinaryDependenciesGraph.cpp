// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "BinaryDependenciesGraph.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

void BinaryDependenciesGraph::addFile(const BinaryDependenciesFile & file) noexcept
{
  assert( !file.isNull() );
  assert( file.dependenciesFileNames().isEmpty() );
}

BinaryDependenciesFileList BinaryDependenciesGraph::findUnprocessedFiles() const noexcept
{
  BinaryDependenciesFileList files;

  return files;
}

}} // namespace Mdt{ namespace DeployUtils{
