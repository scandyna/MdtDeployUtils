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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_H

#include <cstdint>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \sa https://refspecs.linuxbase.org/elf/gabi4+/ch5.dynamic.html
   */
  enum class DynamicSectionTagType
  {
    Null = 0,             /*!< Marks the end of the _DYNAMIC array */
    Needed = 1,           /*!< This element holds the string table offset to get the needed library name */
    StringTable = 5,      /*!< This element holds the address to the string table */
    StringTableSize = 10, /*!< This element holds the size, in bytes, of the string table */
    SoName = 14,          /*!< This element holds the string table offset to get the sahred object name */
    RPath = 15,           /*!< This element holds the string table offset to get the search path (deprecated) */
    Runpath = 29,         /*!< This element holds the string table offset to get the search path */
    Unknown = 100         /*!< Unknown element (not from the standard) */
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
    int64_t tag = 0;
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
        case 10:
          return DynamicSectionTagType::StringTableSize;
        case 14:
          return DynamicSectionTagType::SoName;
        case 15:
          return DynamicSectionTagType::RPath;
        case 29:
          return DynamicSectionTagType::Runpath;
      }

      return DynamicSectionTagType::Unknown;
    }

    constexpr
    bool isNull() const noexcept
    {
      return tag == 0;
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_H
