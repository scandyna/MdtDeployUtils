/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#include "QtModuleList.h"
#include <algorithm>

namespace Mdt{ namespace DeployUtils{

void QtModuleList::addModule(QtModule module) noexcept
{
  if( !contains(module) ){
    mList.push_back(module);
  }
}

bool QtModuleList::contains(QtModule module) const noexcept
{
  return std::find(cbegin(), cend(), module) != cend();
}

QtModuleList QtModuleList::fromQtSharedLibraries(const QtSharedLibraryFileList & qtLibraries)
{
  QtModuleList qtModules;

  for(const QtSharedLibraryFile & qtLibrary : qtLibraries){
    qtModules.addModule( qtModuleFromLibraryName( qtLibrary.fileName() ) );
  }

  return qtModules;
}

}} // namespace Mdt{ namespace DeployUtils{
