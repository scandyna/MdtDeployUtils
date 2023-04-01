/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2023 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_RPATH_H
#define MDT_DEPLOY_UTILS_RPATH_H

#include <Mdt/ExecutableFile/RPath.h>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a single path in a RPath
   */
  using RPathEntry = Mdt::ExecutableFile::RPathEntry;

  /*! \brief Represents a rpath (run-time search path)
   *
   * A rapth is a list of paths encoded in a executable file or a shared library.
   *
   * \sa RPathElf
   * \sa RPathMachO
   * \sa https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
   * \sa https://man7.org/linux/man-pages/man8/ld.so.8.html
   * \sa https://developer.apple.com/library/archive/documentation/DeveloperTools/Conceptual/DynamicLibraries/100-Articles/RunpathDependentLibraries.html
   */
  using RPath = Mdt::ExecutableFile::RPath;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_RPATH_H
