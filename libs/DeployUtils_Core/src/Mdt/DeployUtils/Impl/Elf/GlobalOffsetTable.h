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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_H

#include "Ident.h"
#include <cstdint>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * From the TIS ELF specification v1.2:
   * - A, Global Offset Table A-10
   *
   * \code
   * // entry type is Elf32_Addr -> uint32_t
   * \endcode
   *
   * I think it's a address of type:
   * \code
   * // ElfN_Addr
   * \endcode
   * So, probably a uint64_t on 64-bit machine.
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  struct GlobalOffsetTableEntry
  {
    uint64_t data;
  };

  /*! \internal Get the size, in bytes, of a symbol table entry
   */
  inline
  int64_t globalOffsetTableEntrySize(Class c) noexcept
  {
    assert(c != Class::ClassNone);

    if(c == Class::Class64){
      return 8;
    }
    assert( c == Class::Class32 );

    return 4;
  }

  /*! \internal
   *
   * From the TIS ELF specification v1.2:
   * - Book III, Global Offset Table 2-13
   * - A, Global Offset Table A-10
   */
  class GlobalOffsetTable
  {
   public:

    /*! \brief Add a entry from file
     */
    void addEntryFromFile(const GlobalOffsetTableEntry & entry) noexcept
    {
      mTable.push_back(entry);
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
    const GlobalOffsetTableEntry & entryAt(size_t index) const noexcept
    {
      assert( index < entriesCount() );

      return mTable[index];
    }

    /*! \brief Check if this table contains the dynamic section address
     */
    bool containsDynamicSectionAddress() const noexcept
    {
      if( isEmpty() ){
        return false;
      }

      return mTable[0].data != 0;
    }

    /*! \brief Get the address of the dynamic section
     *
     * \pre this table must not be empty
     */
    uint64_t dynamicSectionAddress() const noexcept
    {
      assert( !isEmpty() );

      return mTable[0].data;
    }

    /*! \brief Set the address of the dynamic section
     *
     * \pre this table must not be empty
     */
    void setDynamicSectionAddress(uint64_t address) noexcept
    {
      assert( !isEmpty() );

      mTable[0].data = address;
    }

   private:

    std::vector<GlobalOffsetTableEntry> mTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_H
