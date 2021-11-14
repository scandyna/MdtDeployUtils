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
}
