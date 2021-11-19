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
#include "Mdt/DeployUtils/Impl/Elf/GnuHashTable.h"

using namespace Mdt::DeployUtils::Impl::Elf;

TEST_CASE("byteCount")
{
  /*
   * struct GnuHashTable
   * {
   *   uint32_t nbuckets;
   *   uint32_t symoffset;
   *   uint32_t bloomSize;
   *   uint32_t bloomShift;
   *   uint64_t bloom[bloom_size]; // uint32_t for 32-bit binaries
   *   uint32_t buckets[nbuckets];
   *   uint32_t chain[];
   * };
   */

  GnuHashTable hashTable;

  SECTION("32-bit")
  {
    Class elfClass = Class::Class32;

    SECTION("default constructed")
    {
      /*
       * 4 x 4 = 16 bytes
       */
      REQUIRE( hashTable.byteCount(elfClass) == 16 );
    }

    SECTION("bloom size: 2 , buckets: 3, chain: 4")
    {
      /*
       * 4x4 + 2x4 + 3x4 + 4x4 = 52
       */
      hashTable.bloom.push_back(10);
      hashTable.bloom.push_back(11);
      hashTable.buckets.push_back(12);
      hashTable.buckets.push_back(13);
      hashTable.buckets.push_back(14);
      hashTable.chain.push_back(15);
      hashTable.chain.push_back(16);
      hashTable.chain.push_back(17);
      hashTable.chain.push_back(18);

      REQUIRE( hashTable.byteCount(elfClass) == 52 );
    }
  }

  SECTION("64-bit")
  {
    Class elfClass = Class::Class64;

    SECTION("default constructed")
    {
      REQUIRE( hashTable.byteCount(elfClass) == 16 );
    }

    SECTION("bloom size: 2 , buckets: 3, chain: 4")
    {
      /*
       * 4x4 + 2x8 + 3x4 + 4x4 = 60
       */
      hashTable.bloom.push_back(10);
      hashTable.bloom.push_back(11);
      hashTable.buckets.push_back(12);
      hashTable.buckets.push_back(13);
      hashTable.buckets.push_back(14);
      hashTable.chain.push_back(15);
      hashTable.chain.push_back(16);
      hashTable.chain.push_back(17);
      hashTable.chain.push_back(18);

      REQUIRE( hashTable.byteCount(elfClass) == 60 );
    }
  }
}
