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
#include "catch2/catch.hpp"
#include "Catch2QString.h"
#include "Mdt/DeployUtils/Impl/Elf/Algorithm.h"

TEST_CASE("findAddressOfNextPage")
{
  using Mdt::DeployUtils::Impl::Elf::findAddressOfNextPage;

  SECTION("page size: 8")
  {
    const uint64_t pageSize = 8;

    SECTION("start: 0 (we are at the beginning of the page)")
    {
      REQUIRE( findAddressOfNextPage(0, pageSize) == 0 );
    }

    SECTION("start: 3 (next page is at address 8")
    {
      REQUIRE( findAddressOfNextPage(3, pageSize) == 8 );
    }

    SECTION("start: 7 (next page is at address 8")
    {
      REQUIRE( findAddressOfNextPage(7, pageSize) == 8 );
    }

    SECTION("start: 8 (we are at the beginning of the page)")
    {
      REQUIRE( findAddressOfNextPage(8, pageSize) == 8 );
    }

    SECTION("start: 9 (next page is at address 16)")
    {
      REQUIRE( findAddressOfNextPage(9, pageSize) == 16 );
    }

    SECTION("start: 15 (next page is at address 16)")
    {
      REQUIRE( findAddressOfNextPage(15, pageSize) == 16 );
    }
  }
}

TEST_CASE("findNextAlignedAddress")
{
  using Mdt::DeployUtils::Impl::Elf::findNextAlignedAddress;

  SECTION("alignment: 4 bytes")
  {
    const uint64_t alignment = 4;

    SECTION("start: 0 (is aligned)")
    {
      REQUIRE( findNextAlignedAddress(0, alignment) == 0 );
    }

    SECTION("start: 1 (next aligned address is 4)")
    {
      REQUIRE( findNextAlignedAddress(1, alignment) == 4 );
    }

    SECTION("start: 3 (next aligned address is 4)")
    {
      REQUIRE( findNextAlignedAddress(3, alignment) == 4 );
    }

    SECTION("start: 4 (is aligned)")
    {
      REQUIRE( findNextAlignedAddress(4, alignment) == 4 );
    }

    SECTION("start: 5 (next aligned address is 8)")
    {
      REQUIRE( findNextAlignedAddress(5, alignment) == 8 );
    }

    SECTION("start: 7 (next aligned address is 8)")
    {
      REQUIRE( findNextAlignedAddress(7, alignment) == 8 );
    }
  }
}

TEST_CASE("virtualAddressAndFileOffsetIsCongruent")
{
  using Mdt::DeployUtils::Impl::Elf::virtualAddressAndFileOffsetIsCongruent;

  SECTION("page size: 4096 (0x1000)")
  {
    const uint64_t pageSize = 4096;

    /*
     * Example from the TIS ELF specification v1.2,
     * Book III, Section A-7 "Program Loading"
     */
    SECTION("vaddr: 0x8048100 , offset: 0x100")
    {
      REQUIRE( virtualAddressAndFileOffsetIsCongruent(0x8048100, 0x100, pageSize) );
    }

    /*
     * Some examples taken from compiled executables
     */

    SECTION("vaddr: 0 , offset: 0")
    {
      REQUIRE( virtualAddressAndFileOffsetIsCongruent(0, 0, pageSize) );
    }

    SECTION("vaddr: 0x201cc8 , offset: 0x1cc8")
    {
      REQUIRE( virtualAddressAndFileOffsetIsCongruent(0x201cc8, 0x1cc8, pageSize) );
    }
  }
}

TEST_CASE("findNextFileOffset")
{
  using Mdt::DeployUtils::Impl::Elf::findNextFileOffset;

  SECTION("page size: 8")
  {
    const uint64_t pageSize = 8;

    SECTION("start: 0 , vaddr: 6 -> offset: 6")
    {
      REQUIRE( findNextFileOffset(0, 6, pageSize) == 6 );
    }

    SECTION("start: 0 , vaddr: 8 -> offset: 0")
    {
      REQUIRE( findNextFileOffset(0, 8, pageSize) == 0 );
    }
  }

  SECTION("page size: 4096 (0x1000)")
  {
    const uint64_t pageSize = 4096;

    /*
     * Example from the TIS ELF specification v1.2,
     * Book III, Section A-7 "Program Loading"
     */
    SECTION("start: 0xFF, vaddr: 0x8048100 -> offset: 0x100")
    {
      REQUIRE( findNextFileOffset(0xFF, 0x8048100, pageSize) == 0x100 );
    }

    /*
     * Some examples taken from compiled executables
     */

    SECTION("start: 0x1818 , vaddr: 0x201cc8 -> offset: 0x1cc8")
    {
      REQUIRE( findNextFileOffset(0x1818, 0x201cc8, pageSize) == 0x1cc8 );
    }
  }
}

/// \todo see: https://wiki.osdev.org/ELF

/*! \internal Find the next virtual address starting from \a start
 *
 * From the TIS ELF specification v1.2:
 * - Book I, 1-10 "Section Header"
 * - Book I, 2-2  "Program Header"
 *
 * \sa https://en.wikipedia.org/wiki/Virtual_memory
 * \sa https://fr.wikipedia.org/wiki/M%C3%A9moire_virtuelle
 * \sa https://fr.wikipedia.org/wiki/Alignement_en_m%C3%A9moire
 *
 * Page size: 8 , alignment: 4
 * \code
 * //     | | | | | | | | | | | | | | | | | | |
 * // Addr 0             7 8             15
 * \endcode
 * Each address at the beginning of a page is aligned.
 *
 * Page size: 4 , alignment: 8
 * \code
 * //     | | | | | | | | | | | | | | | | | | |
 * // Addr 0     3 4     7 8    11 12     15 16
 * \endcode
 * Each address that is aligned is at the beginning of a page
 *
 * \todo look at output of a executable: aligned could be > page size ?
 *
 * \note after looking some ELF executables,
 *  it seems that the virtual address has to be power of 2,
 *  not necessarly aligned to the page size or to the given alignment (p_align).
 *  The constraint is the relation between file offset and virtual address.
 * \todo review Book I 2-2 "Program Header" p_align
 * \todo if we move .dynamic and/or .dynstr to the end,
 *  we will have to be in a new loadable segment,
 *  which should not overlapp any other (with other permissions).
 *  So, maybe put it in a vaddr next page..
 *  See: https://stackoverflow.com/questions/42599558/elf-program-header-virtual-address-and-file-offset
 *
 * \sa virtualAddressAndFileOffsetIsCongruent()
 */
uint64_t findNextVirtualAddress(uint64_t start, uint64_t pageSize, uint64_t alignment) noexcept
{
  assert(pageSize > 0);
  assert(alignment > 0);
}

TEST_CASE("findNextVirtualAddress")
{
  REQUIRE(false);
}
