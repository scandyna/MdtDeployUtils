// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef RPATH_UTILS_H
#define RPATH_UTILS_H

#include "Mdt/DeployUtils/RPath.h"
#include <string>
#include <vector>

Mdt::DeployUtils::RPath
makeRPathFromPathList(const std::vector<std::string> & pathList)
{
  using Mdt::DeployUtils::RPath;

  RPath rpath;

  for(const std::string & path : pathList){
    rpath.appendPath( QString::fromStdString(path) );
  }

  return rpath;
}

#endif // #ifndef RPATH_UTILS_H
