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
