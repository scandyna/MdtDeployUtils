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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_ALGORITHM_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_ALGORITHM_H

#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal Find the address of the next page
   *
   * Example with a page size of 8:
   * \code
   * //     | | | | | | | | | | | | | | | | | | |
   * // Addr 0             7 8             15
   * \endcode
   *
   * Take some cases for \a start address:
   * - 0: 0 % 8 = 0 -> we are at the beginning of the page
   * - 3: 3 % 8 = 3 -> next page is at 3 + 8 - 3 = 8
   * - 9: 9 % 8 = 1 -> next page is at 9 + 8 - 1 = 16
   */
  inline
  uint64_t findAddressOfNextPage(uint64_t start, uint64_t pageSize) noexcept
  {
    assert(pageSize > 0);

    const uint64_t offsetInPage = start % pageSize;
    if(offsetInPage == 0){
      return start;
    }

    return start + pageSize - offsetInPage;
  }

  /*! \internal Find the next address that is aligned to \a alignment
   *
   * Examples for \a start with \a alignment of 4 bytes (32bit):
   * - 0: 0 % 4 = 0 : is aligned
   * - 1: 1 % 4 = 1 : not aligned -> next address is at 1 + 4 - 1 = 4
   * - 3: 3 % 4 = 3 : not aligned -> next address is at 3 + 4 - 3 = 4
   * - 4: 4 % 4 = 0 : is aligned
   * - 5: 5 % 4 = 1 : not aligned -> next address is at 5 + 4 - 1 = 8
   */
  inline
  uint64_t findNextAlignedAddress(uint64_t start, uint64_t alignment) noexcept
  {
    assert(alignment > 0);

    const uint64_t offset = start % alignment;
    if(offset == 0){
      return start;
    }

    return start + alignment - offset;
  }

  /*! \internal Check if \a virtualAddress and \a fileOffset is congruent regarding \a pageSize
   *
   * From the TIS ELF specification v1.2:
   * - Book I, Section 2-2 "Program Header"
   * - Book III, Section 2-2 "Program Header"
   */
  inline
  bool virtualAddressAndFileOffsetIsCongruent(uint64_t virtualAddress, uint64_t fileOffset, uint64_t pageSize) noexcept
  {
    assert(pageSize > 0);

    return (virtualAddress % pageSize) == (fileOffset % pageSize);
  }

  /*! \internal Find the next file offset starting from \a start
   *
   * Returns the next file offset, starting from \a start ,
   * that is congruent to \a virtualAddress modulo \a pageSize .
   *
   * \pre \a virtualAddress must be > 0 and a power of 2
   * \pre \a pageSize must be > 0 and a power of 2
   * \sa https://fr.wikipedia.org/wiki/Congruence_sur_les_entiers
   */
  inline
  uint64_t findNextFileOffset(uint64_t start, uint64_t virtualAddress, uint64_t pageSize) noexcept
  {
    assert(virtualAddress > 0);
    assert( (virtualAddress % 2) == 0 );
    assert(pageSize > 0);
    assert( (pageSize % 2) == 0 );

    const uint64_t remainder = virtualAddress % pageSize;

    /*
    * I'm not good at math,
    * so use a naive approach
    */
    while( (start % pageSize) != remainder ){
      ++start;
    }
    assert( virtualAddressAndFileOffsetIsCongruent(virtualAddress, start, pageSize) );

    return start;
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_ALGORITHM_H
