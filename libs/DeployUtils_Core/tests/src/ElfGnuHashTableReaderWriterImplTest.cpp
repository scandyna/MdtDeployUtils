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
#include "ElfFileIoTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/GnuHashTableReader.h"
#include "Mdt/DeployUtils/Impl/Elf/GnuHashTableWriter.h"

using namespace Mdt::DeployUtils::Impl::Elf;
using Mdt::DeployUtils::Impl::ByteArraySpan;

TEST_CASE("hashTableFromArray")
{
  GnuHashTable hashTable;
  FileHeader fileHeader;
  ByteArraySpan array;
  uint64_t sectionSize;

  SECTION("32-bit big-endian")
  {
    fileHeader = make32BitBigEndianFileHeader();

    uchar arrayData[52] = {
      // nbuckets
      0,0,0,3,  // 3
      // symoffset
      0x12,0x34,0x56,0x78,  // 0x12345678
      // bloomSize
      0,0,0,2,  // 2
      // bloomShift
      0x23,0x45,0x67,0x89,  // 0x23456789
      // bloom[0]
      0x34,0x56,0x78,0x90,  // 0x34567890
      // bloom[1]
      0x45,0x67,0x89,0x01,  // 0x45678901
      // buckets[0]
      0x56,0x78,0x90,0x12,  // 0x56789012
      // buckets[1]
      0x67,0x89,0x01,0x23,  // 0x67890123
      // buckets[2]
      0x78,0x90,0x12,0x34,  // 0x78901234
      // chain[0]
      0x89,0x01,0x23,0x45,  // 0x89012345
      // chain[1]
      0x90,0x12,0x34,0x56,  // 0x90123456
      // chain[2]
      0x01,0x23,0x45,0x67,  // 0x01234567
      // chain[3]
      0x12,0x34,0x56,0x78   // 0x12345678
    };
    sectionSize = sizeof(arrayData);
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    hashTable = GnuHashTableReader::hashTableFromArray(array, fileHeader.ident, sectionSize);

    REQUIRE( hashTable.bucketCount() == 3 );
    REQUIRE( hashTable.symoffset == 0x12345678 );
    REQUIRE( hashTable.bloomSize() == 2 );
    REQUIRE( hashTable.bloomShift == 0x23456789 );
    REQUIRE( hashTable.bloom[0] == 0x34567890 );
    REQUIRE( hashTable.bloom[1] == 0x45678901 );
    REQUIRE( hashTable.buckets[0] == 0x56789012 );
    REQUIRE( hashTable.buckets[1] == 0x67890123 );
    REQUIRE( hashTable.buckets[2] == 0x78901234 );
    REQUIRE( hashTable.chain.size() == 4 );
    REQUIRE( hashTable.chain[0] == 0x89012345 );
    REQUIRE( hashTable.chain[1] == 0x90123456 );
    REQUIRE( hashTable.chain[2] == 0x01234567 );
    REQUIRE( hashTable.chain[3] == 0x12345678 );
  }

  SECTION("64-bit little-endian")
  {
    fileHeader = make64BitLittleEndianFileHeader();

    uchar arrayData[60] = {
      // nbuckets
      3,0,0,0,  // 3
      // symoffset
      0x78,0x56,0x34,0x12,  // 0x12345678
      // bloomSize
      2,0,0,0,  // 2
      // bloomShift
      0x89,0x67,0x45,0x23,  // 0x23456789
      // bloom[0]
      0x78,0x56,0x34,0x12,0x90,0x78,0x56,0x34,  // 0x3456789012345678
      // bloom[1]
      0x89,0x67,0x45,0x23,0x01,0x89,0x67,0x45,  // 0x4567890123456789
      // buckets[0]
      0x12,0x90,0x78,0x56,  // 0x56789012
      // buckets[1]
      0x23,0x01,0x89,0x67,  // 0x67890123
      // buckets[2]
      0x34,0x12,0x90,0x78,  // 0x78901234
      // chain[0]
      0x45,0x23,0x01,0x89,  // 0x89012345
      // chain[1]
      0x56,0x34,0x12,0x90,  // 0x90123456
      // chain[2]
      0x67,0x45,0x23,0x01,  // 0x01234567
      // chain[3]
      0x78,0x56,0x34,0x12   // 0x12345678
    };
    sectionSize = sizeof(arrayData);
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    hashTable = GnuHashTableReader::hashTableFromArray(array, fileHeader.ident, sectionSize);

    REQUIRE( hashTable.bucketCount() == 3 );
    REQUIRE( hashTable.symoffset == 0x12345678 );
    REQUIRE( hashTable.bloomSize() == 2 );
    REQUIRE( hashTable.bloomShift == 0x23456789 );
    REQUIRE( hashTable.bloom[0] == 0x3456789012345678 );
    REQUIRE( hashTable.bloom[1] == 0x4567890123456789 );
    REQUIRE( hashTable.buckets[0] == 0x56789012 );
    REQUIRE( hashTable.buckets[1] == 0x67890123 );
    REQUIRE( hashTable.buckets[2] == 0x78901234 );
    REQUIRE( hashTable.chain.size() == 4 );
    REQUIRE( hashTable.chain[0] == 0x89012345 );
    REQUIRE( hashTable.chain[1] == 0x90123456 );
    REQUIRE( hashTable.chain[2] == 0x01234567 );
    REQUIRE( hashTable.chain[3] == 0x12345678 );
  }
}

TEST_CASE("setGnuHashTableToArray")
{
  GnuHashTable hashTable;
  FileHeader fileHeader;
  ByteArraySpan array;
  ByteArraySpan expectedArray;

  SECTION("32-bit big-endian")
  {
    fileHeader = make32BitBigEndianFileHeader();

    hashTable.symoffset = 0x12345678;
    hashTable.bloomShift = 0x23456789;
    hashTable.bloom.push_back(0x34567890);
    hashTable.bloom.push_back(0x45678901);
    hashTable.buckets.push_back(0x56789012);
    hashTable.buckets.push_back(0x67890123);
    hashTable.buckets.push_back(0x78901234);
    hashTable.chain.push_back(0x89012345);
    hashTable.chain.push_back(0x90123456);
    hashTable.chain.push_back(0x01234567);
    hashTable.chain.push_back(0x12345678);

    uchar expectedArrayData[52] = {
      // nbuckets
      0,0,0,3,  // 3
      // symoffset
      0x12,0x34,0x56,0x78,  // 0x12345678
      // bloomSize
      0,0,0,2,  // 2
      // bloomShift
      0x23,0x45,0x67,0x89,  // 0x23456789
      // bloom[0]
      0x34,0x56,0x78,0x90,  // 0x34567890
      // bloom[1]
      0x45,0x67,0x89,0x01,  // 0x45678901
      // buckets[0]
      0x56,0x78,0x90,0x12,  // 0x56789012
      // buckets[1]
      0x67,0x89,0x01,0x23,  // 0x67890123
      // buckets[2]
      0x78,0x90,0x12,0x34,  // 0x78901234
      // chain[0]
      0x89,0x01,0x23,0x45,  // 0x89012345
      // chain[1]
      0x90,0x12,0x34,0x56,  // 0x90123456
      // chain[2]
      0x01,0x23,0x45,0x67,  // 0x01234567
      // chain[3]
      0x12,0x34,0x56,0x78   // 0x12345678
    };
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    uchar arrayData[sizeof(expectedArrayData)];
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    GnuHashTableWriter::setGnuHashTableToArray(array, hashTable, fileHeader.ident);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }

  SECTION("64-bit little-endian")
  {
    fileHeader = make64BitLittleEndianFileHeader();

    hashTable.symoffset = 0x12345678;
    hashTable.bloomShift = 0x23456789;
    hashTable.bloom.push_back(0x3456789012345678);
    hashTable.bloom.push_back(0x4567890123456789);
    hashTable.buckets.push_back(0x56789012);
    hashTable.buckets.push_back(0x67890123);
    hashTable.buckets.push_back(0x78901234);
    hashTable.chain.push_back(0x89012345);
    hashTable.chain.push_back(0x90123456);
    hashTable.chain.push_back(0x01234567);
    hashTable.chain.push_back(0x12345678);

    uchar expectedArrayData[60] = {
      // nbuckets
      3,0,0,0,  // 3
      // symoffset
      0x78,0x56,0x34,0x12,  // 0x12345678
      // bloomSize
      2,0,0,0,  // 2
      // bloomShift
      0x89,0x67,0x45,0x23,  // 0x23456789
      // bloom[0]
      0x78,0x56,0x34,0x12,0x90,0x78,0x56,0x34,  // 0x3456789012345678
      // bloom[1]
      0x89,0x67,0x45,0x23,0x01,0x89,0x67,0x45,  // 0x4567890123456789
      // buckets[0]
      0x12,0x90,0x78,0x56,  // 0x56789012
      // buckets[1]
      0x23,0x01,0x89,0x67,  // 0x67890123
      // buckets[2]
      0x34,0x12,0x90,0x78,  // 0x78901234
      // chain[0]
      0x45,0x23,0x01,0x89,  // 0x89012345
      // chain[1]
      0x56,0x34,0x12,0x90,  // 0x90123456
      // chain[2]
      0x67,0x45,0x23,0x01,  // 0x01234567
      // chain[3]
      0x78,0x56,0x34,0x12   // 0x12345678
    };
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    uchar arrayData[sizeof(expectedArrayData)];
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    GnuHashTableWriter::setGnuHashTableToArray(array, hashTable, fileHeader.ident);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }
}
