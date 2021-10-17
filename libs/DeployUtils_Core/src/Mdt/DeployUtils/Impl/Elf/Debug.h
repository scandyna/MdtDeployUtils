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
#include "ProgramHeader.h"
#include "ProgramHeaderTable.h"
#include "FileAllHeaders.h"
#include "SectionHeader.h"
#include "DynamicSection.h"
#include "SymbolTable.h"
#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QLatin1String>
#include <vector>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  class StringTable;

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(Class c);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(DataFormat dataFormat);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(OsAbiType osAbiType);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const Ident & ident);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(ObjectFileType type);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(Machine machine);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const FileHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(SegmentType type);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const ProgramHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const ProgramHeaderTable & headers);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(SectionType type);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const SectionHeader & header);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const std::vector<SectionHeader> & headers);

  /** \internal Get the sections / segments mapping
   *
   * \code
   * 00 PT_PHDR
   * 01 PT_INTERP .interp
   * \endcode
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString sectionSegmentMappingToDebugString(const ProgramHeaderTable & programHeaderTable, const std::vector<SectionHeader> & sectionHeaderTable);

  /*! \internal
   */
  inline
  QString sectionSegmentMappingToDebugString(const FileAllHeaders & headers)
  {
    return sectionSegmentMappingToDebugString( headers.programHeaderTable(), headers.sectionHeaderTable() );
  }

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const StringTable & table);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(DynamicSectionTagType type);

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const DynamicStruct & section, const QString & leftPad = QLatin1String("  "));

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const DynamicSection & section, const QString & leftPad = QLatin1String("  "));

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const SymbolTableEntry & entry, const QString & leftPad = QLatin1String("  "));

  /*! \internal
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString toDebugString(const PartialSymbolTable & table, const QString & leftPad = QLatin1String("  "));

  /** \internal Print the file layout regarding all headers (File, Programm, Section)
   *
   * \code
   * from 0x0000 to 0x1111 : file header
   * from 0x1112 to 0x1133 : program header table
   * from 0x1134 to 0x2000 : XY segment type
   * from 0x2001 to 0x3000 : XY segment type
   * from 0x3001 to 0x4000 : .xy section
   * from 0x4001 to 0x5000 : .xy section
   * from 0x5001 to 0x6000 : section header table
   * \endcode
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QString fileLayoutToDebugString(const FileHeader & fileHeader, ProgramHeaderTable programHeaders, std::vector<SectionHeader> sectionHeaders);

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DEBUG_H
