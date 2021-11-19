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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_READER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_READER_H

#include "GlobalOffsetTable.h"
#include "GlobalOffsetTableReaderWriterCommon.h"
#include "SectionHeader.h"
#include "SectionHeaderTable.h"
#include "FileHeader.h"
#include "FileReader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cstdint>
#include <string>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  GlobalOffsetTableEntry globalOffsetTableEntryFromArray(const ByteArraySpan & array, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );
    assert( array.size == globalOffsetTableEntrySize(ident._class) );

    GlobalOffsetTableEntry entry;

    entry.data = getNWord(array.data, ident);

    return entry;
  }

  /*! \internal
   */
  inline
  GlobalOffsetTable extractGlobalOffsetTable(const ByteArraySpan & map, const FileHeader & fileHeader, const SectionHeader & sectionHeader) noexcept
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= sectionHeader.minimumSizeToReadSection() );
    assert( isGlobalOffsetTableSection(sectionHeader) );

    GlobalOffsetTable table;

    const int64_t entrySize = static_cast<int64_t>(sectionHeader.entsize);
    const int64_t offsetStart = static_cast<int64_t>(sectionHeader.offset);
    const int64_t offsetEnd = offsetStart + static_cast<int64_t>(sectionHeader.size);
    for(int64_t offset = offsetStart; offset < offsetEnd; offset += entrySize){
      const GlobalOffsetTableEntry entry = globalOffsetTableEntryFromArray(map.subSpan(offset, entrySize), fileHeader.ident);
      table.addEntryFromFile(entry);
    }

    return table;
  }

  /*! \internal
   *
   * If the section given by \a name does not exist, a empty table is returned
   */
  inline
  GlobalOffsetTable extractGlobalOffsetTable(const ByteArraySpan & map, const FileHeader & fileHeader,
                                             const std::vector<SectionHeader> & sectionHeaderTable,
                                             const std::string & sectionName) noexcept
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );

    const auto headerIt = findFirstSectionHeader(sectionHeaderTable, SectionType::ProgramData, sectionName);
    if( headerIt == sectionHeaderTable.cend() ){
      return GlobalOffsetTable();
    }
    assert( map.size >= headerIt->minimumSizeToReadSection() );

    return extractGlobalOffsetTable(map, fileHeader, *headerIt);
  }

  /*! \internal
   *
   * If the .got section does not exist, a empty table is returned
   */
  inline
  GlobalOffsetTable extractGotSection(const ByteArraySpan & map, const FileHeader & fileHeader,
                                      const std::vector<SectionHeader> & sectionHeaderTable) noexcept
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );

    return extractGlobalOffsetTable(map, fileHeader, sectionHeaderTable, ".got");
//     const auto headerIt = findFirstSectionHeader(sectionHeaderTable, SectionType::ProgramData, ".got");
//     if( headerIt == sectionHeaderTable.cend() ){
//       return GlobalOffsetTable();
//     }
//     assert( map.size >= headerIt->minimumSizeToReadSection() );
// 
//     return extractGlobalOffsetTable(map, fileHeader, *headerIt);
  }

  /*! \internal
   *
   * If the .got section does not exist, a empty table is returned
   */
  inline
  GlobalOffsetTable extractGotPltSection(const ByteArraySpan & map, const FileHeader & fileHeader,
                                         const std::vector<SectionHeader> & sectionHeaderTable) noexcept
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );

    return extractGlobalOffsetTable(map, fileHeader, sectionHeaderTable, ".got.plt");
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_READER_H
