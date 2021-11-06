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
#include "Mdt/DeployUtils/Impl/Elf/SectionHeaderTable.h"

using namespace Mdt::DeployUtils::Impl::Elf;

TEST_CASE("findIndexOfFirstSectionHeader")
{
  std::vector<SectionHeader> table;

  SECTION("empty table")
  {
    REQUIRE( findIndexOfFirstSectionHeader(table, SectionType::Dynamic, ".dynamic") == 0 );
  }

  SECTION("table contains .dynamic and .dynstr")
  {
    SectionHeader dynamicSectionHeader = makeDynamicSectionHeader();
    dynamicSectionHeader.name = ".dynamic";

    SectionHeader dynamicStringTableSectionHeader = makeStringTableSectionHeader();
    dynamicStringTableSectionHeader.name = ".dynstr";

    table.push_back( makeNullSectionHeader() );
    table.push_back(dynamicSectionHeader);
    table.push_back(dynamicStringTableSectionHeader);

    SECTION("find index of .dynamic")
    {
      REQUIRE( findIndexOfFirstSectionHeader(table, SectionType::Dynamic, ".dynamic") == 1 );
    }

    SECTION("find index of .dynstr")
    {
      REQUIRE( findIndexOfFirstSectionHeader(table, SectionType::StringTable, ".dynstr") == 2 );
    }

    SECTION("index of nn existing section header")
    {
      REQUIRE( findIndexOfFirstSectionHeader(table, SectionType::StringTable, ".unknown") == 0 );
    }
  }
}

TEST_CASE("sortSectionHeadersByFileOffset")
{
  std::vector<SectionHeader> headers;

  SECTION("empty collection")
  {
    sortSectionHeadersByFileOffset(headers);
  }

  SECTION("1 section")
  {
    SectionHeader dynStr = makeDynamicStringTableSectionHeader();
    dynStr.offset = 50;
    dynStr.link = 0;

    headers.push_back(dynStr);

    sortSectionHeadersByFileOffset(headers);

    REQUIRE( headers[0].offset == 50 );
  }

  SECTION(".dynamic , .dynstr")
  {
    SectionHeader dynStr = makeDynamicStringTableSectionHeader();
    dynStr.offset = 50;
    dynStr.link = 0;

    SectionHeader dynamic = makeDynamicSectionHeader();
    dynamic.offset = 100;

    headers.push_back( makeNullSectionHeader() );

    SECTION("headers are allready sorted")
    {
      dynamic.link = 1;

      headers.push_back(dynStr);
      headers.push_back(dynamic);

      sortSectionHeadersByFileOffset(headers);

      REQUIRE( headers[1].name == ".dynstr" );
      REQUIRE( headers[1].link == 0 );
      REQUIRE( headers[2].name == ".dynamic" );
      REQUIRE( headers[2].link == 1 );
    }

    SECTION("headers must be sorted")
    {
      dynamic.link = 2;

      headers.push_back(dynamic);
      headers.push_back(dynStr);

      sortSectionHeadersByFileOffset(headers);

      REQUIRE( headers[1].name == ".dynstr" );
      REQUIRE( headers[1].link == 0 );
      REQUIRE( headers[2].name == ".dynamic" );
      REQUIRE( headers[2].link == 1 );
    }
  }

  SECTION(".dynsym , .interp")
  {
    SectionHeader interp = makeProgramInterpreterSectionHeader();
    interp.offset = 50;
    interp.info = 0;

    SectionHeader dynSym = makeDynamicLinkerSymbolTableSectionHeader();
    dynSym.offset = 100;

    dynSym.info = 2;  // This is not a index to the section header table
    headers.push_back( makeNullSectionHeader() );
    headers.push_back(dynSym);
    headers.push_back(interp);

    sortSectionHeadersByFileOffset(headers);

    REQUIRE( headers[1].name == ".interp" );
    REQUIRE( headers[1].info == 0 );
    REQUIRE( headers[2].name == ".dynsym" );
    REQUIRE( headers[2].info == 2 );
  }

  SECTION(".symtab , .strtab")
  {
    SectionHeader symtab = makeSymbolTableSectionHeader();
    symtab.offset = 50;

    SectionHeader strtab = makeStringTableSectionHeader(".strtab");
    strtab.offset = 100;

    symtab.link = 1;
    symtab.info = 58; // This is not a index to the section header table
    headers.push_back( makeNullSectionHeader() );
    headers.push_back(strtab);
    headers.push_back(symtab);

    sortSectionHeadersByFileOffset(headers);

    REQUIRE( headers[1].name == ".symtab" );
    REQUIRE( headers[1].link == 2 );
    REQUIRE( headers[1].info == 58 );
    REQUIRE( headers[2].name == ".strtab" );
    REQUIRE( headers[2].link == 0 );
    REQUIRE( headers[2].info == 0 );
  }

  SECTION(".rela.plt , .got , .dynsym")
  {
    SectionHeader dynsym = makeDynamicLinkerSymbolTableSectionHeader();
    dynsym.offset = 50;

    SectionHeader relaPlt = makeRelocationWithAddendSectionHeader(".rela.plt");
    relaPlt.offset = 100;

    SectionHeader got = makeGotSectionHeader();
    got.offset = 150;

    relaPlt.link = 3; // links to .dynsym
    relaPlt.info = 1; // links to .got
    headers.push_back( makeNullSectionHeader() );
    headers.push_back(got);
    headers.push_back(relaPlt);
    headers.push_back(dynsym);

    sortSectionHeadersByFileOffset(headers);

    REQUIRE( headers[1].name == ".dynsym" );
    REQUIRE( headers[1].link == 0 );
    REQUIRE( headers[1].info == 0 );
    REQUIRE( headers[2].name == ".rela.plt" );
    REQUIRE( headers[2].link == 1 );
    REQUIRE( headers[2].info == 3 );
    REQUIRE( headers[3].name == ".got" );
    REQUIRE( headers[3].link == 0 );
    REQUIRE( headers[3].info == 0 );
  }
}

TEST_CASE("findCountOfSectionsToMoveToFreeSize")
{
  std::vector<SectionHeader> headers;

  SECTION("no section")
  {
    REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 20) == 1 );
  }

  SECTION("1 section of size 20")
  {
    SectionHeader headerA;
    headerA.offset = 50;
    headerA.size = 20;

    headers.push_back(headerA);

    SECTION("needed size: 10")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 10) == 1 );
    }

    SECTION("needed size: 20")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 20) == 1 );
    }

    SECTION("needed size: 30")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 30) == 2 );
    }
  }

  SECTION("2 sections no hole")
  {
    /*
     * size:      20    10
     * section: |  A  |  B  |
     * offset:   50    70    80
     */

    SectionHeader headerA;
    headerA.offset = 50;
    headerA.size = 20;

    SectionHeader headerB;
    headerB.offset = 70;
    headerB.size = 10;

    headers.push_back(headerA);
    headers.push_back(headerB);

    SECTION("needed size: 19")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 19) == 1 );
    }

    SECTION("needed size: 20")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 20) == 1 );
    }

    SECTION("needed size: 21")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 21) == 2 );
    }

    SECTION("needed size: 30")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 30) == 2 );
    }

    SECTION("needed size: 31")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 31) == 3 );
    }
  }

  SECTION("null section followed by a section (bug seen from real file)")
  {
    /*
     * size:      0   10
     * section: | A |  B  |
     * offset:   0   50    60
     */

    SectionHeader headerA;
    headerA.offset = 0;
    headerA.size = 0;

    SectionHeader headerB;
    headerB.offset = 50;
    headerB.size = 10;

    headers.push_back(headerA);
    headers.push_back(headerB);

    SECTION("needed size: 9")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 9) == 2 );
    }

    SECTION("needed size: 10")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 10) == 2 );
    }

    SECTION("needed size: 11")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 11) == 3 );
    }
  }

  SECTION("2 sections with a hole")
  {
    /*
     * size:      20    30    10
     * section: |  A  |     |  B  |
     * offset:   50    70    100   110
     */

    SectionHeader headerA;
    headerA.offset = 50;
    headerA.size = 20;

    SectionHeader headerB;
    headerB.offset = 100;
    headerB.size = 10;

    headers.push_back(headerA);
    headers.push_back(headerB);

    SECTION("needed size: 20 (is exactly the size of the first section)")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 20) == 1 );
    }

    SECTION("needed size: 25 (requires the space of the first section and space in the hole after it)")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 25) == 1 );
    }

    SECTION("needed size: 49")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 49) == 1 );
    }

    SECTION("needed size: 50 (the last required element is just before the start of the second section)")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 50) == 1 );
    }

    SECTION("needed size: 51 (the last required element is in the second section)")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 51) == 2 );
    }

    SECTION("needed size: 60 (the last required element is just before the end of the table)")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 60) == 2 );
    }

    SECTION("needed size: 61 (out of bound of the table)")
    {
      REQUIRE( findCountOfSectionsToMoveToFreeSize(headers, 61) == 3 );
    }
  }
}
