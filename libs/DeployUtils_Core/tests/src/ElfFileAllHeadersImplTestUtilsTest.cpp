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

using namespace Mdt::DeployUtils::Impl::Elf;

TEST_CASE("makeTestHeaders")
{
  TestHeadersSetup setup;
  FileAllHeaders headers;

  SECTION(".interp")
  {
    setup.programInterpreterSectionOffset = 10;
    setup.programInterpreterSectionAddress = 100;
    setup.programInterpreterSectionSize = 15;

    headers = makeTestHeaders(setup);

    REQUIRE( headers.containsProgramInterpreterProgramHeader() );
    REQUIRE( headers.containsProgramInterpreterSectionHeader() );
    REQUIRE( headers.programInterpreterProgramHeader().vaddr == 100 );
    REQUIRE( headers.programInterpreterSectionHeader().addr == 100 );
  }

  SECTION("contains .dynamic")
  {
    setup.dynamicSectionOffset = 10;
    setup.dynamicSectionAddress = 100;
    setup.dynamicSectionSize = 50;

    headers = makeTestHeaders(setup);

    REQUIRE( headers.containsDynamicProgramHeader() );
    REQUIRE( headers.containsDynamicSectionHeader() );
    REQUIRE( !headers.containsDynamicStringTableSectionHeader() );
    REQUIRE( headers.dynamicProgramHeader().vaddr == 100 );
    REQUIRE( headers.dynamicSectionHeader().addr == 100 );
  }

  SECTION(".dynstr")
  {
    setup.dynamicStringTableOffset = 10;
    setup.dynamicStringTableAddress = 100;
    setup.dynamicStringTableSize = 15;

    headers = makeTestHeaders(setup);

    REQUIRE( headers.containsDynamicStringTableSectionHeader() );
    REQUIRE( headers.dynamicStringTableSectionHeader().addr == 100 );
  }

  SECTION("contains .dynamic and .dynstr")
  {
    setup.dynamicSectionOffset = 10;
    setup.dynamicSectionAddress = 100;
    setup.dynamicSectionSize = 50;
    setup.dynamicStringTableOffset = 60;
    setup.dynamicStringTableAddress = 150;
    setup.dynamicStringTableSize = 10;

    headers = makeTestHeaders(setup);

    REQUIRE( headers.containsDynamicProgramHeader() );
    REQUIRE( headers.containsDynamicSectionHeader() );
    REQUIRE( headers.containsDynamicStringTableSectionHeader() );
  }

  SECTION(".gnu.hash")
  {
    SECTION("when not set, no .gnu.hash")
    {
      headers = makeTestHeaders(setup);

      REQUIRE( !headers.containsGnuHashTableSectionHeader() );
    }

    SECTION("setup with .gnu.hash")
    {
      setup.gnuHashTableSectionOffset = 10;
      setup.gnuHashTableSectionAddress = 100;
      setup.gnuHashTableSectionSize = 15;

      headers = makeTestHeaders(setup);

      REQUIRE( headers.containsGnuHashTableSectionHeader() );
      REQUIRE( headers.gnuHashTableSectionHeader().addr == 100 );
    }
  }

  SECTION(".got.plt")
  {
    SECTION("when not set, no .got.plt")
    {
      headers = makeTestHeaders(setup);

      REQUIRE( !headers.containsGotPltSectionHeader() );
    }

    SECTION("setup with .got.plt")
    {
      setup.gotPltSectionOffset = 10;
      setup.gotPltSectionAddress = 100;
      setup.gotPltSectionSize = 15;

      headers = makeTestHeaders(setup);

      REQUIRE( headers.containsGotPltSectionHeader() );
      REQUIRE( headers.gotPltSectionHeader().addr == 100 );
    }
  }

  SECTION(".shstrtab (section name string table)")
  {
    SECTION("when not set, no .shstrtab")
    {
      headers = makeTestHeaders(setup);

      REQUIRE( !headers.containsSectionNameStringTableHeader() );
    }

    SECTION("setup with .shstrtab")
    {
      setup.sectionNameStringTableOffset = 25;

      headers = makeTestHeaders(setup);

      REQUIRE( headers.containsSectionNameStringTableHeader() );
      REQUIRE( headers.sectionNameStringTableHeader().offset == 25 );
      REQUIRE( headers.sectionNameStringTableHeader().addr == 0 );
    }
  }
}
