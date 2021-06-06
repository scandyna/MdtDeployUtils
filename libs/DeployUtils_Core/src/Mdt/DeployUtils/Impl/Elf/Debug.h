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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DEBUG_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_DEBUG_H

#include "FileHeader.h"
#include "SectionHeader.h"
#include "DynamicSection.h"
#include "mdt_deployutils_export.h"
#include <QString>
#include <QLatin1String>
#include <vector>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(Class c);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(DataFormat dataFormat);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(OsAbiType osAbiType);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const Ident & ident);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(ObjectFileType type);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(Machine machine);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const FileHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(SectionType type);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const SectionHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const std::vector<SectionHeader> & headers);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(DynamicSectionTagType type);

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const DynamicStruct & section, const QString & leftPad = QLatin1String("  "));

  /*! \internal
   */
  MDT_DEPLOYUTILS_EXPORT
  QString toDebugString(const std::vector<DynamicStruct> & sections, const QString & leftPad = QLatin1String("  "));

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DEBUG_H
