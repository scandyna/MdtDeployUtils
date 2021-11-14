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
#include "ElfFileAllHeadersImplTestUtils.h"
#include "ElfFileIoTestUtils.h"
#include "ElfProgramHeaderTestUtils.h"
#include "ElfSectionHeaderTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/FileAllHeaders.h"
#include "Mdt/DeployUtils/Impl/Elf/SectionSegmentUtils.h"
#include <cassert>

using Mdt::DeployUtils::Impl::Elf::FileAllHeaders;
using Mdt::DeployUtils::Impl::Elf::MoveSectionAlignment;
using Mdt::DeployUtils::Impl::Elf::FileHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::SegmentType;
using Mdt::DeployUtils::Impl::Elf::ProgramHeaderTable;
using Mdt::DeployUtils::Impl::Elf::SectionHeader;

std::vector<SectionHeader> makeSectionHeaderTable(int n)
{
  assert(n > 0);

  return std::vector<SectionHeader>( n, makeNullSectionHeader() );
}


TEST_CASE("FileHeader")
{
  FileAllHeaders allHeaders;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.fileHeaderSeemsValid() );
  }

//   SECTION("set a invalid file header")
//   {
//     FileHeader fileHeader;
//     REQUIRE( !fileHeader.seemsValid() );
//     allHeaders.setFileHeader(fileHeader);
//     REQUIRE( !allHeaders.fileHeaderSeemsValid() );
//   }

  SECTION("set a valid file header")
  {
    allHeaders.setFileHeader( make64BitLittleEndianFileHeader() );
    REQUIRE( allHeaders.fileHeaderSeemsValid() );
  }
}

TEST_CASE("ProgramHeaderTable")
{
  FileAllHeaders allHeaders;
  ProgramHeaderTable programHeaderTable;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsProgramHeaderTable() );
    REQUIRE( !allHeaders.containsDynamicProgramHeader() );
    REQUIRE( !allHeaders.containsProgramHeaderTableProgramHeader() );
    REQUIRE( allHeaders.fileHeader().phnum == 0 );
  }

  SECTION("2 program headers")
  {
    programHeaderTable.addHeaderFromFile( makeNullProgramHeader() );
    programHeaderTable.addHeaderFromFile( makeNullProgramHeader() );

    allHeaders.setProgramHeaderTable(programHeaderTable);

    REQUIRE( allHeaders.programHeaderTable().headerCount() == 2 );
    REQUIRE( allHeaders.fileHeader().phnum == 2 );
    REQUIRE( allHeaders.containsProgramHeaderTable() );
    REQUIRE( !allHeaders.containsDynamicProgramHeader() );
  }

  SECTION("3 program headers")
  {
    programHeaderTable.addHeaderFromFile( makeNullProgramHeader() );
    programHeaderTable.addHeaderFromFile( makeNullProgramHeader() );
    programHeaderTable.addHeaderFromFile( makeNullProgramHeader() );

    allHeaders.setProgramHeaderTable(programHeaderTable);

    REQUIRE( allHeaders.programHeaderTable().headerCount() == 3 );
    REQUIRE( allHeaders.fileHeader().phnum == 3 );
  }

  SECTION("also contain the dynamic program header")
  {
    ProgramHeader dynamicSectionProgramHeader = makeDynamicSectionProgramHeader();
    dynamicSectionProgramHeader.offset = 46;

    programHeaderTable.addHeaderFromFile( makeNullProgramHeader() );
    programHeaderTable.addHeaderFromFile(dynamicSectionProgramHeader);

    allHeaders.setProgramHeaderTable(programHeaderTable);
    REQUIRE( allHeaders.containsDynamicProgramHeader() );
    REQUIRE( allHeaders.dynamicProgramHeader().offset == 46 );
  }

  SECTION("also contains the program header table program header (PT_PHD)")
  {
    programHeaderTable.addHeaderFromFile( makeProgramHeaderTableProgramHeader() );
    allHeaders.setProgramHeaderTable(programHeaderTable);

    REQUIRE( allHeaders.containsProgramHeaderTableProgramHeader() );
    REQUIRE( allHeaders.programHeaderTableProgramHeader().segmentType() == SegmentType::ProgramHeaderTable );
  }
}

TEST_CASE("addProgramHeader")
{
  FileAllHeaders allHeaders;

  allHeaders.addProgramHeader( makeNullProgramHeader() );

  REQUIRE( allHeaders.programHeaderTable().headerCount() == 1 );
  REQUIRE( allHeaders.fileHeader().phnum == 1 );
}

TEST_CASE("SectionHeaderTable")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsSectionNameStringTableHeader() );
    REQUIRE( !allHeaders.containsSectionHeaderTable() );
    REQUIRE( !allHeaders.containsDynamicSectionHeader() );
    REQUIRE( !allHeaders.containsDynamicStringTableSectionHeader() );
  }

  SECTION("2 section headers")
  {
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back( makeNullSectionHeader() );

    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( allHeaders.sectionHeaderTable().size() == 2 );
    REQUIRE( allHeaders.fileHeader().shnum == 2 );
    REQUIRE( allHeaders.containsSectionHeaderTable() );
    REQUIRE( !allHeaders.containsDynamicSectionHeader() );
    REQUIRE( !allHeaders.containsDynamicStringTableSectionHeader() );
    REQUIRE( !allHeaders.containsDynamicStringTableSectionHeader() );
  }

  SECTION("3 section headers")
  {
    allHeaders.setSectionHeaderTable( makeSectionHeaderTable(3) );

    REQUIRE( allHeaders.sectionHeaderTable().size() == 3 );
    REQUIRE( allHeaders.fileHeader().shnum == 3 );
  }

  SECTION("also contains the section name string table")
  {
    SectionHeader snStringTableHeader = makeStringTableSectionHeader();
    snStringTableHeader.offset = 152;

    FileHeader fileHeader = make64BitLittleEndianFileHeader();
    fileHeader.shstrndx = 1;
    allHeaders.setFileHeader(fileHeader);

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(snStringTableHeader);

    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( allHeaders.containsSectionNameStringTableHeader() );
    REQUIRE( allHeaders.sectionNameStringTableHeader().offset == 152 );
  }

  SECTION("also contains the dynamic section header and dynamic string table section header")
  {
    SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
    dynamicSectionHeader.offset = 72;
    dynamicSectionHeader.link = 2;

    SectionHeader dynamicStringTableSectionHeader = makeDynamicStringTableSectionHeader();
    dynamicStringTableSectionHeader.offset = 48;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(dynamicSectionHeader);
    sectionHeaderTable.push_back(dynamicStringTableSectionHeader);

    allHeaders.setSectionHeaderTable(sectionHeaderTable);
    REQUIRE( allHeaders.containsDynamicSectionHeader() );
    REQUIRE( allHeaders.dynamicSectionHeader().offset == 72 );
    REQUIRE( allHeaders.containsDynamicStringTableSectionHeader() );
    REQUIRE( allHeaders.dynamicStringTableSectionHeader().offset == 48 );
  }
}

TEST_CASE("sortSectionHeaderTableByFileOffset")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("the section header string table section (.shstrtab) is also sorted")
  {
    SectionHeader shtStringTable = makeStringTableSectionHeader(".shstrtab");
    shtStringTable.offset = 50;

    SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
    dynamicSectionHeader.offset = 100;

    FileHeader fileHeader = make64BitLittleEndianFileHeader();
    fileHeader.shstrndx = 2;
    allHeaders.setFileHeader(fileHeader);

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(dynamicSectionHeader);
    sectionHeaderTable.push_back(shtStringTable);

    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    allHeaders.sortSectionHeaderTableByFileOffset();

    REQUIRE( allHeaders.sectionHeaderTable()[1].name == ".shstrtab" );
    REQUIRE( allHeaders.sectionHeaderTable()[2].name == ".dynamic" );
    REQUIRE( allHeaders.fileHeader().shstrndx == 1 );
  }
}

TEST_CASE("gotSectionHeader")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsGotSectionHeader() );
  }

  SECTION("does not contains the got section header")
  {
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back( makeStringTableSectionHeader() );
    sectionHeaderTable.push_back( makeGotPltSectionHeader() );

    REQUIRE( !allHeaders.containsGotSectionHeader() );
  }

  SECTION("contains the got section header")
  {
    SectionHeader gotSectionHeader = makeGotSectionHeader();
    gotSectionHeader.offset = 25;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back( makeStringTableSectionHeader() );
    sectionHeaderTable.push_back(gotSectionHeader);
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( allHeaders.containsGotSectionHeader() );
    REQUIRE( allHeaders.gotSectionHeader().offset == 25 );
  }
}

TEST_CASE("gotPltSectionHeader")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsGotPltSectionHeader() );
  }

  SECTION("does not contains the .got.plt section header")
  {
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back( makeStringTableSectionHeader() );
    sectionHeaderTable.push_back( makeGotSectionHeader() );

    REQUIRE( !allHeaders.containsGotPltSectionHeader() );
  }

  SECTION("contains the got section header")
  {
    SectionHeader gotPltSectionHeader = makeGotPltSectionHeader();
    gotPltSectionHeader.offset = 46;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back( makeStringTableSectionHeader() );
    sectionHeaderTable.push_back(gotPltSectionHeader);
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( allHeaders.containsGotPltSectionHeader() );
    REQUIRE( allHeaders.gotPltSectionHeader().offset == 46 );
  }
}

TEST_CASE("programInterpreterSection")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;
  SectionHeader interpHeader;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsProgramInterpreterSectionHeader());
  }

  SECTION("add the PT_INTERP header")
  {
    interpHeader = makeProgramInterpreterSectionHeader();
    interpHeader.offset = 142;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(interpHeader);
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( allHeaders.containsProgramInterpreterSectionHeader() );
    REQUIRE( allHeaders.programInterpreterSectionHeader().offset == 142 );
  }

  SECTION("add a other program header")
  {
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( !allHeaders.containsProgramInterpreterSectionHeader() );
  }
}

TEST_CASE("gnuHashTableSectionHeader")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;
  SectionHeader gnuHashHeader = makeGnuHashTableSectionHeader();

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsGnuHashTableSectionHeader  ());
  }

  SECTION("add the .gnu.hash section header")
  {
    gnuHashHeader.offset = 156;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(gnuHashHeader);
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( allHeaders.containsGnuHashTableSectionHeader() );
    REQUIRE( allHeaders.gnuHashTableSectionHeader().offset == 156 );
  }

  SECTION("add a other section header")
  {
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( !allHeaders.containsGnuHashTableSectionHeader() );
  }
}

TEST_CASE("noteAbiTagSectionHeader")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;
  SectionHeader noteSectionHeader;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsNoteAbiTagSectionHeader());
  }

  SECTION("table contains the .note.ABI-tag section header")
  {
    noteSectionHeader = makeNoteSectionHeader(".note.ABI-tag");
    noteSectionHeader.offset = 25;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(noteSectionHeader);
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( allHeaders.containsNoteAbiTagSectionHeader() );
    REQUIRE( allHeaders.noteAbiTagSectionHeader().offset == 25 );
  }

  SECTION("add a other program header")
  {
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( !allHeaders.containsNoteAbiTagSectionHeader() );
  }
}

TEST_CASE("noteGnuBuildIdSectionHeader")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;
  SectionHeader noteSectionHeader;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsNoteGnuBuildIdSectionHeader());
  }

  SECTION("table contains the .note.gnu.build-id section header")
  {
    noteSectionHeader = makeNoteSectionHeader(".note.gnu.build-id");
    noteSectionHeader.offset = 26;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(noteSectionHeader);
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( allHeaders.containsNoteGnuBuildIdSectionHeader() );
    REQUIRE( allHeaders.noteGnuBuildIdSectionHeader().offset == 26 );
  }

  SECTION("add a other program header")
  {
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    allHeaders.setSectionHeaderTable(sectionHeaderTable);

    REQUIRE( !allHeaders.containsNoteGnuBuildIdSectionHeader() );
  }
}

TEST_CASE("setDynamicSectionSize")
{
  FileAllHeaders allHeaders;
  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  ProgramHeaderTable programHeaderTable;
  std::vector<SectionHeader> sectionHeaderTable;

  ProgramHeader dynamicSectionProgramHeader = makeDynamicSectionProgramHeader();
  dynamicSectionProgramHeader.filesz = 100;

  SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
  dynamicSectionHeader.size = 100;

  programHeaderTable.addHeaderFromFile(dynamicSectionProgramHeader);
  sectionHeaderTable.push_back( makeNullSectionHeader() );
  sectionHeaderTable.push_back(dynamicSectionHeader);

  allHeaders.setFileHeader(fileHeader);
  allHeaders.setProgramHeaderTable(programHeaderTable);
  allHeaders.setSectionHeaderTable(sectionHeaderTable);

  allHeaders.setDynamicSectionSize(120);

  REQUIRE( allHeaders.dynamicProgramHeader().filesz == 120 );
  REQUIRE( allHeaders.dynamicProgramHeader().memsz == 120 );
  REQUIRE( allHeaders.dynamicSectionHeader().size == 120 );
}

TEST_CASE("moveProgramHeaderTableToNextPageAfterEnd")
{
  TestHeadersSetup setup;
  setup.programHeaderTableOffset = 0x40;
  setup.dynamicSectionOffset = 100;
  setup.dynamicSectionSize = 10;
  setup.sectionHeaderTableOffset = 1'000;

  FileAllHeaders headers = makeTestHeaders(setup);
  const uint64_t originalVirtualMemoryEnd = headers.findLastSegmentVirtualAddressEnd();
  const uint64_t originalFileEnd = headers.findGlobalFileOffsetEnd();

  headers.moveProgramHeaderTableToNextPageAfterEnd();

  REQUIRE( headers.programHeaderTableProgramHeader().vaddr >= originalVirtualMemoryEnd );
  REQUIRE( (headers.programHeaderTableProgramHeader().vaddr % 2) == 0 );
  REQUIRE( headers.programHeaderTableProgramHeader().offset >= originalFileEnd );
  REQUIRE( headers.fileHeader().phoff == headers.programHeaderTableProgramHeader().offset );
}

TEST_CASE("moveProgramInterpreterSectionToEnd")
{
  TestHeadersSetup setup;
  setup.programHeaderTableOffset = 0x40;
  setup.programInterpreterSectionOffset = 200;
  setup.programInterpreterSectionAddress = 2000;
  setup.programInterpreterSectionSize = 10;
  setup.dynamicSectionOffset = 300;
  setup.dynamicSectionAddress = 3000;
  setup.dynamicSectionSize = 10;
  setup.sectionHeaderTableOffset = 1'000;

  FileAllHeaders headers = makeTestHeaders(setup);
  const uint64_t originalVirtualAddressEnd = headers.findLastSegmentVirtualAddressEnd();
  const uint64_t originalFileEnd = headers.findGlobalFileOffsetEnd();

  headers.moveProgramInterpreterSectionToEnd(MoveSectionAlignment::NextPage);

  SECTION(".interp section must have been moved past the end")
  {
    REQUIRE( headers.programInterpreterSectionHeader().addr >= originalVirtualAddressEnd );
    REQUIRE( (headers.programInterpreterSectionHeader().addr % 2) == 0 );
    REQUIRE( headers.programInterpreterSectionHeader().offset >= originalFileEnd );
  }

  SECTION("the PT_INTERP segment must cover the .interp section")
  {
    REQUIRE( headers.programInterpreterProgramHeader().vaddr == headers.programInterpreterSectionHeader().addr );
    REQUIRE( headers.programInterpreterProgramHeader().memsz == headers.programInterpreterSectionHeader().size );
    REQUIRE( headers.programInterpreterProgramHeader().offset == headers.programInterpreterSectionHeader().offset );
    REQUIRE( headers.programInterpreterProgramHeader().filesz == headers.programInterpreterSectionHeader().size );
  }
}

TEST_CASE("moveNoteSectionsToEnd")
{
  TestHeadersSetup setup;
  setup.programHeaderTableOffset = 50;
  setup.noteAbiTagSectionOffset = 100;
  setup.noteAbiTagSectionAddress = 200;
  setup.noteAbiTagSectionSize = 10;
  setup.noteGnuBuilIdSectionOffset = 110;
  setup.noteGnuBuilIdSectionAddress = 210;
  setup.noteGnuBuilIdSectionSize = 10;
  setup.sectionHeaderTableOffset = 1'000;

  FileAllHeaders headers = makeTestHeaders(setup);
  const uint64_t originalVirtualAddressEnd = headers.findLastSegmentVirtualAddressEnd();
  const uint64_t originalFileEnd = headers.findGlobalFileOffsetEnd();

  headers.moveNoteSectionsToEnd(MoveSectionAlignment::SectionAlignment);

  const auto noteSectionHeaders = headers.getNoteSectionHeaders();
  REQUIRE( noteSectionHeaders.size() == 2 );

  SECTION("note sections must have been moved to the end")
  {
    REQUIRE( noteSectionHeaders[0].addr >= originalVirtualAddressEnd );
    REQUIRE( noteSectionHeaders[0].offset >= originalFileEnd );
    REQUIRE( noteSectionHeaders[1].addr >= originalVirtualAddressEnd );
    REQUIRE( noteSectionHeaders[1].offset >= originalFileEnd );
  }

  SECTION("the PT_NOTE segment must cover the new location of the note sections")
  {
    REQUIRE( headers.noteProgramHeader().vaddr >= originalVirtualAddressEnd );
    REQUIRE( headers.noteProgramHeader().offset >= originalFileEnd );
    REQUIRE( headers.noteProgramHeader().memsz == 20 );
    REQUIRE( headers.noteProgramHeader().filesz == 20 );
  }
}

TEST_CASE("moveDynamicSectionToEnd")
{
  TestHeadersSetup setup;
  setup.dynamicSectionOffset = 100;
  setup.dynamicSectionAddress = 200;
  setup.dynamicSectionAlignment = 8;
  setup.dynamicSectionSize = 10;
  setup.sectionHeaderTableOffset = 1'000;

  FileAllHeaders headers = makeTestHeaders(setup);
  const uint64_t originalVirtualAddressEnd = headers.findLastSegmentVirtualAddressEnd();
  const uint64_t originalFileEnd = headers.findGlobalFileOffsetEnd();

  headers.moveDynamicSectionToEnd(MoveSectionAlignment::SectionAlignment);

  SECTION("the new virtual address must be at end and aligned (dynamic section has its own segment)")
  {
    REQUIRE( headers.dynamicProgramHeader().vaddr >= originalVirtualAddressEnd );
    REQUIRE( (headers.dynamicProgramHeader().vaddr % setup.dynamicSectionAlignment) == 0 );
  }

  SECTION("the new file offset must be at least at end and congruent to the virtual address modulo page size")
  {
    REQUIRE( headers.dynamicProgramHeader().offset >= originalFileEnd );

    const uint64_t pageSize = headers.fileHeader().pageSize();
    const uint64_t virtualAddressRemainder = headers.dynamicProgramHeader().vaddr % pageSize;
    const uint64_t fileOffsetRemainder = headers.dynamicProgramHeader().offset % pageSize;
    REQUIRE( fileOffsetRemainder == virtualAddressRemainder );
  }

  SECTION("section header and program header must be in sync")
  {
    REQUIRE( headers.dynamicSectionHeader().addr == headers.dynamicProgramHeader().vaddr );
    REQUIRE( headers.dynamicSectionHeader().offset == headers.dynamicSectionHeader().offset );
  }
}

TEST_CASE("moveDynamicStringTableToEnd")
{
  TestHeadersSetup setup;
  setup.dynamicStringTableOffset = 100;
  setup.dynamicStringTableAddress = 200;
  setup.dynamicStringTableSize = 10;
  setup.dynamicSectionOffset = 300;
  setup.dynamicSectionAddress = 500;
  setup.dynamicSectionSize = 15;
  setup.sectionHeaderTableOffset = 1'000;

  FileAllHeaders headers = makeTestHeaders(setup);
  const uint64_t originalVirtualAddressEnd = headers.findLastSegmentVirtualAddressEnd();
  const uint64_t originalFileEnd = headers.findGlobalFileOffsetEnd();

  headers.moveDynamicStringTableToEnd();

  SECTION("the new virtual address must be at end and modulo 2")
  {
    REQUIRE( headers.dynamicStringTableSectionHeader().addr >= originalVirtualAddressEnd );
    REQUIRE( (headers.dynamicStringTableSectionHeader().addr % 2) == 0 );
  }

  SECTION("the new file offset must be at least at end")
  {
    REQUIRE( headers.dynamicStringTableSectionHeader().offset >= originalFileEnd );
  }
}

TEST_CASE("setDynamicStringTableSize")
{
  TestHeadersSetup setup;
  setup.dynamicStringTableOffset = 100;
  setup.dynamicStringTableAddress = 100;
  setup.dynamicStringTableSize = 10;

  FileAllHeaders allHeaders = makeTestHeaders(setup);

  allHeaders.setDynamicStringTableSize(25);

  REQUIRE( allHeaders.dynamicStringTableSectionHeader().size == 25 );
}

TEST_CASE("findLastSegmentVirtualAddressEnd")
{
  TestHeadersSetup setup;
  FileAllHeaders allHeaders;

  SECTION("section header table is at the end of the file but it is not loaded into memory")
  {
    setup.programHeaderTableOffset = 50;
    setup.sectionHeaderTableOffset = 15'000;

    SECTION("the dynamic section is at the end of the file")
    {
      setup.dynamicStringTableOffset = 200;
      setup.dynamicStringTableSize = 10;
      setup.dynamicStringTableAddress = 2000;
      setup.dynamicSectionOffset = 300;
      setup.dynamicSectionSize = 20;
      setup.dynamicSectionAddress = 3000;

      allHeaders = makeTestHeaders(setup);
      REQUIRE( allHeaders.seemsValid() );

      const uint64_t expectedLastAddress = 3020;

      REQUIRE( allHeaders.findLastSegmentVirtualAddressEnd() == expectedLastAddress );
    }
  }
}

TEST_CASE("findGlobalFileOffsetEnd")
{
  TestHeadersSetup setup;
  FileAllHeaders allHeaders;

  SECTION("section header table is at the end of the file (the common case)")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicSectionSize = 10;
    setup.dynamicStringTableOffset = 1'000;
    setup.dynamicStringTableSize = 100;
    setup.sectionHeaderTableOffset = 10'000;
    allHeaders = makeTestHeaders(setup);
    REQUIRE( allHeaders.seemsValid() );

    const uint64_t expectedEnd = static_cast<uint64_t>( allHeaders.fileHeader().minimumSizeToReadAllSectionHeaders() );

    REQUIRE( allHeaders.findGlobalFileOffsetEnd() == expectedEnd );
  }

  SECTION("the dynamic string table is at the end of the file")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicSectionSize = 10;
    setup.dynamicStringTableOffset = 10'000;
    setup.dynamicStringTableAddress = 10'000;
    setup.dynamicStringTableSize = 100;
    setup.sectionHeaderTableOffset = 2'000;
    allHeaders = makeTestHeaders(setup);
    REQUIRE( allHeaders.seemsValid() );

    const uint64_t expectedEnd = setup.dynamicStringTableOffset + setup.dynamicStringTableSize;

    REQUIRE( allHeaders.findGlobalFileOffsetEnd() == expectedEnd );
  }
}

TEST_CASE("globalFileOffsetRange")
{
  TestHeadersSetup setup;
  FileAllHeaders allHeaders;

  SECTION("section header table is at the end of the file (the common case)")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicSectionSize = 10;
    setup.dynamicStringTableOffset = 1'000;
    setup.dynamicStringTableAddress = 1'000;
    setup.dynamicStringTableSize = 100;
    setup.sectionHeaderTableOffset = 10'000;
    allHeaders = makeTestHeaders(setup);
    REQUIRE( allHeaders.seemsValid() );

    const int64_t expectedMinimumSize = allHeaders.fileHeader().minimumSizeToReadAllSectionHeaders();

    REQUIRE( allHeaders.globalFileOffsetRange().begin() == 0 );
    REQUIRE( allHeaders.globalFileOffsetRange().minimumSizeToAccessRange() == expectedMinimumSize );
  }

  SECTION("the dynamic string table is at the end of the file")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicSectionSize = 10;
    setup.dynamicStringTableOffset = 10'000;
    setup.dynamicStringTableAddress = 10'000;
    setup.dynamicStringTableSize = 100;
    setup.sectionHeaderTableOffset = 2'000;
    allHeaders = makeTestHeaders(setup);
    REQUIRE( allHeaders.seemsValid() );

    const uint64_t expectedEnd = setup.dynamicStringTableOffset + setup.dynamicStringTableSize;

    REQUIRE( allHeaders.globalFileOffsetRange().begin() == 0 );
    REQUIRE( allHeaders.globalFileOffsetRange().end() == expectedEnd );
  }
}

TEST_CASE("seemsValid")
{
  FileAllHeaders allHeaders;
  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  ProgramHeaderTable programHeaderTable;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.seemsValid() );
  }

  SECTION("no program headers and no section headers")
  {
    fileHeader.phnum = 0;
    fileHeader.shnum = 0;

    SECTION("file header is correct")
    {
      allHeaders.setFileHeader(fileHeader);

      REQUIRE( allHeaders.seemsValid() );
    }

    SECTION("file header declares 2 program headers")
    {
      fileHeader.phnum = 2;
      allHeaders.setFileHeader(fileHeader);

      REQUIRE( !allHeaders.seemsValid() );
    }

    SECTION("file header declares 3 section headers")
    {
      fileHeader.shnum = 3;
      allHeaders.setFileHeader(fileHeader);

      REQUIRE( !allHeaders.seemsValid() );
    }
  }

  SECTION("dynamic section")
  {
    fileHeader.phnum = 0;
    fileHeader.shnum = 0;
    allHeaders.setFileHeader(fileHeader);

    programHeaderTable.addHeaderFromFile( makeDynamicSectionProgramHeader() );

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back( makeDynamicSectionHeader() );

    SECTION("contains the dynamic program header and section header")
    {
      allHeaders.setProgramHeaderTable(programHeaderTable);
      allHeaders.setSectionHeaderTable(sectionHeaderTable);

      REQUIRE( allHeaders.seemsValid() );
    }

    SECTION("contains the dynamic section header but not program header")
    {
      allHeaders.setSectionHeaderTable(sectionHeaderTable);

      REQUIRE( !allHeaders.seemsValid() );
    }
  }
}
