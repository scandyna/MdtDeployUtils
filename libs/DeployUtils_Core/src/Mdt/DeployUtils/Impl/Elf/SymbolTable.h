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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_H

#include "Ident.h"
#include "SectionHeader.h"
#include "SectionHeaderTable.h"
#include "SectionIndexChangeMap.h"
#include <cstdint>
#include <limits>
#include <vector>
#include <algorithm>
#include <cassert>

// #include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  enum class SymbolType : unsigned char
  {
    NoType = 0,   /*!< The symbol's type is not specified. */
    Object = 1,   /*!< Symbol is associated with data object, like variable, array, ... */
    Function = 2, /*!< The symbol is associated with a function or other executable code. */
    Section = 3,  /*!< The symbol is associated with a section */
    File = 4,     /*!< Section index in SHN_ABS ? */
    LowProc = 13, /*!< Low bound of CPU specific semantics. */
    HighProc = 15 /*!< High bound of CPU specific semantics. */
  };

  /*! \internal
   *
   * From the TIS ELF specification v1.2:
   * - Book I, Symbol Table 1-18
   *
   * For a 32-bit file, this struct is like:
   * \code
   * struct
   * {
   *   Elf32_Word name;
   *   Elf32_Addr value;
   *   Elf32_Word size;
   *   unsigned char info;
   *   unsigned char other;
   *   Elf32_Half shndx;
   * };
   * \endcode
   *
   * The version for a 64-bit file is like:
   * \code
   * struct
   * {
   *   uint32_t name;
   *   unsigned char info;
   *   unsigned char other;
   *   uint16_t shndx;
   *   Elf64_Addr value; // => uint64_t
   *   uint64_t size;
   * };
   * \endcode
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  struct SymbolTableEntry
  {
    uint32_t name;
    unsigned char info;
    unsigned char other;
    uint16_t shndx;
    uint64_t value;
    uint64_t size;

    /*! \brief Get the symbol type
     */
    SymbolType symbolType() const noexcept
    {
      const unsigned char type = info & 0x0F;

      switch(type){
        case 0:
          return SymbolType::NoType;
        case 1:
          return SymbolType::Object;
        case 2:
          return SymbolType::Function;
        case 3:
          return SymbolType::Section;
        case 4:
          return SymbolType::File;
        case 13:
          return SymbolType::LowProc;
        case 15:
          return SymbolType::HighProc;
      }

      return SymbolType::NoType;
    }

    /*! \brief Check if this entry is realted to a section
     *
     * From the TIS ELF specification v1.2:
     * - Book I, Symbol Table 1-18
     * - Book I, Figure 1-7. Special Section Indexes 1-9
     */
    bool isRelatedToASection() const noexcept
    {
      if(shndx == 0){
        return false;
      }
      return shndx < 0xff00;  // SHN_LORESERVE
    }
  };

  /*! \internal Get the size, in bytes, of a symbol table entry
   */
  inline
  int64_t symbolTableEntrySize(Class c) noexcept
  {
    assert(c != Class::ClassNone);

    if(c == Class::Class32){
      /*
       * 2x Elf32_Word + Elf32_Addr + 2x uchar + Elf32_Half
       * 2x uint32_t   + uint32_t   + 2x uchar + uint16_t
       * 2x 4          + 4          + 2x 1     + 2  = 16
       */
      return 16;
    }
    assert(c == Class::Class64);

    /*
     * uint32_t + 2x uchar + uint16_t + 2x uint64_t
     * 4        + 2x 1     + 2        + 2x 8  = 24
     */
    return 24;
  }

  /*! \internal Helper class for PartialSymbolTable 
   */
  struct PartialSymbolTableEntry
  {
    int64_t fileOffset;
    SymbolTableEntry entry;

    /*! \brief Get the minimum size to access this entry
     */
    int64_t minimumSizeToAccessEntry(Class c) const noexcept
    {
      assert(c != Class::ClassNone);

      return fileOffset + symbolTableEntrySize(c);
    }
  };

  /*! \internal
   */
  class PartialSymbolTable
  {
   public:

    /*! \brief Add a entry from file
     */
    void addEntryFromFile(const PartialSymbolTableEntry & entry) noexcept
    {
      mTable.push_back(entry);
    }

    /*! \brief Updates the symbols referring to a index in the section header table regarding \a indexChanges
     */
    void updateSectionIndexes(const SectionIndexChangeMap & indexChanges) noexcept
    {
      for(PartialSymbolTableEntry & entry : mTable){
        if( entry.entry.isRelatedToASection() ){
          assert( entry.entry.shndx < indexChanges.indexCount() );
          entry.entry.shndx = indexChanges.indexForOldIndex(entry.entry.shndx);
        }
      }
    }

    /*! \brief Update the virtual addresses in this symbol table regarding given section headers indexes referring \a sectionHeaderTable
     */
    void updateVirtualAddresses(const std::vector<uint16_t> sectionHeadersIndexes, const std::vector<SectionHeader> & sectionHeaderTable) noexcept
    {
      for(PartialSymbolTableEntry & entry : mTable){
        if( entry.entry.isRelatedToASection() ){
          const auto sectionHeaderIndexIt = std::find(sectionHeadersIndexes.cbegin(), sectionHeadersIndexes.cend(), entry.entry.shndx);
          if( sectionHeaderIndexIt != sectionHeadersIndexes.cend() ){
            assert( *sectionHeaderIndexIt < sectionHeaderTable.size() );
            entry.entry.value = sectionHeaderTable[*sectionHeaderIndexIt].addr;
          }
        }
      }
    }

    /*! \brief Index the associations to known sections
     *
     * \todo rename: indexKnownSymbols()
     */
    void indexAssociationsKnownSections(const std::vector<SectionHeader> & sectionHeaderTable) noexcept
    {
      constexpr uint16_t uint16Max = std::numeric_limits<uint16_t>::max();

      uint16_t dynamicSectionIndex = uint16Max;
      uint16_t dynamicStringTableIndex = uint16Max;

      const uint16_t sectionHeaderTableSize = static_cast<uint16_t>( sectionHeaderTable.size() );
      for(uint16_t sectionIndex = 0; sectionIndex < sectionHeaderTableSize; ++sectionIndex){
        const SectionHeader & header = sectionHeaderTable[sectionIndex];
        if(header.sectionType() == SectionType::Dynamic){
          dynamicSectionIndex = sectionIndex;
          dynamicStringTableIndex = static_cast<uint16_t>(header.link);
          break;
        }
      }

      for(size_t i=0; i < mTable.size(); ++i){
        if(dynamicSectionIndex < uint16Max){
          const SymbolTableEntry & entry = mTable[i].entry;
          if(entry.shndx == dynamicSectionIndex){
            assert( i < mTable.size() );
            switch( entry.symbolType() ){
              case SymbolType::Section:
                mDynamicSectionIndex = i;
                break;
              case SymbolType::Object:
                mDynamicObjectIndex = i;
                break;
              default:
                break;
            }
          }
        }
        if( (dynamicStringTableIndex < uint16Max)&&(mTable[i].entry.shndx == dynamicStringTableIndex) ){
          assert( i < mTable.size() );
          mDynamicStringTableIndex = i;
        }
      }
    }

    /*! \brief Check if this table is empty
     */
    bool isEmpty() const noexcept
    {
      return mTable.empty();
    }

    /*! \brief Get the cout of entries if this table
     */
    size_t entriesCount() const noexcept
    {
      return mTable.size();
    }

    /*! \brief Get the entry at \a index
     *
     * \pre \a index must be in valid range
     */
    const SymbolTableEntry & entryAt(size_t index) const noexcept
    {
      assert( index < entriesCount() );

      return mTable[index].entry;
    }

    /*! \brief Get the file map offset for the entry at \a index
     *
     * \pre \a index must be in valid range
     */
    int64_t fileMapOffsetAt(size_t index) const noexcept
    {
      assert( index < entriesCount() );

      return mTable[index].fileOffset;
    }

    /*! \brief Check if this table contains the association to the dynamic section
     */
    bool containsDynamicSectionAssociation() const noexcept
    {
      return mDynamicSectionIndex < std::numeric_limits<size_t>::max();
    }

    /*! \brief Check if this table contains the dynamic object (_DYNAMIC)
     */
    bool containsDynamicObject() const noexcept
    {
      return mDynamicObjectIndex < std::numeric_limits<size_t>::max();
    }

    /*! \brief Set the virtual address to the dynamic section
     */
    void setDynamicSectionVirtualAddress(uint64_t address) noexcept
    {
      if( containsDynamicSectionAssociation() ){
        mTable[mDynamicSectionIndex].entry.value = address;
      }
      if( containsDynamicObject() ){
        mTable[mDynamicObjectIndex].entry.value = address;
      }
    }

    /*! \brief Check if this table contains the association to the dynamic string table (.dynstr)
     */
    bool containsDynamicStringTableAssociation() const noexcept
    {
      return mDynamicStringTableIndex < std::numeric_limits<size_t>::max();
    }

    /*! \brief Set the virtual address to the dynamic string table
     */
    void setDynamicStringTableVirtualAddress(uint64_t address) noexcept
    {
      assert( containsDynamicStringTableAssociation() );

      mTable[mDynamicStringTableIndex].entry.value = address;
    }

    /*! \brief Find the minimum size to access all entries in this table
     */
    int64_t findMinimumSizeToAccessEntries(Class c) const noexcept
    {
      assert( !isEmpty() );
      assert(c != Class::ClassNone);

      const auto cmp = [c](const PartialSymbolTableEntry & a, const PartialSymbolTableEntry & b){
        return a.minimumSizeToAccessEntry(c) < b.minimumSizeToAccessEntry(c);
      };

      const auto it = std::max_element(mTable.cbegin(), mTable.cend(), cmp);

      return it->minimumSizeToAccessEntry(c);
    }

   private:

    size_t mDynamicSectionIndex = std::numeric_limits<size_t>::max();
    size_t mDynamicObjectIndex = std::numeric_limits<size_t>::max();
    size_t mDynamicStringTableIndex = std::numeric_limits<size_t>::max();
    std::vector<PartialSymbolTableEntry> mTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SYMBOL_TABLE_H
