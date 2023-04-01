/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2011-2023 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_PLATFORM_H
#define MDT_DEPLOY_UTILS_PLATFORM_H

#include "Mdt/DeployUtils/OperatingSystem.h"
#include "Mdt/DeployUtils/Compiler.h"
#include "Mdt/DeployUtils/ProcessorISA.h"
#include "Mdt/DeployUtils/ExecutableFileFormat.h"
#include <Mdt/ExecutableFile/Platform.h>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Definition of a platform
   *
   * In term of a executable or a shared library,
   * there are several parts of a platorm that can be interresting:
   * - The operating system on which the file is executed
   * - The executable file format (ELF, PE)
   * - The compiler that created the file
   * - The processor ISA on which the file is executed
   *
   * Note that the operating system and the executable file format
   * can seem redoundant. But, for example, ELF supports Linux, NetBSD, HP-UX and many others.
   */
  using Platform = Mdt::ExecutableFile::Platform;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_PLATFORM_H
