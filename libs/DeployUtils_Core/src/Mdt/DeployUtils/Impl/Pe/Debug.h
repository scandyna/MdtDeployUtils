/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_IMPL_PE_DEBUG_H
#define MDT_DEPLOY_UTILS_IMPL_PE_DEBUG_H

#include "FileHeader.h"
#include "SectionHeader.h"
#include "ImportDirectory.h"
#include "mdt_deployutils_export.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const DosHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(MachineType type);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const CoffHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(MagicType type);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const OptionalHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const SectionHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const ImportDirectory & directory, const QString & leftPad = QLatin1String("  "));

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const ImportDirectoryTable & directoryTable);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const DelayLoadDirectory & directory, const QString & leftPad = QLatin1String("  "));

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const DelayLoadTable & table);

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Pe{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_PE_DEBUG_H
