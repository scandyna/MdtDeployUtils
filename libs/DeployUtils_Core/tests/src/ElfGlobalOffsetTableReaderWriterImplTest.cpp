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
#include "Mdt/DeployUtils/Impl/Elf/GlobalOffsetTableReader.h"
#include "Mdt/DeployUtils/Impl/Elf/GlobalOffsetTableWriter.h"

using namespace Mdt::DeployUtils::Impl::Elf;
using Mdt::DeployUtils::Impl::ByteArraySpan;


TEST_CASE("globalOffsetTableEntryFromArray")
{
  GlobalOffsetTableEntry entry;
  FileHeader fileHeader;
  ByteArraySpan array;

  SECTION("32-bit big-endian")
  {
    uchar arrayData[4] = {
      0x12,0x34,0x56,0x78 // 0x12345678
    };
    fileHeader = make32BitBigEndianFileHeader();
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    entry = globalOffsetTableEntryFromArray(array, fileHeader.ident);

    REQUIRE( entry.data == 0x12345678 );
  }

  SECTION("64-bit little-endian")
  {
    uchar arrayData[8] = {
      0x56,0x34,0x12,0x90,0x78,0x56,0x34,0x12 // 0x1234567890123456
    };
    fileHeader = make64BitLittleEndianFileHeader();
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    entry = globalOffsetTableEntryFromArray(array, fileHeader.ident);

    REQUIRE( entry.data == 0x1234567890123456 );
  }
}

TEST_CASE("globalOffsetTableEntryToArray")
{
  GlobalOffsetTableEntry entry;
  FileHeader fileHeader;
  ByteArraySpan array;
  ByteArraySpan expectedArray;

  SECTION("32-bit big-endian")
  {
    entry.data = 0x12345678;

    uchar expectedArrayData[4] = {
      0x12,0x34,0x56,0x78 // 0x12345678
    };
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    fileHeader = make32BitBigEndianFileHeader();
    uchar arrayData[4];
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    setGlobalOffsetTableEntryToArray(array, entry, fileHeader.ident);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }

  SECTION("64-bit little-endian")
  {
    entry.data = 0x1234567890123456;

    uchar expectedArrayData[8] = {
      0x56,0x34,0x12,0x90,0x78,0x56,0x34,0x12 // 0x1234567890123456
    };
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    fileHeader = make64BitLittleEndianFileHeader();
    uchar arrayData[8];
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    setGlobalOffsetTableEntryToArray(array, entry, fileHeader.ident);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }
}
