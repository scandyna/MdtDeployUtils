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
#include "Mdt/DeployUtils/Impl/Elf/NoteSectionTable.h"
#include <string>

using namespace Mdt::DeployUtils::Impl::Elf;

NoteSection makeNoteSection(const std::string & name)
{
  NoteSection section;

  section.name = name;

  return section;
}


TEST_CASE("updateSectionHeaderTable")
{
  NoteSectionTable noteSectionTable;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("both tables are empty")
  {
    noteSectionTable.updateSectionHeaderTable(sectionHeaderTable);

    REQUIRE( sectionHeaderTable.empty() );
  }

  SECTION("section header table is empty")
  {
    noteSectionTable.addSectionFromFile( makeNoteSectionHeader(".note.A"), makeNoteSection("A") );

    noteSectionTable.updateSectionHeaderTable(sectionHeaderTable);

    REQUIRE( sectionHeaderTable.empty() );
  }

  SECTION("section header table contains no note section")
  {
    sectionHeaderTable.emplace_back( makeNullSectionHeader() );

    noteSectionTable.addSectionFromFile( makeNoteSectionHeader(".note.A"), makeNoteSection("A") );

    noteSectionTable.updateSectionHeaderTable(sectionHeaderTable);

    REQUIRE( sectionHeaderTable.size() == 1 );
    REQUIRE( sectionHeaderTable[0].sectionType() == SectionType::Null );
  }

  SECTION("section header table contains a note section, but does not match any one in the note section table")
  {
    sectionHeaderTable.emplace_back( makeNoteSectionHeader(".note") );

    noteSectionTable.addSectionFromFile( makeNoteSectionHeader(".note.A"), makeNoteSection("A") );

    noteSectionTable.updateSectionHeaderTable(sectionHeaderTable);

    REQUIRE( sectionHeaderTable.size() == 1 );
    REQUIRE( sectionHeaderTable[0].name == ".note" );
  }

  SECTION("section header table contains a note section that matches one in the note section table")
  {
    SectionHeader noteSectionHeaderA = makeNoteSectionHeader(".note.A");
    noteSectionHeaderA.offset = 256;

    sectionHeaderTable.emplace_back( makeNullSectionHeader() );
    sectionHeaderTable.emplace_back(noteSectionHeaderA);

    noteSectionHeaderA.offset = 584;
    noteSectionTable.addSectionFromFile( noteSectionHeaderA, makeNoteSection("A") );

    noteSectionTable.updateSectionHeaderTable(sectionHeaderTable);

    REQUIRE( sectionHeaderTable.size() == 2 );
    REQUIRE( sectionHeaderTable[0].sectionType() == SectionType::Null );
    REQUIRE( sectionHeaderTable[1].offset == 584 );
  }
}

TEST_CASE("findMinimumSizeToWriteTable")
{
  NoteSectionTable noteSectionTable;

  SECTION("empty table")
  {
    REQUIRE( noteSectionTable.findMinimumSizeToWriteTable() == 0 );
  }

  SECTION("1 note section")
  {
    SectionHeader headerA = makeNoteSectionHeader(".note.A");
    headerA.offset = 25;
    headerA.size = 10;

    noteSectionTable.addSectionFromFile( headerA, makeNoteSection("A") );

    REQUIRE( noteSectionTable.findMinimumSizeToWriteTable() == 35 );
  }

  SECTION("2 note sections")
  {
    SectionHeader headerA = makeNoteSectionHeader(".note.A");
    headerA.offset = 25;
    headerA.size = 10;

    SectionHeader headerB = makeNoteSectionHeader(".note.B");
    headerA.offset = 50;
    headerA.size = 15;

    noteSectionTable.addSectionFromFile( headerA, makeNoteSection("A") );
    noteSectionTable.addSectionFromFile( headerB, makeNoteSection("B") );

    REQUIRE( noteSectionTable.findMinimumSizeToWriteTable() == 65 );
  }
}
