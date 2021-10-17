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
#include "Mdt/DeployUtils/Impl/Elf/ProgramHeaderTable.h"

using Mdt::DeployUtils::Impl::Elf::SegmentType;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::ProgramHeaderTable;

TEST_CASE("ProgramHeaderTableHeader_PT_PHDR")
{
  ProgramHeaderTable table;

  SECTION("default constructed")
  {
    REQUIRE( !table.containsProgramHeaderTableHeader() );
  }

  SECTION("add PT_PHDR")
  {
    table.addHeaderFromFile( makeProgramHeaderTableProgramHeader() );
    REQUIRE( table.containsProgramHeaderTableHeader() );
    REQUIRE( table.programHeaderTableHeader().segmentType() == SegmentType::ProgramHeaderTable );
  }
}

TEST_CASE("addHeader")
{
  ProgramHeaderTable table;

  SECTION("table does not contain the program header table (PT_PHDR)")
  {
    ProgramHeader dynamicSectionHeader = makeDynamicSectionProgramHeader();

    table.addHeader(dynamicSectionHeader, 50);

    REQUIRE( table.headerCount() == 1 );
    REQUIRE( table.containsDynamicSectionHeader() );
  }

  SECTION("table contains the program header table (PT_PHDR)")
  {
    ProgramHeader programHeaderTableHeader = makeProgramHeaderTableProgramHeader();
    ProgramHeader dynamicSectionHeader = makeDynamicSectionProgramHeader();

    table.addHeader(programHeaderTableHeader, 50);
    table.addHeader(dynamicSectionHeader, 50);

    REQUIRE( table.headerCount() == 2 );
    REQUIRE( table.containsProgramHeaderTableHeader() );
    REQUIRE( table.programHeaderTableHeader().memsz == 100 );
    REQUIRE( table.programHeaderTableHeader().filesz == 100 );
    REQUIRE( table.containsDynamicSectionHeader() );
  }
}

TEST_CASE("DynamicSection")
{
  ProgramHeaderTable table;
  ProgramHeader dynamicSectionHeader;

  SECTION("default constructed")
  {
    REQUIRE( !table.containsDynamicSectionHeader() );
  }

  SECTION("add dynamic section to table")
  {
    dynamicSectionHeader = makeDynamicSectionProgramHeader();
    dynamicSectionHeader.offset = 100;

    table.addHeaderFromFile(dynamicSectionHeader);

    REQUIRE( table.containsDynamicSectionHeader() );
    REQUIRE( table.dynamicSectionHeader().offset == 100 );
  }

  SECTION("add other segment header")
  {
    table.addHeaderFromFile( makeNullProgramHeader() );
    REQUIRE( !table.containsDynamicSectionHeader() );
  }
}

TEST_CASE("findLastSegmentVirtualAddressEnd")
{
  ProgramHeaderTable table;

  ProgramHeader dynamicSectionHeader = makeDynamicSectionProgramHeader();
  dynamicSectionHeader.vaddr = 1000;
  dynamicSectionHeader.memsz = 100;

  table.addHeaderFromFile(dynamicSectionHeader);

  REQUIRE( table.findLastSegmentVirtualAddressEnd() == 1100 );
}

TEST_CASE("findLastSegmentFileOffsetEnd")
{
  ProgramHeaderTable table;

  ProgramHeader dynamicSectionHeader = makeDynamicSectionProgramHeader();
  dynamicSectionHeader.offset = 1000;
  dynamicSectionHeader.filesz = 100;

  table.addHeaderFromFile(dynamicSectionHeader);

  REQUIRE( table.findLastSegmentFileOffsetEnd() == 1100 );
}
