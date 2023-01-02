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

  if( !result.isSolved() ){
    mIsSolved = false;
  }
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
      assert( library.isFound() || library.shouldNotBeRedistributed() );
      const QString path = library.absoluteFilePath();
      if( !pathList.contains(path) ){
        pathList.append(path);
      }
    }
  }

  return pathList;
}

bool listContainsLibrary(const std::vector<BinaryDependenciesResultLibrary> & list,
                         const BinaryDependenciesResultLibrary & library,
                         OperatingSystem os) noexcept
{
  const QString name = library.libraryName();
  const auto pred = [&name, os](const BinaryDependenciesResultLibrary & library){
    return Impl::BinaryDependencies::fileNamesAreEqual(library.libraryName(), name, os);
  };

  const auto it = std::find_if(list.cbegin(), list.cend(), pred);

  return it != list.cend();
}

void addLibrariesToList(const std::vector<BinaryDependenciesResultLibrary> & libraries,
                        std::vector<BinaryDependenciesResultLibrary> & list,
                        OperatingSystem os) noexcept
{
  for(const BinaryDependenciesResultLibrary & library : libraries){
    if( !listContainsLibrary(list, library, os) ){
      list.push_back(library);
    }
  }
}


std::vector<BinaryDependenciesResultLibrary>
getLibrariesToRedistribute(const BinaryDependenciesResultList & resultList) noexcept
{
  std::vector<BinaryDependenciesResultLibrary> libraries;

  for(const BinaryDependenciesResult & result : resultList){
    assert( result.isSolved() );
    addLibrariesToList( getLibrariesToRedistribute(result), libraries, resultList.operatingSystem() );
  }

  return libraries;
}

}} // namespace Mdt{ namespace DeployUtils{
