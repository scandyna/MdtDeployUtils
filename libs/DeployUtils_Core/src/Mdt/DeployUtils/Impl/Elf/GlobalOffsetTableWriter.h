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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_WRITER_H

#include "GlobalOffsetTable.h"
#include "GlobalOffsetTableReaderWriterCommon.h"
#include "Ident.h"
#include "FileHeader.h"
#include "SectionHeader.h"
#include "FileWriterUtils.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  void setGlobalOffsetTableEntryToArray(ByteArraySpan array, const GlobalOffsetTableEntry & entry, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );
    assert( array.size == globalOffsetTableEntrySize(ident._class) );

    setNWord(array, entry.data, ident);
  }

  /*! \internal
   */
  inline
  void setGlobalOffsetTableToMap(ByteArraySpan map, const SectionHeader & sectionHeader,
                                 const GlobalOffsetTable & table, const FileHeader & fileHeader) noexcept
  {
    assert( !map.isNull() );
    assert( !table.isEmpty() );
    assert( fileHeader.seemsValid() );
    assert( isGlobalOffsetTableSection(sectionHeader) );
    assert( map.size >= sectionHeader.minimumSizeToWriteSection() );

    const int64_t entrySize = globalOffsetTableEntrySize(fileHeader.ident._class);
    int64_t offset = static_cast<int64_t>(sectionHeader.offset);
    assert(offset >= 0);

    for(size_t i=0; i < table.entriesCount(); ++i){
      setGlobalOffsetTableEntryToArray( map.subSpan(offset, entrySize), table.entryAt(i), fileHeader.ident );
      offset += entrySize;
    }
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_WRITER_H
