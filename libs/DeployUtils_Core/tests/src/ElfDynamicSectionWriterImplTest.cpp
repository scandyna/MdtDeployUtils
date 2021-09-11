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
#include "ElfDynamicSectionImplTestCommon.h"
#include "ElfFileIoTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/DynamicSection.h"
#include "Mdt/DeployUtils/Impl/Elf/DynamicSectionWriter.h"
#include <QLatin1String>

using Mdt::DeployUtils::Impl::ByteArraySpan;
using Mdt::DeployUtils::Impl::Elf::FileHeader;
using Mdt::DeployUtils::Impl::Elf::Ident;
using Mdt::DeployUtils::Impl::Elf::DynamicSection;

TEST_CASE("dynamicSectionByteCount")
{
  using Mdt::DeployUtils::Impl::Elf::dynamicSectionByteCount;

  Ident ident;
  DynamicSection dynamicSection;

  SECTION("1 entry")
  {
    dynamicSection.addEntry( makeNeededEntry(12) );

    SECTION("32-bit")
    {
      ident = make32BitBigEndianIdent();
      REQUIRE( dynamicSectionByteCount(dynamicSection, ident) == 8 );
    }

    SECTION("64-bit")
    {
      ident = make64BitLittleEndianIdent();
      REQUIRE( dynamicSectionByteCount(dynamicSection, ident) == 16 );
    }
  }

  SECTION("2 entries")
  {
    dynamicSection.addEntry( makeNullEntry() );
    dynamicSection.addEntry( makeNeededEntry(12) );

    SECTION("32-bit")
    {
      ident = make32BitBigEndianIdent();
      REQUIRE( dynamicSectionByteCount(dynamicSection, ident) == 16 );
    }

    SECTION("64-bit")
    {
      ident = make64BitLittleEndianIdent();
      REQUIRE( dynamicSectionByteCount(dynamicSection, ident) == 32 );
    }
  }
}

TEST_CASE("dynamicSectionToArray")
{
  using Mdt::DeployUtils::Impl::Elf::dynamicSectionToArray;

  Ident ident;
  ByteArraySpan array;
  ByteArraySpan expectedArray;

  DynamicSection dynamicSection;

  SECTION("1 entry")
  {
    dynamicSection.addEntry( makeRunPathEntry(0x12345678) );

    SECTION("32-bit big-endian")
    {
      uchar expectedArrayData[8] = {
        // d_tag
        0,0,0,0x1D, // 0x1D (29)
        // d_val
        0x12,0x34,0x56,0x78   // 0x12345678
      };
      expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

      uchar arrayData[8] = {};
      array = arraySpanFromArray( arrayData, sizeof(arrayData) );
      ident = make32BitBigEndianIdent();

      dynamicSectionToArray(array, dynamicSection, ident);
      REQUIRE( arraysAreEqual(array, expectedArray) );
    }

    SECTION("64-bit little-endian")
    {
      uchar expectedArrayData[16] = {
        // d_tag
        0x1D,0,0,0,0,0,0,0, // 0x1D (29)
        // d_val
        0x78,0x56,0x34,0x12,0,0,0,0 // 0x12345678
      };
      expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

      uchar arrayData[16] = {};
      array = arraySpanFromArray( arrayData, sizeof(arrayData) );
      ident = make64BitLittleEndianIdent();

      dynamicSectionToArray(array, dynamicSection, ident);
      REQUIRE( arraysAreEqual(array, expectedArray) );
    }
  }

  SECTION("2 entries")
  {
    dynamicSection.addEntry( makeNullEntry() );
    dynamicSection.addEntry( makeRunPathEntry(0x12345678) );

    SECTION("32-bit big-endian")
    {
      uchar expectedArrayData[16] = {
        // d_tag
        0,0,0,0,
        // d_val or d_ptr
        0,0,0,0,
        // d_tag
        0,0,0,0x1D, // 0x1D (29)
        // d_val
        0x12,0x34,0x56,0x78   // 0x12345678
      };
      expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

      uchar arrayData[16] = {};
      array = arraySpanFromArray( arrayData, sizeof(arrayData) );
      ident = make32BitBigEndianIdent();

      dynamicSectionToArray(array, dynamicSection, ident);
      REQUIRE( arraysAreEqual(array, expectedArray) );
    }

    SECTION("64-bit little-endian")
    {
      uchar expectedArrayData[32] = {
        // d_tag
        0,0,0,0,0,0,0,0,
        // d_val or d_ptr
        0,0,0,0,0,0,0,0,
        // d_tag
        0x1D,0,0,0,0,0,0,0, // 0x1D (29)
        // d_val
        0x78,0x56,0x34,0x12,0,0,0,0 // 0x12345678
      };
      expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

      uchar arrayData[32] = {};
      array = arraySpanFromArray( arrayData, sizeof(arrayData) );
      ident = make64BitLittleEndianIdent();

      dynamicSectionToArray(array, dynamicSection, ident);
      REQUIRE( arraysAreEqual(array, expectedArray) );
    }
  }
}
