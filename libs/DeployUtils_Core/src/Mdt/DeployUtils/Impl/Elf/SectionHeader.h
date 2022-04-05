/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2022 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_H

// #include <QtGlobal>
#include <cstdint>
#include <string>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * For some OS specific section type, see also:
   * - https://refspecs.linuxfoundation.org/LSB_3.0.0/LSB-PDA/LSB-PDA.junk/sections.html
   */
  enum class SectionType
  {
    Null = 0,                 /*!< Inactive section header, does not have a associated section */
    ProgramData = 1,          /*!< Program data - SHT_PROGBITS */
    SymbolTable = 2,          /*!< Symbol table */
    StringTable = 3,          /*!< Refers to a string table section */
    Rela = 4,                 /*!< Relocation entries with addends */
    Dynamic = 6,              /*!< Dynamic linking information */
    Note = 7,                 /*!< Notes */
    NoBits = 8,               /*!< Program space with no data (bss) */
    Rel = 9,                  /*!< Relocation entries without addends*/
    DynSym = 0x0B,            /*!< Dynamic linker symbol table */
    InitArray = 0x0E,         /*!< array of constructors */
    FiniArray = 0x0F,         /*!< array of destructors */
    OsSpecific = 0x60000000,  /*!< Value >= 0x60000000 */
    GnuHash = 0x6ffffff6,         /*!< GNU_HASH: GNU hash table. Did not find standards doc, but got the value from a executable */
    GnuVersionDef = 0x6ffffffd,   /*!< This section contains the symbol versions that are provided */
    GnuVersionNeed = 0x6ffffffe,  /*!< This section contains the symbol versions that are required */
    GnuVersionSym = 0x6fffffff    /*!< This section contains the Symbol Version Table */
  };

  /*! \internal Section Attribute Flags
   *
   * \sa SectionAttributeFlags
   * \sa https://refspecs.linuxbase.org/elf/gabi4+/ch4.sheader.html#sh_flags
   */
  enum class SectionAttributeFlag : uint64_t
  {
    Write = 0x01, /*!< The section holds data that should be writable during process execution */
    Alloc = 0x02, /*!< The section occupies memory during process execution */
    Tls = 0x400   /*!< Section holds Thread-Local Storage */
  };

  /*! \internal Section Attribute Flags
   *
   * \sa SectionAttributeFlag
   * \note We cannot use QFlags, because it can't be initialzed from a uint64_t
   */
//   class SectionAttributeFlags
//   {
//    public:
//   };

  /*! \internal
   */
  struct SectionHeader
  {
    std::string name;
    uint32_t nameIndex;
    uint32_t type = 0;
    uint64_t flags = 0;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link = 0;
    uint32_t info = 0;
    uint64_t addralign;
    uint64_t entsize;

    void clear() noexcept
    {
      name.clear();
      type = 0;
    }

    constexpr
    SectionType sectionType() const noexcept
    {
      switch(type){
        case 0x01:
          return SectionType::ProgramData;
        case 0x02:
          return SectionType::SymbolTable;
        case 0x03:
          return SectionType::StringTable;
        case 0x04:
          return SectionType::Rela;
        case 0x06:
          return SectionType::Dynamic;
        case 0x07:
          return SectionType::Note;
        case 0x08:
          return SectionType::NoBits;
        case 9:
          return SectionType::Rel;
        case 0x0B:
          return SectionType::DynSym;
        case 0x0E:
          return SectionType::InitArray;
        case 0x0F:
          return SectionType::FiniArray;
        case 0x6ffffff6:
          return SectionType::GnuHash;
        case 0x6ffffffd:
          return SectionType::GnuVersionDef;
        case 0x6ffffffe:
          return SectionType::GnuVersionNeed;
        case 0x6fffffff:
          return SectionType::GnuVersionSym;
      }
      if(type >= 0x60000000){
        return SectionType::OsSpecific;
      }

      return SectionType::Null;
    }

    /*! \brief Check if the section represented by this header is writable
     */
    constexpr
    bool isWritable() const noexcept
    {
      return flags & static_cast<uint64_t>(SectionAttributeFlag::Write);
    }

    /*! \brief Check if this section holds TLS (Thread-Local Storage)
     */
    constexpr
    bool holdsTls() const noexcept
    {
      return flags & static_cast<uint64_t>(SectionAttributeFlag::Tls);
    }

    /*! \brief Check if this section allocates memory during process execution
     */
    constexpr
    bool allocatesMemory() const noexcept
    {
      return flags & static_cast<uint64_t>(SectionAttributeFlag::Alloc);
    }

    /*! \brief Check if the link field (sh_link) refers to the index to a header in the section header table
     *
     * From the TIS ELF specification v1.2:
     * - Book I: Figure 1-12 sh_link and sh_info Interpretation 1-14
     * - Book III: Figure 1-1 sh_link and sh_info Interpretation 1-2
     *
     * \sa https://refspecs.linuxfoundation.org/LSB_3.0.0/LSB-PDA/LSB-PDA.junk/symversion.html
     */
    constexpr
    bool linkIsIndexInSectionHeaderTable() const noexcept
    {
      return link != 0;
    }

    /*! \brief Check if the info field (sh_info) refers to the index to a header in the section header table
     *
     * From the TIS ELF specification v1.2:
     * - Book I: Figure 1-12 sh_link and sh_info Interpretation 1-14
     * - Book III: Figure 1-1 sh_link and sh_info Interpretation 1-2
     *
     * \sa https://refspecs.linuxfoundation.org/LSB_3.0.0/LSB-PDA/LSB-PDA.junk/symversion.html
     */
    constexpr
    bool infoIsIndexInSectionHeaderTable() const noexcept
    {
      if(info == 0){
        return false;
      }
      switch( sectionType() ){
        case SectionType::Rela:
          return true;
        case SectionType::Rel:
          return true;
        default:
          break;
      }

      return false;
    }

    /*! \brief Check if this section is the .interp section
     */
    bool isProgramInterpreterSectionHeader() const noexcept
    {
      if(sectionType() != SectionType::ProgramData){
        return false;
      }

      return name == ".interp";
    }

    /*! \brief Check if this section is the .gnu.hash section
     */
    bool isGnuHashTableSectionHeader() const noexcept
    {
      return sectionType() == SectionType::GnuHash;
    }

    /*! \brief Get the file offset of the end of the section represented by this header
     *
     * \note the returned offset is 1 byte past the last offset of the section
     */
    constexpr
    uint64_t fileOffsetEnd() const noexcept
    {
      return offset + size;
    }

    /*! \brief Get the virtual address of the end of the section represented by this header
     *
     * \note the returned address is 1 byte past the last address of the section
     */
    constexpr
    uint64_t virtualAddressEnd() const noexcept
    {
      return addr + size;
    }

    /*! \brief Get the minimum size to read the section this header references
     */
    constexpr
    int64_t minimumSizeToReadSection() const noexcept
    {
      return static_cast<int64_t>(offset) + static_cast<int64_t>(size);
    }

    /*! \brief Get the minimum size to write the section this header references
     */
    constexpr
    int64_t minimumSizeToWriteSection() const noexcept
    {
      return minimumSizeToReadSection();
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_H
