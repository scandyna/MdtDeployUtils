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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_READER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_READER_H

#include "SymbolTable.h"
#include "Ident.h"
#include "FileHeader.h"
#include "SectionHeader.h"
#include "FileReader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  SymbolTableEntry symbolTableEntryFromArray(const ByteArraySpan & array, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );
    assert( array.size == symbolTableEntrySize(ident._class) );

    SymbolTableEntry entry;
    const unsigned char *it = array.data;

    entry.name = getWord(it, ident.dataFormat);
    it += 4;

    if(ident._class == Class::Class32){
      entry.value = getWord(it, ident.dataFormat);
      it += 4;
      entry.size = getWord(it, ident.dataFormat);
      it += 4;
      entry.info = *it;
      ++it;
      entry.other = *it;
      ++it;
      entry.shndx = getHalfWord(it, ident.dataFormat);
    }else{
      assert(ident._class == Class::Class64);
      entry.info = *it;
      ++it;
      entry.other = *it;
      ++it;
      entry.shndx = getHalfWord(it, ident.dataFormat);
      it += 2;
      entry.value = getAddress(it, ident);
      it += 8;
      entry.size = getNWord(it, ident);
    }

    return entry;
  }

  /*! \internal
   */
  inline
  int64_t minimumSizeToExtractSymbolTableEntry(int64_t fileOffset, Class c) noexcept
  {
    assert( fileOffset >= 0 );
    assert( c != Class::ClassNone );

    return fileOffset + symbolTableEntrySize(c);
  }

  /*! \internal
   */
  inline
  bool isSymbolTableSection(SectionType type) noexcept
  {
    switch(type){
      case SectionType::SymbolTable:
        return true;
      case SectionType::DynSym:
        return true;
      default:
        break;
    }

    return false;
  }

  /*! \internal
   */
  inline
  PartialSymbolTableEntry extractPartialSymbolTableEntry(const ByteArraySpan & map, int64_t fileOffset, const Ident & ident) noexcept
  {
    assert( !map.isNull() );
    assert( fileOffset >= 0 );
    assert( ident.isValid() );
    assert( map.size >= minimumSizeToExtractSymbolTableEntry(fileOffset, ident._class) );

    PartialSymbolTableEntry entry;

    entry.fileOffset = fileOffset;
    entry.entry = symbolTableEntryFromArray(map.subSpan( fileOffset, symbolTableEntrySize(ident._class) ), ident);

    return entry;
  }

  /*! \internal Extract a partial symbol table from \a sectionType for given \a symbolType
   */
  inline
  PartialSymbolTable extractPartialSymbolTable(const ByteArraySpan & map,
                                               const FileHeader & fileHeader,
                                               const std::vector<SectionHeader> & sectionHeaderTable,
                                               SectionType sectionType, SymbolType symbolType) noexcept
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );
    assert( isSymbolTableSection(sectionType) );

    PartialSymbolTable symbolTable;

    const auto symTabPred = [sectionType](const SectionHeader & header){
      return header.sectionType() == sectionType;
    };
    const auto symTabIt = std::find_if(sectionHeaderTable.cbegin(), sectionHeaderTable.cend(), symTabPred);
    if( symTabIt == sectionHeaderTable.cend() ){
      return symbolTable;
    }

    assert( map.size >= symTabIt->minimumSizeToReadSection() );

    const uint64_t entrySize = symTabIt->entsize;
    const uint64_t offsetEnd = symTabIt->offset + symTabIt->size;
    for(uint64_t offset = symTabIt->offset; offset < offsetEnd; offset += entrySize){
      const PartialSymbolTableEntry entry = extractPartialSymbolTableEntry(map, offset, fileHeader.ident);
      if(entry.entry.symbolType() == symbolType){
        symbolTable.addEntryFromFile(entry);
      }
    }

    symbolTable.indexAssociationsKnownSections(sectionHeaderTable);

    return symbolTable;
  }

  /*! \internal Extract the sections associations from the symbol table (.symtab)
   *
   * If the symbol table (.symtab) does not exist,
   * a empty symbol table is returned.
   */
  inline
  PartialSymbolTable extractSectionsAssociationsSymTab(const ByteArraySpan & map,
                                                       const FileHeader & fileHeader,
                                                       const std::vector<SectionHeader> & sectionHeaderTable) noexcept
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );

    return extractPartialSymbolTable(map, fileHeader, sectionHeaderTable, SectionType::SymbolTable, SymbolType::Section);
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_READER_H
