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
#include "QtPlugins.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{

bool QtPlugins::pathIsAbsoluteAndCouldBePluginsRoot(const QFileInfo & qtPluginsRoot) noexcept
{
  if( qtPluginsRoot.filePath().isEmpty() ){
    return false;
  }
  if( !qtPluginsRoot.isAbsolute() ){
    return false;
  }
  if( !qtPluginsRoot.isDir() ){
    return false;
  }
  if( qtPluginsRoot.fileName() != QLatin1String("plugins") ){
    return false;
  }

  return true;
}

}} // namespace Mdt{ namespace DeployUtils{
