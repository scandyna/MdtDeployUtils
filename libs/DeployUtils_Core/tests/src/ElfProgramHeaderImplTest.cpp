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
#include "ElfProgramHeaderTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/ProgramHeader.h"

using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::SegmentType;
using Mdt::DeployUtils::Impl::Elf::SegmentPermission;

TEST_CASE("segmentType")
{
  ProgramHeader header;

  SECTION("PT_NULL")
  {
    header.type = 0;
    REQUIRE( header.segmentType() == SegmentType::Null );
  }

  SECTION("PT_LOAD")
  {
    header.type = 1;
    REQUIRE( header.segmentType() == SegmentType::Load );
  }

  SECTION("PT_DYNAMIC")
  {
    header.type = 2;
    REQUIRE( header.segmentType() == SegmentType::Dynamic );
  }

  SECTION("PT_INTERP")
  {
    header.type = 3;
    REQUIRE( header.segmentType() == SegmentType::Interpreter );
  }

  SECTION("PT_NOTE")
  {
    header.type = 4;
    REQUIRE( header.segmentType() == SegmentType::Note );
  }

  SECTION("PT_PHDR")
  {
    header.type = 6;
    REQUIRE( header.segmentType() == SegmentType::ProgramHeaderTable );
  }

  SECTION("PT_TLS")
  {
    header.type = 7;
    REQUIRE( header.segmentType() == SegmentType::Tls );
  }

  SECTION("PT_GNU_EH_FRAME")
  {
    header.type = 0x6474e550;
    REQUIRE( header.segmentType() == SegmentType::GnuEhFrame );
  }

  SECTION("PT_GNU_STACK")
  {
    header.type = 0x6474e551;
    REQUIRE( header.segmentType() == SegmentType::GnuStack );
  }

  SECTION("PT_GNU_RELRO")
  {
    header.type = 0x6474e552;
    REQUIRE( header.segmentType() == SegmentType::GnuRelo );
  }
}

TEST_CASE("setSegmentType")
{
  ProgramHeader header;

  header.setSegmentType(SegmentType::Load);

  REQUIRE( header.segmentType() == SegmentType::Load );
}

TEST_CASE("SegmentPermission")
{
  ProgramHeader header;

  SECTION("Executable")
  {
    header.setPermissions(SegmentPermission::Execute);
    REQUIRE( header.isExecutable() );
    REQUIRE( !header.isWritable() );
    REQUIRE( !header.isReadable() );
  }

  SECTION("Readable and writable")
  {
    header.setPermissions(SegmentPermission::Read | SegmentPermission::Write);
    REQUIRE( !header.isExecutable() );
    REQUIRE( header.isWritable() );
    REQUIRE( header.isReadable() );
  }
}

TEST_CASE("requiresAlignment")
{
  ProgramHeader header;

  SECTION("align 0")
  {
    header.align = 0;
    REQUIRE( !header.requiresAlignment() );
  }

  SECTION("align 1")
  {
    header.align = 1;
    REQUIRE( !header.requiresAlignment() );
  }

  SECTION("align 4")
  {
    header.align = 4;
    REQUIRE( header.requiresAlignment() );
  }
}

TEST_CASE("shiftOffset")
{
  ProgramHeader header;

  SECTION("1 byte up")
  {
    header.offset = 124;
    header.shiftOffset(1);
    REQUIRE( header.offset == 125 );
  }

  SECTION("test incomplete - vaddr etc")
  {
    REQUIRE( false );
  }
}

TEST_CASE("segmentVirtualAddressEnd")
{
  ProgramHeader header;
  header.vaddr = 10;
  header.memsz = 5;

  REQUIRE( header.segmentVirtualAddressEnd() == 15 );
}

TEST_CASE("fileOffsetEnd")
{
  ProgramHeader header;
  header.offset = 10;
  header.filesz = 5;

  REQUIRE( header.fileOffsetEnd() == 15 );
}
