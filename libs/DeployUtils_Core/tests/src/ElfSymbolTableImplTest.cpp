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
#include "ElfSectionHeaderTestUtils.h"
#include "Mdt/DeployUtils/Impl/Elf/SymbolTable.h"

using namespace Mdt::DeployUtils::Impl::Elf;


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

TEST_CASE("updateSectionIndexes")
{
  PartialSymbolTable table;

  SECTION("empty symbol table")
  {
    SECTION("map: 0->0")
    {
      SectionIndexChangeMap map(1);

      table.updateSectionIndexes(map);
    }
  }

  SECTION("1 symbol -> section index 1")
  {
    PartialSymbolTableEntry entry = makeSectionAssociationSymbolTableEntryWithFileOffset(1);
    entry.entry.shndx = 1;

    table.addEntryFromFile(entry);

    SECTION("map: 0->0,1->1")
    {
      SectionIndexChangeMap map(2);

      table.updateSectionIndexes(map);

      REQUIRE( table.entryAt(0).shndx == 1 );
    }

    SECTION("map: 0->1,1->0")
    {
      SectionIndexChangeMap map(2);
      map.swapIndexes(0,1);

      table.updateSectionIndexes(map);

      REQUIRE( table.entryAt(0).shndx == 0 );
    }
  }

  SECTION("2 symbols: 0 -> section index 2 , 1 -> section index 0")
  {
    PartialSymbolTableEntry entry0 = makeSectionAssociationSymbolTableEntryWithFileOffset(1);
    entry0.entry.shndx = 2;

    PartialSymbolTableEntry entry1 = makeSectionAssociationSymbolTableEntryWithFileOffset(1);
    entry1.entry.shndx = 0;

    table.addEntryFromFile(entry0);
    table.addEntryFromFile(entry1);

    SECTION("map: 0->0,1->1,2->2 (no changes)")
    {
      SectionIndexChangeMap map(3);

      table.updateSectionIndexes(map);

      REQUIRE( table.entryAt(0).shndx == 2 );
      REQUIRE( table.entryAt(1).shndx == 0 );
    }

    SECTION("map: 0->2,1->1,2->0 (symbol 1, section index 0, must be ignored)")
    {
      SectionIndexChangeMap map(3);
      map.swapIndexes(0,2);

      table.updateSectionIndexes(map);

      REQUIRE( table.entryAt(0).shndx == 0 );
      REQUIRE( table.entryAt(1).shndx == 0 );
    }
  }

  SECTION("3 symbols: 0 -> section index 0, 1 -> section index 2, 2 -> section index 1")
  {
    PartialSymbolTableEntry entry0 = makeSectionAssociationSymbolTableEntryWithFileOffset(1);
    entry0.entry.shndx = 0;

    PartialSymbolTableEntry entry1 = makeSectionAssociationSymbolTableEntryWithFileOffset(1);
    entry1.entry.shndx = 2;

    PartialSymbolTableEntry entry2 = makeSectionAssociationSymbolTableEntryWithFileOffset(1);
    entry2.entry.shndx = 1;

    table.addEntryFromFile(entry0);
    table.addEntryFromFile(entry1);
    table.addEntryFromFile(entry2);

    SECTION("map: 0->0,1->2,2->1")
    {
      SectionIndexChangeMap map(3);
      map.swapIndexes(1,2);

      table.updateSectionIndexes(map);

      REQUIRE( table.entryAt(0).shndx == 0 );
      REQUIRE( table.entryAt(1).shndx == 1 );
      REQUIRE( table.entryAt(2).shndx == 2 );
    }
  }
}

TEST_CASE("updateVirtualAddresses")
{
  PartialSymbolTable symbolTable;
  std::vector<uint16_t> headerIndexes;
  std::vector<SectionHeader> sectionHeaderTable;

  SectionHeader nullSh = makeNullSectionHeader();
  nullSh.addr = 0;

  SectionHeader dynamicSh = makeDynamicSectionHeader();
  dynamicSh.addr = 1000;

  SectionHeader dynstrSh = makeStringTableSectionHeader(".dynstr");
  dynstrSh.addr = 2000;

  sectionHeaderTable.push_back(nullSh);
  sectionHeaderTable.push_back(dynamicSh);
  sectionHeaderTable.push_back(dynstrSh);

  PartialSymbolTableEntry entry1 = makeSectionAssociationSymbolTableEntryWithFileOffset(100);
  entry1.entry.value = 1000;
  entry1.entry.shndx = 1;

  PartialSymbolTableEntry entry2 = makeSectionAssociationSymbolTableEntryWithFileOffset(200);
  entry2.entry.value = 2000;
  entry2.entry.shndx = 2;

  symbolTable.addEntryFromFile(entry1);
  symbolTable.addEntryFromFile(entry2);

  SECTION("no section to update")
  {
    symbolTable.updateVirtualAddresses(headerIndexes, sectionHeaderTable);

    REQUIRE( symbolTable.entryAt(0).value == 1000 );
    REQUIRE( symbolTable.entryAt(1).value == 2000 );
  }

  SECTION("change .dynamic address")
  {
    sectionHeaderTable[1].addr = 1500;
    headerIndexes.push_back(1);

    symbolTable.updateVirtualAddresses(headerIndexes, sectionHeaderTable);

    REQUIRE( symbolTable.entryAt(0).value == 1500 );
    REQUIRE( symbolTable.entryAt(1).value == 2000 );
  }

  SECTION("change .dynamic and .dynstr address")
  {
    sectionHeaderTable[1].addr = 1500;
    sectionHeaderTable[2].addr = 2500;
    headerIndexes.push_back(1);
    headerIndexes.push_back(2);

    symbolTable.updateVirtualAddresses(headerIndexes, sectionHeaderTable);

    REQUIRE( symbolTable.entryAt(0).value == 1500 );
    REQUIRE( symbolTable.entryAt(1).value == 2500 );
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
     * | SymTab | symbol type | Section (header)  |
     * | index  |             |  index |   name   |
     * -------------------------------------------|
     * |   0    |  SECTION    |    1   | .dynamic |
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
    REQUIRE( !table.containsDynamicObject() );
    REQUIRE( table.entryAt(0).shndx == 1 );
  }

  SECTION("table contains the dynamic object")
  {
    /*
     * | SymTab | symbol type | Section (header)  |
     * | index  |             |  index |   name   |
     * -------------------------------------------|
     * |   0    |  OBJECT     |    1   | .dynamic |
     */
    SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(dynamicSectionHeader);

    entry.entry = makeObjectSymbolTableEntry();
    entry.entry.shndx = 1;
    table.addEntryFromFile(entry);
    table.indexAssociationsKnownSections(sectionHeaderTable);

    REQUIRE( !table.containsDynamicSectionAssociation() );
    REQUIRE( !table.containsDynamicStringTableAssociation() );
    REQUIRE( table.containsDynamicObject() );
  }

  SECTION("table contains the association to the dynamic string table (that implies the .dynamic section)")
  {
    /*
     * | SymTab | symbol type | Section (header)  |
     * | index  |             |  index |   name   |
     * -------------------------------------------|
     * |   0    |  SECTION    |    1   | .dynamic |
     * |   1    |  SECTION    |    2   | .dynstr  |
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
    REQUIRE( !table.containsDynamicObject() );
    REQUIRE( table.entryAt(0).shndx == 1 );
    REQUIRE( table.entryAt(1).shndx == 2 );
  }

}

TEST_CASE("setDynamicSectionVirtualAddress")
{
  PartialSymbolTableEntry entry;
  PartialSymbolTable table;
  std::vector<SectionHeader> sectionHeaderTable;

  SECTION("table does not contain any dynamic symbol")
  {
    table.setDynamicSectionVirtualAddress(25);
  }

  SECTION("table contains all known symbols")
  {
    /*
     * | SymTab | symbol type | Section (header)  |
     * | index  |             |  index |   name   |
     * -------------------------------------------|
     * |   0    |  SECTION    |    1   | .dynamic |
     * |   1    |  SECTION    |    2   | .dynstr  |
     * |   2    |  OBJECT     |    1   | .dynamic |
     */
    SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
    dynamicSectionHeader.link = 2;
    SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
    sectionHeaderTable.push_back( makeNullSectionHeader() );
    sectionHeaderTable.push_back(dynamicSectionHeader);
    sectionHeaderTable.push_back(dynamicStringTableSectionHeader);

    entry.entry = makeSectionAssociationSymbolTableEntry();
    entry.entry.shndx = 1;
    entry.entry.value = 0;
    table.addEntryFromFile(entry);

    entry.entry = makeSectionAssociationSymbolTableEntry();
    entry.entry.shndx = 2;
    entry.entry.value = 0;
    table.addEntryFromFile(entry);

    entry.entry = makeObjectSymbolTableEntry();
    entry.entry.shndx = 1;
    entry.entry.value = 0;
    table.addEntryFromFile(entry);

    table.indexAssociationsKnownSections(sectionHeaderTable);

    table.setDynamicSectionVirtualAddress(152);
    REQUIRE( table.entryAt(0).value == 152 );
    REQUIRE( table.entryAt(1).value == 0 );
    REQUIRE( table.entryAt(2).value == 152 );
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
