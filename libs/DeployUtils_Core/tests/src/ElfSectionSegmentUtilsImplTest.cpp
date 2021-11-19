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
#include "ElfSectionHeaderTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/SectionSegmentUtils.h"

using namespace Mdt::DeployUtils::Impl::Elf;

TEST_CASE("setProgramHeaderCoveringSections")
{
  ProgramHeader programHeader;
  std::vector<SectionHeader> sectionHeaders;

  SECTION("1 section")
  {
    SectionHeader headerA;
    headerA.offset = 10;
    headerA.addr = 100;
    headerA.size = 20;

    sectionHeaders.push_back(headerA);

    setProgramHeaderCoveringSections(programHeader, sectionHeaders);

    REQUIRE( programHeader.offset == 10 );
    REQUIRE( programHeader.vaddr == 100 );
    REQUIRE( programHeader.paddr == 100 );
    REQUIRE( programHeader.memsz == 20 );
    REQUIRE( programHeader.filesz == 20 );
  }

  SECTION("2 contigous sections")
  {
    SectionHeader headerA;
    headerA.offset = 10;
    headerA.addr = 100;
    headerA.size = 20;

    SectionHeader headerB;
    headerB.offset = 30;
    headerB.addr = 120;
    headerB.size = 20;

    sectionHeaders.push_back(headerA);
    sectionHeaders.push_back(headerB);

    setProgramHeaderCoveringSections(programHeader, sectionHeaders);

    REQUIRE( programHeader.offset == 10 );
    REQUIRE( programHeader.vaddr == 100 );
    REQUIRE( programHeader.paddr == 100 );
    REQUIRE( programHeader.memsz == 40 );
    REQUIRE( programHeader.filesz == 40 );
  }

  SECTION("2 section with a hole in between")
  {
    /*
     * offset  |10-29|30...|50-69|70
     * section |  A  |.....|  B  |
     * segment |                 |
     */
    SectionHeader headerA;
    headerA.offset = 10;
    headerA.addr = 100;
    headerA.size = 20;

    SectionHeader headerB;
    headerB.offset = 50;
    headerB.addr = 150;
    headerB.size = 20;

    sectionHeaders.push_back(headerA);
    sectionHeaders.push_back(headerB);

    setProgramHeaderCoveringSections(programHeader, sectionHeaders);

    REQUIRE( programHeader.offset == 10 );
    REQUIRE( programHeader.vaddr == 100 );
    REQUIRE( programHeader.paddr == 100 );
    REQUIRE( programHeader.memsz == 70 );
    REQUIRE( programHeader.filesz == 60 );
  }

  SECTION("segment permissions")
  {
    SectionHeader headerA;
    headerA.offset = 10;
    headerA.addr = 100;
    headerA.size = 20;

    SectionHeader headerB;
    headerB.offset = 30;
    headerB.addr = 120;
    headerB.size = 20;

    SECTION("no section requires write permission")
    {
      headerA.flags = 0x02;
      headerB.flags = 0x04;

      sectionHeaders.push_back(headerA);
      sectionHeaders.push_back(headerB);

      setProgramHeaderCoveringSections(programHeader, sectionHeaders);

      REQUIRE( programHeader.isReadable() );
      REQUIRE( !programHeader.isWritable() );
    }

    SECTION("1 section requires write permission")
    {
      headerA.flags = 0x02;
      headerB.flags = 0x03;

      sectionHeaders.push_back(headerA);
      sectionHeaders.push_back(headerB);

      setProgramHeaderCoveringSections(programHeader, sectionHeaders);

      REQUIRE( programHeader.isReadable() );
      REQUIRE( programHeader.isWritable() );
    }
  }
}

TEST_CASE("setProgramHeaderCoveringSections_byIndexes")
{
  ProgramHeader programHeader;
  std::vector<uint16_t> sectionHeaders;
  std::vector<SectionHeader> sectionHeaderTable;

  SectionHeader headerA;
  headerA.offset = 10;
  headerA.addr = 100;
  headerA.size = 20;

  SectionHeader headerB;
  headerB.offset = 30;
  headerB.addr = 120;
  headerB.size = 20;

  SectionHeader headerC;
  headerC.offset = 80;
  headerC.addr = 180;
  headerC.size = 10;

  /*
   * Section header table:
   * index:  |  1  |  2  |     |  3  |
   * offset  |10-29|30-49|50-79|80-89|90
   * section |  A  |  B  |.....|  C  |
   */
  sectionHeaderTable.push_back( makeNullSectionHeader() );
  sectionHeaderTable.push_back(headerA);
  sectionHeaderTable.push_back(headerB);
  sectionHeaderTable.push_back(headerC);

  SECTION("2 contigous sections")
  {
    sectionHeaders.push_back(1);
    sectionHeaders.push_back(2);

    setProgramHeaderCoveringSections(programHeader, sectionHeaders, sectionHeaderTable);

    REQUIRE( programHeader.offset == 10 );
    REQUIRE( programHeader.vaddr == 100 );
    REQUIRE( programHeader.paddr == 100 );
    REQUIRE( programHeader.memsz == 40 );
    REQUIRE( programHeader.filesz == 40 );
  }
}

TEST_CASE("extendProgramHeaderSizeToCoverSections")
{
  std::vector<SectionHeader> sectionHeaders;
  ProgramHeader programHeader;
  programHeader.offset = 10;
  programHeader.vaddr = 100;
  programHeader.paddr = 100;
  programHeader.memsz = 10;
  programHeader.filesz = 10;

  SECTION("1 section starting at segment but with greater size")
  {
    SectionHeader headerA;
    headerA.offset = 10;
    headerA.addr = 100;
    headerA.size = 20;

    sectionHeaders.push_back(headerA);

    extendProgramHeaderSizeToCoverSections(programHeader, sectionHeaders);

    REQUIRE( programHeader.offset == 10 );
    REQUIRE( programHeader.vaddr == 100 );
    REQUIRE( programHeader.paddr == 100 );
    REQUIRE( programHeader.memsz == 20 );
    REQUIRE( programHeader.filesz == 20 );
  }

  SECTION("1 section offset: 30 address: 130")
  {
    /*
     * offset  10 - 19|20 - 29|30 - 49|50
     * address 100-119|120-129|130-149|150
     * section                |   A   |
     * segment |                      |
     */

    SectionHeader headerA;
    headerA.offset = 30;
    headerA.addr = 130;
    headerA.size = 20;

    sectionHeaders.push_back(headerA);

    extendProgramHeaderSizeToCoverSections(programHeader, sectionHeaders);

    REQUIRE( programHeader.offset == 10 );
    REQUIRE( programHeader.vaddr == 100 );
    REQUIRE( programHeader.paddr == 100 );
    REQUIRE( programHeader.memsz == 50 );
    REQUIRE( programHeader.filesz == 40 );
  }

  SECTION("2 section with a hole in between")
  {
    /*
     * offset  10 - 29|30 - 49|50 - 69|70
     * address 100-129|130-149|150-169|170
     * section |  A   |.......|   B   |
     * segment |                      |
     */

    SectionHeader headerA;
    headerA.offset = 10;
    headerA.addr = 100;
    headerA.size = 20;

    SectionHeader headerB;
    headerB.offset = 50;
    headerB.addr = 150;
    headerB.size = 20;

    sectionHeaders.push_back(headerA);
    sectionHeaders.push_back(headerB);

    extendProgramHeaderSizeToCoverSections(programHeader, sectionHeaders);

    REQUIRE( programHeader.offset == 10 );
    REQUIRE( programHeader.vaddr == 100 );
    REQUIRE( programHeader.paddr == 100 );
    REQUIRE( programHeader.memsz == 70 );
    REQUIRE( programHeader.filesz == 60 );
  }
}

TEST_CASE("makeNoteProgramHeaderCoveringSections")
{
  std::vector<SectionHeader> sectionHeaders;
  ProgramHeader noteProgramHeader;

  SECTION(".note.ABI-tag")
  {
    SectionHeader noteAbiTagHeader = makeNoteSectionHeader(".note.ABI-tag");

    SECTION("file offset and memory v address are the same (like Gcc)")
    {
      noteAbiTagHeader.offset = 0x254;
      noteAbiTagHeader.addr = 0x254;
      noteAbiTagHeader.size = 32;
      noteAbiTagHeader.addralign = 4;

      sectionHeaders.push_back(noteAbiTagHeader);

      noteProgramHeader = makeNoteProgramHeaderCoveringSections(sectionHeaders);
      REQUIRE( noteProgramHeader.segmentType() == SegmentType::Note );
      REQUIRE( noteProgramHeader.offset == 0x254 );
      REQUIRE( noteProgramHeader.vaddr == 0x254 );
      REQUIRE( noteProgramHeader.filesz == 32 );
      REQUIRE( noteProgramHeader.memsz == 32 );
      REQUIRE( noteProgramHeader.flags == 0x04 );
      REQUIRE( noteProgramHeader.align == 4 );
    }

    SECTION("file offset and memory v address are different (like Clang)")
    {
      noteAbiTagHeader.offset = 0x254;
      noteAbiTagHeader.addr = 0x400254;
      noteAbiTagHeader.size = 32;
      noteAbiTagHeader.addralign = 4;

      sectionHeaders.push_back(noteAbiTagHeader);

      noteProgramHeader = makeNoteProgramHeaderCoveringSections(sectionHeaders);
      REQUIRE( noteProgramHeader.segmentType() == SegmentType::Note );
      REQUIRE( noteProgramHeader.offset == 0x254 );
      REQUIRE( noteProgramHeader.vaddr == 0x400254 );
      REQUIRE( noteProgramHeader.filesz == 32 );
      REQUIRE( noteProgramHeader.memsz == 32 );
      REQUIRE( noteProgramHeader.flags == 0x04 );
      REQUIRE( noteProgramHeader.align == 4 );
    }
  }

  SECTION(".note.ABI-tag and .note.gnu.build-id")
  {
    SectionHeader noteAbiTagHeader = makeNoteSectionHeader(".note.ABI-tag");
    noteAbiTagHeader.offset = 50;
    noteAbiTagHeader.addr = 150;
    noteAbiTagHeader.size = 10;
    noteAbiTagHeader.addralign = 4;

    SectionHeader noteGnuBuilIdHeader = makeNoteSectionHeader(".note.gnu.build-id");
    noteGnuBuilIdHeader.offset = 60;
    noteGnuBuilIdHeader.addr = 160;
    noteGnuBuilIdHeader.size = 12;
    noteGnuBuilIdHeader.addralign = 4;

    sectionHeaders.push_back(noteAbiTagHeader);
    sectionHeaders.push_back(noteGnuBuilIdHeader);

    noteProgramHeader = makeNoteProgramHeaderCoveringSections(sectionHeaders);
    REQUIRE( noteProgramHeader.segmentType() == SegmentType::Note );
    REQUIRE( noteProgramHeader.offset == 50 );
    REQUIRE( noteProgramHeader.vaddr == 150 );
    REQUIRE( noteProgramHeader.filesz == 22 );
    REQUIRE( noteProgramHeader.memsz == 22 );
    REQUIRE( noteProgramHeader.flags == 0x04 );
    REQUIRE( noteProgramHeader.align == 4 );
  }
}

TEST_CASE("makeLoadProgramHeaderCoveringSections")
{
  std::vector<SectionHeader> sectionHeaders;
  ProgramHeader loadProgramHeader;

  SECTION(".note.ABI-tag")
  {
    SectionHeader noteAbiTagHeader = makeNoteSectionHeader(".note.ABI-tag");
    noteAbiTagHeader.flags = 0x02;
    noteAbiTagHeader.offset = 0x254;
    noteAbiTagHeader.addr = 0x400254;
    noteAbiTagHeader.size = 32;
    noteAbiTagHeader.addralign = 4;

    sectionHeaders.push_back(noteAbiTagHeader);

    loadProgramHeader = makeLoadProgramHeaderCoveringSections(sectionHeaders, 4096);
    REQUIRE( loadProgramHeader.segmentType() == SegmentType::Load );
    REQUIRE( loadProgramHeader.offset == 0x254 );
    REQUIRE( loadProgramHeader.vaddr == 0x400254 );
    REQUIRE( loadProgramHeader.filesz == 32 );
    REQUIRE( loadProgramHeader.memsz == 32 );
    REQUIRE( loadProgramHeader.isReadable() );
    REQUIRE( !loadProgramHeader.isWritable() );
    REQUIRE( loadProgramHeader.align == 4096 );
  }

  SECTION(".note.ABI-tag , .dynamic (there is a hole between the sections)")
  {
    SectionHeader noteAbiTagHeader = makeNoteSectionHeader(".note.ABI-tag");
    noteAbiTagHeader.flags = 0x02;
    noteAbiTagHeader.offset = 50;
    noteAbiTagHeader.addr = 500;
    noteAbiTagHeader.size = 20;
    noteAbiTagHeader.addralign = 4;

    SectionHeader dynamicHeader = makeDynamicSectionHeader();
    dynamicHeader.flags = 0x03;
    dynamicHeader.offset = 80;
    dynamicHeader.addr = 800;
    dynamicHeader.size = 40;
    dynamicHeader.addralign = 8;

    sectionHeaders.push_back(noteAbiTagHeader);
    sectionHeaders.push_back(dynamicHeader);

    loadProgramHeader = makeLoadProgramHeaderCoveringSections(sectionHeaders, 4096);
    REQUIRE( loadProgramHeader.segmentType() == SegmentType::Load );
    REQUIRE( loadProgramHeader.offset == 50 );
    REQUIRE( loadProgramHeader.vaddr == 500 );
    // file size: 80 + 40 - 50 = 70
    REQUIRE( loadProgramHeader.filesz == 70 );
    // memory size: 800 + 40 - 500 = 340
    REQUIRE( loadProgramHeader.memsz == 340 );
    REQUIRE( loadProgramHeader.isReadable() );
    REQUIRE( loadProgramHeader.isWritable() );
    REQUIRE( loadProgramHeader.align == 4096 );
  }
}
