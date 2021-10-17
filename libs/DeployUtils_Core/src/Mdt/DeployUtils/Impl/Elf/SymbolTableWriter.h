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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_WRITER_H

#include "SymbolTable.h"
#include "Ident.h"
#include "FileWriterUtils.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cassert>

// #include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  void setSymbolTableEntryToArray(ByteArraySpan array, const SymbolTableEntry & entry, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );
    assert( array.size == symbolTableEntrySize(ident._class) );

    set32BitWord(array.subSpan(0, 4), entry.name, ident.dataFormat);
    if(ident._class == Class::Class32){
      setNWord(array.subSpan(4, 4), entry.value, ident);
      setNWord(array.subSpan(8, 4), entry.size, ident);
      *(array.data + 12) = entry.info;
      *(array.data + 13) = entry.other;
      setHalfWord(array.subSpan(14, 2), entry.shndx, ident.dataFormat);
    }else{
      assert(ident._class == Class::Class64);
      *(array.data + 4) = entry.info;
      *(array.data + 5) = entry.other;
      setHalfWord(array.subSpan(6, 2), entry.shndx, ident.dataFormat);
      setNWord(array.subSpan(8, 8), entry.value, ident);
      setNWord(array.subSpan(16, 8), entry.size, ident);
    }
  }

  /*! \internal
   */
  inline
  void setSymbolTableToMap(ByteArraySpan map, const PartialSymbolTable & table, const Ident & ident) noexcept
  {
    assert( !map.isNull() );
    assert( ident.isValid() );
    assert( map.size >= table.findMinimumSizeToAccessEntries(ident._class) );

    for(size_t i=0; i < table.entriesCount(); ++i){
      const int64_t fileOffset = table.fileMapOffsetAt(i);
      const int64_t entrySize = symbolTableEntrySize(ident._class);
      setSymbolTableEntryToArray( map.subSpan(fileOffset, entrySize), table.entryAt(i), ident );
    }
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_WRITER_H
