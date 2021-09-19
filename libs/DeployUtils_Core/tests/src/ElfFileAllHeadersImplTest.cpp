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
#include "Mdt/DeployUtils/Impl/Elf/FileAllHeaders.h"
#include <cassert>

using Mdt::DeployUtils::Impl::Elf::FileAllHeaders;
using Mdt::DeployUtils::Impl::Elf::FileHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::SectionHeader;


ProgramHeader makeNullProgramHeader()
{
  ProgramHeader header;
  header.type = 0;

  return header;
}

ProgramHeader makeDynamicSectionProgramHeader()
{
  ProgramHeader header;
  header.type = 2;

  return header;
}


std::vector<ProgramHeader> makeProgramHeaderTable(int n)
{
  assert(n > 0);

  return std::vector<ProgramHeader>( n, makeNullProgramHeader() );
}

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
  std::vector<ProgramHeader> programHeaderTable;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsProgramHeaderTable() );
    REQUIRE( !allHeaders.containsDynamicProgramHeader() );
  }

  SECTION("2 program headers")
  {
    programHeaderTable.push_back( makeNullProgramHeader() );
    programHeaderTable.push_back( makeNullProgramHeader() );

    allHeaders.setProgramHeaderTable(programHeaderTable);

    REQUIRE( allHeaders.programHeaderTable().size() == 2 );
    REQUIRE( allHeaders.fileHeader().phnum == 2 );
    REQUIRE( allHeaders.containsProgramHeaderTable() );
    REQUIRE( !allHeaders.containsDynamicProgramHeader() );
  }

  SECTION("3 program headers")
  {
    programHeaderTable.push_back( makeNullProgramHeader() );
    programHeaderTable.push_back( makeNullProgramHeader() );
    programHeaderTable.push_back( makeNullProgramHeader() );

    allHeaders.setProgramHeaderTable(programHeaderTable);

    REQUIRE( allHeaders.programHeaderTable().size() == 3 );
    REQUIRE( allHeaders.fileHeader().phnum == 3 );
  }

  SECTION("also contain the dynamic program header")
  {
    ProgramHeader dynamicSectionProgramHeader = makeDynamicSectionProgramHeader();
    dynamicSectionProgramHeader.offset = 46;

    programHeaderTable.push_back( makeNullProgramHeader() );
    programHeaderTable.push_back(dynamicSectionProgramHeader);

    allHeaders.setProgramHeaderTable(programHeaderTable);
    REQUIRE( allHeaders.containsDynamicProgramHeader() );
    REQUIRE( allHeaders.dynamicProgramHeader().offset == 46 );
  }
}

TEST_CASE("SectionHeaderTable")
{
  FileAllHeaders allHeaders;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("default constructed")
  {
    REQUIRE( !allHeaders.containsSectionHeaderTable() );
    REQUIRE( !allHeaders.containsDynamicSectionHeader() );
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
  }

  SECTION("3 section headers")
  {
    allHeaders.setSectionHeaderTable( makeSectionHeaderTable(3) );

    REQUIRE( allHeaders.sectionHeaderTable().size() == 3 );
    REQUIRE( allHeaders.fileHeader().shnum == 3 );
  }

  SECTION("also contains the dynamic section header")
  {
    SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
    dynamicSectionHeader.offset = 72;

    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(dynamicSectionHeader);

    allHeaders.setSectionHeaderTable(sectionHeaderTable);
    REQUIRE( allHeaders.containsDynamicSectionHeader() );
    REQUIRE( allHeaders.dynamicSectionHeader().offset == 72 );
  }
}

TEST_CASE("seemsValid")
{
  FileAllHeaders allHeaders;
  FileHeader fileHeader = make64BitLittleEndianFileHeader();
  std::vector<ProgramHeader> programHeaderTable;
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

    programHeaderTable.push_back( makeDynamicSectionProgramHeader() );

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

//   SECTION("only 3 program headers")
//   {
//     allHeaders.setProgramHeaderTable( makeProgramHeaderTable(3) );
//     fileHeader.phnum = 3;
//     fileHeader.shnum = 0;
// 
//     SECTION("file header is correct")
//     {
//       allHeaders.setFileHeader(fileHeader);
//       REQUIRE( allHeaders.seemsValid() );
//     }
// 
//     SECTION("file header declares 2 section headers")
//     {
//       fileHeader.shnum = 2;
//       allHeaders.setFileHeader(fileHeader);
//       REQUIRE( !allHeaders.seemsValid() );
//     }
//   }

//   SECTION("3 program headers and 2 section headers")
//   {
//     allHeaders.setProgramHeaderTable( makeProgramHeaderTable(3) );
//     allHeaders.setSectionHeaderTable( makeSectionHeaderTable(2) );
//     fileHeader.phnum = 3;
//     fileHeader.shnum = 2;
// 
//     SECTION("file header is correct")
//     {
//       allHeaders.setFileHeader(fileHeader);
//       REQUIRE( allHeaders.seemsValid() );
//     }
//   }

}
