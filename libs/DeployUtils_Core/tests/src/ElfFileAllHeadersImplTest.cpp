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
#include "ElfProgramHeaderTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/FileAllHeaders.h"
#include <cassert>

using Mdt::DeployUtils::Impl::Elf::FileAllHeaders;
using Mdt::DeployUtils::Impl::Elf::FileHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeaderTable;
using Mdt::DeployUtils::Impl::Elf::SectionHeader;


// ProgramHeader makeNullProgramHeader()
// {
//   ProgramHeader header;
//   header.type = 0;
// 
//   return header;
// }

// ProgramHeader makeDynamicSectionProgramHeader()
// {
//   ProgramHeader header;
//   header.type = 2;
// 
//   return header;
// }


// std::vector<ProgramHeader> makeProgramHeaderTable(int n)
// {
//   assert(n > 0);
// 
//   return std::vector<ProgramHeader>( n, makeNullProgramHeader() );
// }

SectionHeader makeNullSectionHeader()
{
  SectionHeader header;
  header.type = 0;

  return header;
}

SectionHeader makeDynamicSectionHeader()
{
  SectionHeader header;
  header.type = 6;

  return header;
}

SectionHeader makeStringTableSectionHeader()
{
  SectionHeader header;
  header.type = 3;

  return header;
}

std::vector<SectionHeader> makeSectionHeaderTable(int n)
{
  assert(n > 0);

  return std::vector<SectionHeader>( n, makeNullSectionHeader() );
}

struct TestHeadersSetup
{
  uint64_t programHeaderTableOffset = 0;
  uint64_t sectionHeaderTableOffset = 0;
  uint64_t dynamicSectionOffset = 0;
  uint64_t dynamicSectionFileSize = 0;
  uint64_t dynamicSectionAddress = 0;
  uint64_t dynamicStringTableOffset = 0;
  uint64_t dynamicStringTableFileSize = 0;
  uint64_t dynamicStringTableAddress = 0;
  uint64_t sectionNameStringTableOffset = 0;
};

FileAllHeaders makeTestHeaders(const TestHeadersSetup & setup)
{
  FileAllHeaders headers;
  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  fileHeader.phoff = setup.programHeaderTableOffset;
  fileHeader.shoff = setup.sectionHeaderTableOffset;

  ProgramHeader dynamicSectionProgramHeader = makeDynamicSectionProgramHeader();
  dynamicSectionProgramHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionProgramHeader.filesz = setup.dynamicSectionFileSize;
  dynamicSectionProgramHeader.vaddr = setup.dynamicSectionAddress;
  dynamicSectionProgramHeader.paddr = setup.dynamicSectionAddress;

  SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
  dynamicSectionHeader.name = ".dynamic";
  dynamicSectionHeader.offset = setup.dynamicSectionOffset;
  dynamicSectionHeader.size = setup.dynamicSectionFileSize;
  dynamicSectionHeader.addr = setup.dynamicSectionAddress;
  dynamicSectionHeader.link = 2;

  SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
  dynamicStringTableSectionHeader.name = ".dynstr";
  dynamicStringTableSectionHeader.offset = setup.dynamicStringTableOffset;
  dynamicStringTableSectionHeader.size = setup.dynamicStringTableFileSize;
  dynamicStringTableSectionHeader.addr = setup.dynamicStringTableAddress;

  SectionHeader sectionNameStringTableHeader = makeStringTableSectionHeader();
  sectionNameStringTableHeader.name = "shstrtab";
  sectionNameStringTableHeader.offset = setup.sectionNameStringTableOffset;

  ProgramHeaderTable programHeaderTable;
  programHeaderTable.addHeaderFromFile(dynamicSectionProgramHeader);

  std::vector<SectionHeader> sectionHeaderTable;
  sectionHeaderTable.push_back( makeNullSectionHeader() );
  sectionHeaderTable.push_back(dynamicSectionHeader);
  sectionHeaderTable.push_back(dynamicStringTableSectionHeader);
  sectionHeaderTable.push_back(sectionNameStringTableHeader);

  fileHeader.shstrndx = 3;

  headers.setFileHeader(fileHeader);
  headers.setProgramHeaderTable(programHeaderTable);
  headers.setSectionHeaderTable(sectionHeaderTable);

  return headers;
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

//     SECTION("edit the dynamic program header")
//     {
//       dynamicSectionProgramHeader.offset = 88;
//       allHeaders.setDynamicProgramHeader(dynamicSectionProgramHeader);
//       REQUIRE( allHeaders.dynamicProgramHeader().offset == 88 );
//     }
  }
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

    SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
    dynamicStringTableSectionHeader.offset = 48;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(dynamicSectionHeader);
    sectionHeaderTable.push_back(dynamicStringTableSectionHeader);

    allHeaders.setSectionHeaderTable(sectionHeaderTable);
    REQUIRE( allHeaders.containsDynamicSectionHeader() );
    REQUIRE( allHeaders.dynamicSectionHeader().offset == 72 );
    REQUIRE( allHeaders.containsDynamicStringTableSectionHeader() );
    REQUIRE( allHeaders.dynamicStringTableSectionHeader().offset == 48 );

    SECTION("edit the dynamic section header")
    {
      dynamicSectionHeader.offset = 80;
      allHeaders.setDynamicSectionHeader(dynamicSectionHeader);
      REQUIRE( allHeaders.dynamicSectionHeader().offset == 80 );
    }

    SECTION("edit the dynamic string table section header")
    {
      dynamicStringTableSectionHeader.offset = 90;
      allHeaders.setDynamicStringTableSectionHeader(dynamicStringTableSectionHeader);
      REQUIRE( allHeaders.dynamicStringTableSectionHeader().offset == 90 );
    }
  }
}

TEST_CASE("setDynamicSectionFileSize")
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

  allHeaders.setDynamicSectionFileSize(120);

  REQUIRE( allHeaders.dynamicProgramHeader().filesz == 120 );
  REQUIRE( allHeaders.dynamicSectionHeader().size == 120 );
}

TEST_CASE("setDynamicStringTableFileSize")
{
  TestHeadersSetup setup;
  setup.dynamicStringTableAddress = 100;
  setup.dynamicStringTableFileSize = 10;

  FileAllHeaders allHeaders = makeTestHeaders(setup);

  allHeaders.setDynamicStringTableFileSize(25);

  REQUIRE( allHeaders.dynamicStringTableSectionHeader().size == 25 );
}

TEST_CASE("globalFileOffsetRange")
{
  TestHeadersSetup setup;
  FileAllHeaders allHeaders;

  SECTION("section header table is at the end of the file (the common case)")
  {
    setup.programHeaderTableOffset = 50;
    setup.dynamicSectionOffset = 100;
    setup.dynamicSectionFileSize = 10;
    setup.dynamicStringTableOffset = 1'000;
    setup.dynamicStringTableFileSize = 100;
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
    setup.dynamicSectionFileSize = 10;
    setup.dynamicStringTableOffset = 10'000;
    setup.dynamicStringTableFileSize = 100;
    setup.sectionHeaderTableOffset = 2'000;
    allHeaders = makeTestHeaders(setup);
    REQUIRE( allHeaders.seemsValid() );

    const uint64_t expectedEnd = setup.dynamicStringTableOffset + setup.dynamicStringTableFileSize;

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
