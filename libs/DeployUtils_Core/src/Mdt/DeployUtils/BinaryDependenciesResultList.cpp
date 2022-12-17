// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ****************************************************************************/
#include "BinaryDependenciesResultList.h"
#include "Impl/BinaryDependencies/FileComparison.h"
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

BinaryDependenciesResultList::BinaryDependenciesResultList(OperatingSystem os)
 : mOs(os)
{
  assert( operatingSystemIsValid(mOs) );
}

void BinaryDependenciesResultList::addResult(const BinaryDependenciesResult & result) noexcept
{
  assert( result.operatingSystem() == mOs );

  mList.push_back(result);
}

std::optional<BinaryDependenciesResult>
BinaryDependenciesResultList::findResultForTargetName(const QString & name) const noexcept
{
  assert( !name.trimmed().isEmpty() );

  const OperatingSystem os = mOs;
  const auto pred = [&name, os](const BinaryDependenciesResult & resut){
    return Impl::BinaryDependencies::fileNamesAreEqual(resut.target().fileName(), name, os);
  };

  const auto it = std::find_if(cbegin(), cend(), pred);

  if( it == cend() ){
    return {};
  }

  return *it;
}

QStringList getLibrariesAbsoluteFilePathList(const BinaryDependenciesResultList & resultList)
{
  QStringList pathList;

  for(const BinaryDependenciesResult & result : resultList){
    assert( result.isSolved() );
    for(const BinaryDependenciesResultLibrary & library : result){
      assert( library.isFound() );
      const QString path = library.absoluteFilePath();
      if( !pathList.contains(path) ){
        pathList.append(path);
      }
    }
  }

  return pathList;
}

}} // namespace Mdt{ namespace DeployUtils{
