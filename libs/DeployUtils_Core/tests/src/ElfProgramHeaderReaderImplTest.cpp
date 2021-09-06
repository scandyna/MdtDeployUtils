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
#include "ElfProgramHeaderReaderImplTestCommon.h"

TEST_CASE("programHeaderFromArray")
{
  using Impl::Elf::programHeaderFromArray;
  using Impl::Elf::ProgramHeader;
  using Impl::Elf::Ident;

  ProgramHeader programHeader;
  Ident ident;
  ByteArraySpan array;

  SECTION("32-bit big-endian")
  {
    const uchar programHeaderArray[32] = {
      // p_type
      0x12,0x34,0x56,0x78, // 0x12345678
      // p_offset
      0x90,0x12,0x34,0x56, // 0x90123456
      // p_vaddr
      0x34,0x56,0x78,0x90, // 0x34567890
      // p_paddr
      0x23,0x45,0x67,0x89, // 0x23456789
      // p_filesz
      0x34,0x56,0x78,0x90, // 0x34567890
      // p_memsz
      0x45,0x67,0x89,0x01, // 0x45678901
      // p_flags
      0x56,0x78,0x90,0x12, // 0x56789012
      // p_align
      0x67,0x89,0x01,0x23  // 0x67890123
    };

    ident = make32BitBigEndianIdent();
    array = arraySpanFromArray( programHeaderArray, sizeof(programHeaderArray) );
    programHeader = programHeaderFromArray(array, ident);
    REQUIRE( programHeader.type == 0x12345678 );
    REQUIRE( programHeader.flags == 0x56789012 );
    REQUIRE( programHeader.offset == 0x90123456 );
    REQUIRE( programHeader.vaddr == 0x34567890 );
    REQUIRE( programHeader.paddr == 0x23456789 );
    REQUIRE( programHeader.filesz == 0x34567890 );
    REQUIRE( programHeader.memsz == 0x45678901 );
    REQUIRE( programHeader.align == 0x67890123 );
  }

  SECTION("64-bit little-endian")
  {
    const uchar programHeaderArray[56] = {
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

    ident = make64BitLittleEndianIdent();
    array = arraySpanFromArray( programHeaderArray, sizeof(programHeaderArray) );
    programHeader = programHeaderFromArray(array, ident);
    REQUIRE( programHeader.type == 0x12345678 );
    REQUIRE( programHeader.flags == 0x23456789 );
    REQUIRE( programHeader.offset == 0x34567890 );
    REQUIRE( programHeader.vaddr == 0x45678901 );
    REQUIRE( programHeader.paddr == 0x56789012 );
    REQUIRE( programHeader.filesz == 0x67890123 );
    REQUIRE( programHeader.memsz == 0x78901234 );
    REQUIRE( programHeader.align == 0x89012345 );
  }
}
