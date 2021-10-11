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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_H

#include <QtGlobal>
#include <cstdint>
#include <string>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  enum class SectionType
  {
    Null = 0,               /*!< Inactive section header, does not have a associated section */
    ProgramData = 0x01,     /*!< Program data - SHT_PROGBITS */
    SymbolTable = 0x02,     /*!< Symbol table */
    StringTable = 0x03,     /*!< Refers to a string table section */
    Rela = 0x04,            /*!< Relocation entries with addends */
    Dynamic = 0x06,         /*!< Dynamic linking information */
    Note = 0x07,            /*!< Notes */
    NoBits = 0x08,          /*!< Program space with no data (bss) */
    DynSym = 0x0B,          /*!< Dynamic linker symbol table */
    InitArray = 0x0E,       /*!< array of constructors */
    FiniArray = 0x0F,       /*!< array of destructors */
    OsSpecific = 0x60000000 /*!< Value >= 0x60000000 (not directly from the standards) */
  };

  /*! \internal Section Attribute Flags
   *
   * \sa SectionAttributeFlags
   * \sa https://refspecs.linuxbase.org/elf/gabi4+/ch4.sheader.html#sh_flags
   */
  enum class SectionAttributeFlag : uint64_t
  {
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
    uint64_t flags;
    uint64_t addr;
    uint64_t offset;
    uint64_t size;
    uint32_t link;
    uint32_t info;
    uint64_t addralign;
    uint64_t entsize;

    void clear() noexcept
    {
      name.clear();
      type = 0;
    }

    SectionType sectionType() const noexcept
    {
      if(type >= 0x60000000){
        return SectionType::OsSpecific;
      }
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
        case 0x0B:
          return SectionType::DynSym;
        case 0x0E:
          return SectionType::InitArray;
        case 0x0F:
          return SectionType::FiniArray;
      }

      return SectionType::Null;
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

    /*! \brief Get the file offset of the end of the section represented by this header
     *
     * \note the returned offset is 1 byte past the last offset of the section
     */
    constexpr
    uint64_t fileOffsetEnd() const noexcept
    {
      return offset + size;
    }

    /*! \brief Get the minimum size to read the section this header references
     */
    constexpr
    qint64 minimumSizeToReadSection() const noexcept
    {
      return static_cast<qint64>(offset) + static_cast<qint64>(size);
    }
  };


}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_H
