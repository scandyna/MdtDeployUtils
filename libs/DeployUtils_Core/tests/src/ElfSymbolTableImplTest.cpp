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
#include "ElfSymbolTableTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/SymbolTable.h"

using namespace Mdt::DeployUtils::Impl::Elf;

/// \todo This is a copy paste from ElfFileAllHeadersImplTest
SectionHeader makeNullSectionHeader()
{
  SectionHeader header;
  header.type = 0;
  header.offset = 0;
  header.size = 0;

  return header;
}

/// \todo This is a copy paste from ElfFileAllHeadersImplTest
SectionHeader makeDynamicSectionHeader()
{
  SectionHeader header;
  header.type = 6;

  return header;
}

/// \todo This is a copy paste from ElfFileAllHeadersImplTest
SectionHeader makeStringTableSectionHeader()
{
  SectionHeader header;
  header.type = 3;

  return header;
}


TEST_CASE("symbolType")
{
  SymbolTableEntry entry;

  SECTION("Object")
  {
    entry.info = 1;
    REQUIRE( entry.symbolType() == SymbolType::Object );
  }

  SECTION("Object (info also contains binding)")
  {
    entry.info = 0x21;
    REQUIRE( entry.symbolType() == SymbolType::Object );
  }

  SECTION("Section")
  {
    entry.info = 3;
    REQUIRE( entry.symbolType() == SymbolType::Section );
  }
}

TEST_CASE("indexAssociationsKnownSections")
{
  PartialSymbolTableEntry entry;
  PartialSymbolTable table;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("default constructed table has no known associations")
  {
    REQUIRE( !table.containsDynamicSectionAssociation() );
    REQUIRE( !table.containsDynamicStringTableAssociation() );
  }

  SECTION("table contains the association to the dynamic section")
  {
    /*
     * | SymTab index | Associated header index | section  |
     * |       0      |          1              | .dynamic |
     */
    SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(dynamicSectionHeader);

    entry.entry.info = 3;
    entry.entry.shndx = 1;
    table.addEntryFromFile(entry);
    table.indexAssociationsKnownSections(sectionHeaderTable);

    REQUIRE( table.containsDynamicSectionAssociation() );
    REQUIRE( !table.containsDynamicStringTableAssociation() );
    REQUIRE( table.entryAt(0).shndx == 1 );
  }

  SECTION("table contains the association to the dynamic string table (that implies the .dynamic section)")
  {
    /*
     * | SymTab index | Associated header index | section  |
     * |       0      |          1              | .dynamic |
     * |       1      |          2              | .dynamic |
     */
    SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
    dynamicSectionHeader.link = 2;
    SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(dynamicSectionHeader);
    sectionHeaderTable.push_back(dynamicStringTableSectionHeader);

    entry.entry.info = 3;
    entry.entry.shndx = 1;
    table.addEntryFromFile(entry);
    entry.entry.shndx = 2;
    table.addEntryFromFile(entry);
    table.indexAssociationsKnownSections(sectionHeaderTable);

    REQUIRE( table.containsDynamicSectionAssociation() );
    REQUIRE( table.containsDynamicStringTableAssociation() );
    REQUIRE( table.entryAt(0).shndx == 1 );
    REQUIRE( table.entryAt(1).shndx == 2 );
  }
}

TEST_CASE("findMinimumSizeToAccessEntries")
{
  PartialSymbolTableEntry entry;
  PartialSymbolTable table;
  Class c;

  SECTION("64-bit")
  {
    c = Class::Class64;

    SECTION("offsets 100,10")
    {
      entry.fileOffset = 100;
      table.addEntryFromFile(entry);
      entry.fileOffset = 10;
      table.addEntryFromFile(entry);

      REQUIRE( table.findMinimumSizeToAccessEntries(c) == 124 );
    }
  }
}
