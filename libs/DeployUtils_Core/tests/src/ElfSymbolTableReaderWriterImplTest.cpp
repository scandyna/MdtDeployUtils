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
#include "Mdt/DeployUtils/Impl/Elf/SymbolTableReader.h"
#include "Mdt/DeployUtils/Impl/Elf/SymbolTableWriter.h"

using namespace Mdt::DeployUtils::Impl::Elf;
using Mdt::DeployUtils::Impl::ByteArraySpan;

TEST_CASE("symbolTableEntryFromArray")
{
  SymbolTableEntry entry;
  FileHeader fileHeader;
  ByteArraySpan array;

  SECTION("32-bit big-endian")
  {
    uchar arrayData[16] = {
      // name
      0x12,0x34,0x56,0x78,  // 0x12345678
      // value
      0x23,0x45,0x67,0x89,  // 0x23456789
      // size
      0x34,0x56,0x78,0x90,  // 0x34567890
      // info
      0x45,
      // other
      0x56,
      // shndx
      0x78,0x90             // 0x7890
    };
    fileHeader = make32BitBigEndianFileHeader();
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    entry = symbolTableEntryFromArray(array, fileHeader.ident);

    REQUIRE( entry.name == 0x12345678 );
    REQUIRE( entry.value == 0x23456789 );
    REQUIRE( entry.size == 0x34567890 );
    REQUIRE( entry.info == 0x45 );
    REQUIRE( entry.other == 0x56 );
    REQUIRE( entry.shndx == 0x7890 );
  }

  SECTION("64-bit little-endian")
  {
    uchar arrayData[24] = {
      // name
      0x78,0x56,0x34,0x12,  // 0x12345678
      // info
      0x23,
      // other
      0x34,
      // shndx
      0x78,0x56,            // 0x5678
      // value
      0x56,0x34,0x12,0x90,0x78,0x56,0x34,0x12,  // 0x1234567890123456
      // size
      0x67,0x45,0x23,0x01,0x89,0x67,0x45,0x23   // 0x2345678901234567
    };
    fileHeader = make64BitLittleEndianFileHeader();
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    entry = symbolTableEntryFromArray(array, fileHeader.ident);
    REQUIRE( entry.name == 0x12345678 );
    REQUIRE( entry.info == 0x23 );
    REQUIRE( entry.other == 0x34 );
    REQUIRE( entry.shndx == 0x5678 );
    REQUIRE( entry.value == 0x1234567890123456 );
    REQUIRE( entry.size == 0x2345678901234567 );
  }
}

TEST_CASE("extractPartialSymbolTableEntry")
{
  PartialSymbolTableEntry entry;
  FileHeader fileHeader;
  ByteArraySpan map;

  SECTION("map is just the required size")
  {
    uchar mapData[24] = {
      // name
      0x12,0,0,0,
      // info
      0x23,
      // other
      0x34,
      // shndx
      0x45,0,
      // value
      0x56,0,0,0,0,0,0,0,
      // size
      0x67,0,0,0,0,0,0,0
    };
    fileHeader = make64BitLittleEndianFileHeader();
    map = arraySpanFromArray( mapData, sizeof(mapData) );

    entry = extractPartialSymbolTableEntry(map, 0, fileHeader.ident);
    REQUIRE( entry.fileOffset == 0 );
    REQUIRE( entry.entry.name == 0x12 );
    REQUIRE( entry.entry.info == 0x23 );
    REQUIRE( entry.entry.other == 0x34 );
    REQUIRE( entry.entry.shndx == 0x45 );
    REQUIRE( entry.entry.value == 0x56 );
    REQUIRE( entry.entry.size == 0x67 );
  }

  SECTION("entry is in the somwhere in the map")
  {
    uchar mapData[26] = {
      0x98,
      // name
      0x12,0,0,0,
      // info
      0x23,
      // other
      0x34,
      // shndx
      0x45,0,
      // value
      0x56,0,0,0,0,0,0,0,
      // size
      0x67,0,0,0,0,0,0,0,
      0x89
    };
    fileHeader = make64BitLittleEndianFileHeader();
    map = arraySpanFromArray( mapData, sizeof(mapData) );

    entry = extractPartialSymbolTableEntry(map, 1, fileHeader.ident);
    REQUIRE( entry.fileOffset == 1 );
    REQUIRE( entry.entry.name == 0x12 );
    REQUIRE( entry.entry.info == 0x23 );
    REQUIRE( entry.entry.other == 0x34 );
    REQUIRE( entry.entry.shndx == 0x45 );
    REQUIRE( entry.entry.value == 0x56 );
    REQUIRE( entry.entry.size == 0x67 );
  }
}

TEST_CASE("setSymbolTableEntryToArray")
{
  SymbolTableEntry entry;
  FileHeader fileHeader;
  ByteArraySpan array;
  ByteArraySpan expectedArray;

  SECTION("32-bit big-endian")
  {
    entry.name = 0x12345678;
    entry.value = 0x23456789;
    entry.size = 0x34567890;
    entry.info = 0x45;
    entry.other = 0x56;
    entry.shndx = 0x7890;

    uchar expectedArrayData[16] = {
      // name
      0x12,0x34,0x56,0x78,  // 0x12345678
      // value
      0x23,0x45,0x67,0x89,  // 0x23456789
      // size
      0x34,0x56,0x78,0x90,  // 0x34567890
      // info
      0x45,
      // other
      0x56,
      // shndx
      0x78,0x90             // 0x7890
    };
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    fileHeader = make32BitBigEndianFileHeader();
    uchar arrayData[16];
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    setSymbolTableEntryToArray(array, entry, fileHeader.ident);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }

  SECTION("64-bit little-endian")
  {
    entry.name = 0x12345678;
    entry.info = 0x23;
    entry.other = 0x34;
    entry.shndx = 0x5678;
    entry.value = 0x1234567890123456;
    entry.size = 0x2345678901234567;

    uchar expectedArrayData[24] = {
      // name
      0x78,0x56,0x34,0x12,  // 0x12345678
      // info
      0x23,
      // other
      0x34,
      // shndx
      0x78,0x56,            // 0x5678
      // value
      0x56,0x34,0x12,0x90,0x78,0x56,0x34,0x12,  // 0x1234567890123456
      // size
      0x67,0x45,0x23,0x01,0x89,0x67,0x45,0x23   // 0x2345678901234567
    };
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    fileHeader = make64BitLittleEndianFileHeader();
    uchar arrayData[24];
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    setSymbolTableEntryToArray(array, entry, fileHeader.ident);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }
}
