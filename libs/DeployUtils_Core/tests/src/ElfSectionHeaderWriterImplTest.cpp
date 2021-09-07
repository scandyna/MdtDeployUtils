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
#include "Mdt/DeployUtils/Impl/Elf/SectionHeaderWriter.h"
#include <QString>
#include <algorithm>
#include <cassert>
#include <iostream>

using Mdt::DeployUtils::Impl::ByteArraySpan;
using Mdt::DeployUtils::Impl::Elf::SectionHeader;
using Mdt::DeployUtils::Impl::Elf::Ident;


TEST_CASE("sectionHeaderToArray")
{
  using Mdt::DeployUtils::Impl::Elf::sectionHeaderToArray;

  SectionHeader sectionHeader;
  Ident ident;
  ByteArraySpan array;
  ByteArraySpan expectedArray;

  SECTION("32-bit big-endian")
  {
    sectionHeader.nameIndex = 0x12345678;
    sectionHeader.type = 0x23456789;
    sectionHeader.flags = 0x34567890;
    sectionHeader.addr = 0x45678901;
    sectionHeader.offset = 0x56789012;
    sectionHeader.size = 0x67890123;
    sectionHeader.link = 0x78901234;
    sectionHeader.info = 0x89012345;
    sectionHeader.addralign = 0x90123456;
    sectionHeader.entsize = 0x01234567;

    uchar expectedArrayData[40] = {
      // sh_name
      0x12,0x34,0x56,0x78, // 0x12345678
      // sh_type
      0x23,0x45,0x67,0x89, // 0x23456789
      // sh_flags
      0x34,0x56,0x78,0x90, // 0x34567890
      // sh_addr
      0x45,0x67,0x89,0x01, // 0x45678901
      // sh_offset
      0x56,0x78,0x90,0x12, // 0x56789012
      // sh_size
      0x67,0x89,0x01,0x23, // 0x67890123
      // sh_link
      0x78,0x90,0x12,0x34, // 0x78901234
      // sh_info
      0x89,0x01,0x23,0x45, // 0x89012345
      // sh_addralign
      0x90,0x12,0x34,0x56, // 0x90123456
      // sh_entsize
      0x01,0x23,0x45,0x67  // 0x01234567
    };
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    ident = make32BitBigEndianIdent();
    uchar arrayData[40] = {0};
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    sectionHeaderToArray(array, sectionHeader, ident);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }

  SECTION("64-bit little-endian")
  {
    sectionHeader.nameIndex = 0x12345678;
    sectionHeader.type = 0x23456789;
    sectionHeader.flags = 0x34567890;
    sectionHeader.addr = 0x45678901;
    sectionHeader.offset = 0x56789012;
    sectionHeader.size = 0x67890123;
    sectionHeader.link = 0x78901234;
    sectionHeader.info = 0x89012345;
    sectionHeader.addralign = 0x90123456;
    sectionHeader.entsize = 0x01234567;

    uchar expectedArrayData[64] = {
      // sh_name
      0x78,0x56,0x34,0x12,  // 0x12345678
      // sh_type
      0x89,0x67,0x45,0x23,  // 0x23456789
      // sh_flags
      0x90,0x78,0x56,0x34,0,0,0,0,  // 0x34567890
      // sh_addr
      0x01,0x89,0x67,0x45,0,0,0,0,  // 0x45678901
      // sh_offset
      0x12,0x90,0x78,0x56,0,0,0,0,  // 0x56789012
      // sh_size
      0x23,0x01,0x89,0x67,0,0,0,0,  // 0x67890123
      // sh_link
      0x34,0x12,0x90,0x78,  // 0x78901234
      // sh_info
      0x45,0x23,0x01,0x89,  // 0x89012345
      // sh_addralign
      0x56,0x34,0x12,0x90,0,0,0,0,  // 0x90123456
      // sh_entsize
      0x67,0x45,0x23,0x01,0,0,0,0   // 0x01234567
    };
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    ident = make64BitLittleEndianIdent();
    uchar arrayData[64] = {0};
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    sectionHeaderToArray(array, sectionHeader, ident);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }
}
