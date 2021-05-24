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

#include <cstdint>
#include <string>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  enum class SectionType
  {
    Null = 0,               /*!< Inactive section header, does not have a associated section */
    ProgramData = 0x01,     /*!< Program data */
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

  /*! \internal
   */
  struct SectionHeader
  {
    std::string name;
    uint32_t nameIndex;
    uint32_t type;
    uint64_t offset;
    uint64_t size;

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
  };

  /*! \internal
   *
   * \sa https://refspecs.linuxbase.org/elf/gabi4+/ch5.dynamic.html
   */
  enum class DynamicSectionTagType
  {
    Null = 0,         /*!< Marks the end of the _DYNAMIC array */
    Needed = 1,       /*!< This element holds the string table offset to get the needed library name */
    StringTable = 5,  /*!< This element holds the address to the string table */
    SoName = 14,      /*!< This element holds the string table offset to get the sahred object name */
    RPath = 15,       /*!< This element holds the string table offset to get the search path (deprecated) */
    Runpath = 29,     /*!< This element holds the string table offset to get the search path */
    Unknown = 100     /*!< Unknown element (not from the standard) */
  };

  /*! \internal
   *
   * A dynamic section contains a array of ElfN_Dyn, here called DynamicStruct
   *
   * For a 32-bit file, this struct is like:
   * \code
   * struct Elf32_Dyn
   * {
   *   Elf32_Sword d_tag;
   *   union
   *   {
   *     Elf32_Word d_val;
   *     Elf32_Addr d_ptr;
   *   }d_un;
   * };
   * \endcode
   *
   * For a 64-bit file, this struct is like:
   * \code
   * struct Elf64_Dyn
   * {
   *   Elf64_Sxword d_tag;
   *   union
   *   {
   *     Elf64_Xword d_val;
   *     Elf64_Addr d_ptr;
   *   }d_un;
   * };
   * \endcode
   *
   * The various types are:
   * \code
   * Elf32_Sword: int32_t
   * Elf32_Word: uint32_t
   * Elf32_Addr: uint32_t
   *
   * Elf64_Sxword: int64_t
   * Elf64_Xword: uint64_t
   * Elf64_Addr: uint64_t
   * \endcode
   *
   * If replacing the type aliases, we find the following structs.
   *
   * For a 32-bit file, this struct is like:
   * \code
   * struct Elf32_Dyn
   * {
   *   int32_t d_tag;
   *   union
   *   {
   *     uint32_t d_val;
   *     uint32_t d_ptr;
   *   }d_un;
   * };
   * \endcode
   *
   * For a 64-bit file, this struct is like:
   * \code
   * struct Elf64_Dyn
   * {
   *   int64_t d_tag;
   *   union
   *   {
   *     uint64_t d_val;
   *     uint64_t d_ptr;
   *   }d_un;
   * };
   * \endcode
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  struct DynamicStruct
  {
    int64_t tag;
    uint64_t val_or_ptr;

    DynamicSectionTagType tagType() const noexcept
    {
      switch(tag){
        case 0:
          return DynamicSectionTagType::Null;
        case 1:
          return DynamicSectionTagType::Needed;
        case 5:
          return DynamicSectionTagType::StringTable;
        case 14:
          return DynamicSectionTagType::SoName;
        case 15:
          return DynamicSectionTagType::RPath;
        case 29:
          return DynamicSectionTagType::Runpath;
      }

      return DynamicSectionTagType::Unknown;
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_H
