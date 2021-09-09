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
#include "Mdt/DeployUtils/Impl/Elf/ProgramHeaderWriter.h"
#include <algorithm>
#include <cassert>

using Mdt::DeployUtils::Impl::ByteArraySpan;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::FileHeader;


TEST_CASE("programHeaderToArray")
{
  using Mdt::DeployUtils::Impl::Elf::programHeaderToArray;

  ProgramHeader programHeader;
  FileHeader fileHeader;
  ByteArraySpan array;
  ByteArraySpan expectedArray;

  SECTION("32-bit big-endian")
  {
    programHeader.type = 0x12345678;
    programHeader.offset = 0x23456789;
    programHeader.vaddr = 0x34567890;
    programHeader.paddr = 0x45678901;
    programHeader.filesz = 0x56789012;
    programHeader.memsz = 0x67890123;
    programHeader.flags = 0x78901234;
    programHeader.align = 0x89012345;

    uchar expectedProgramHeaderArray[32] = {
      // p_type
      0x12,0x34,0x56,0x78, // 0x12345678
      // p_offset
      0x23,0x45,0x67,0x89, // 0x23456789
      // p_vaddr
      0x34,0x56,0x78,0x90, // 0x34567890
      // p_paddr
      0x45,0x67,0x89,0x01, // 0x45678901
      // p_filesz
      0x56,0x78,0x90,0x12, // 0x56789012
      // p_memsz
      0x67,0x89,0x01,0x23, // 0x67890123
      // p_flags
      0x78,0x90,0x12,0x34, // 0x78901234
      // p_align
      0x89,0x01,0x23,0x45  // 0x89012345
    };
    expectedArray = arraySpanFromArray( expectedProgramHeaderArray, sizeof(expectedProgramHeaderArray) );

    fileHeader = make32BitBigEndianFileHeader();
    uchar arrayData[32] = {0};
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    programHeaderToArray(array, programHeader, fileHeader);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }

  SECTION("64-bit little-endian")
  {
    programHeader.type = 0x12345678;
    programHeader.flags = 0x23456789;
    programHeader.offset = 0x34567890;
    programHeader.vaddr = 0x45678901;
    programHeader.paddr = 0x56789012;
    programHeader.filesz = 0x67890123;
    programHeader.memsz = 0x78901234;
    programHeader.align = 0x89012345;

    uchar expectedProgramHeaderArray[56] = {
      // p_type
      0x78,0x56,0x34,0x12, // 0x12345678
      // p_flags
      0x89,0x67,0x45,0x23, // 0x23456789
      // p_offset
      0x90,0x78,0x56,0x34,0,0,0,0, // 0x34567890
      // p_vaddr
      0x01,0x89,0x67,0x45,0,0,0,0, // 0x45678901
      // p_paddr
      0x12,0x90,0x78,0x56,0,0,0,0, // 0x56789012
      // p_filesz
      0x23,0x01,0x89,0x67,0,0,0,0, // 0x67890123
      // p_memsz
      0x34,0x12,0x90,0x78,0,0,0,0, // 0x78901234
      // p_align
      0x45,0x23,0x01,0x89,0,0,0,0  // 0x89012345
    };
    expectedArray = arraySpanFromArray( expectedProgramHeaderArray, sizeof(expectedProgramHeaderArray) );

    fileHeader = make64BitLittleEndianFileHeader();
    uchar arrayData[56] = {0};
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    programHeaderToArray(array, programHeader, fileHeader);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }
}
