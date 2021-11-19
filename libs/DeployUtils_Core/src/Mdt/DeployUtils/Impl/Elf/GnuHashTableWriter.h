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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GNU_HASH_TABLE_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_GNU_HASH_TABLE_WRITER_H

#include "GnuHashTable.h"
#include "FileWriterUtils.h"
#include "SectionHeader.h"
#include "Exceptions.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class MDT_DEPLOYUTILSCORE_EXPORT GnuHashTableWriter : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief
     */
    static
    void setGnuHashTableToArray(ByteArraySpan array, const GnuHashTable & table, const Ident & ident) noexcept
    {
      assert( !array.isNull() );
//       assert( !table.isNull() );
      assert( ident.isValid() );
      assert( array.size == table.byteCount(ident._class) );

      const uint32_t nbuckets = static_cast<uint32_t>( table.buckets.size() );
      const uint32_t bloomSize = static_cast<uint32_t>( table.bloom.size() );

      set32BitWord(array.subSpan(0, 4), nbuckets, ident.dataFormat);
      set32BitWord(array.subSpan(4, 4), table.symoffset, ident.dataFormat);
      set32BitWord(array.subSpan(8, 4), bloomSize, ident.dataFormat);
      set32BitWord(array.subSpan(12, 4), table.bloomShift, ident.dataFormat);

      const int64_t bloomStartOffset = 16;
      const int64_t bloomEntrySize = GnuHashTable::bloomEntryByteCount(ident._class);

      int64_t offset = bloomStartOffset;
      for(uint64_t bloomEntry : table.bloom){
        setNWord(array.subSpan(offset, bloomEntrySize), bloomEntry, ident);
        offset += bloomEntrySize;
      }

      for(uint32_t bucketEntry : table.buckets){
        set32BitWord(array.subSpan(offset, 4), bucketEntry, ident.dataFormat);
        offset += 4;
      }

      for(uint32_t chainEntry : table.chain){
        set32BitWord(array.subSpan(offset, 4), chainEntry, ident.dataFormat);
        offset += 4;
      }
    }

    /*! \brief
     */
    static
    void setGnuHashTableToMap(ByteArraySpan map, const SectionHeader & sectionHeader,
                              const GnuHashTable & table, const FileHeader & fileHeader) noexcept
    {
      assert( !map.isNull() );
//       assert( !table.isNull() );
      assert( fileHeader.seemsValid() );
      assert( sectionHeader.isGnuHashTableSectionHeader() );
      assert( map.size >= sectionHeader.minimumSizeToWriteSection() );

      const int64_t offset = static_cast<int64_t>(sectionHeader.offset);
      const int64_t size = static_cast<int64_t>(sectionHeader.size);
      setGnuHashTableToArray(map.subSpan(offset, size), table, fileHeader.ident);
    }

  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GNU_HASH_TABLE_WRITER_H
