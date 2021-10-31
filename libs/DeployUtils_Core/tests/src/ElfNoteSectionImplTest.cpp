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
#include "Mdt/DeployUtils/Impl/Elf/NoteSection.h"

using namespace Mdt::DeployUtils::Impl::Elf;

TEST_CASE("byteCountAligned")
{
  NoteSection section;

  SECTION("default constructed")
  {
    REQUIRE( section.isNull() );
  }

  SECTION("note contains a name and type")
  {
    /*
     * name size: 4 bytes
     * description size: 4 bytes
     * type: 4 bytes
     * name: N,a,m,e,\0 -> 5 bytes -> 8 bytes
     * description: 0 byte
     */
    section.descriptionSize = 0;
    section.type = 1;
    section.name = "Name";

    REQUIRE( section.byteCountAligned() == 20 );
  }

  SECTION("GNU .note.ABI-tag")
  {
    /*
     * name size: 4 bytes
     * description size: 4 bytes
     * type: 4 bytes
     * name: G,N,U,\0 -> 4 bytes
     * description: 16 (0x10) bytes
     */
    section.descriptionSize = 0;
    section.type = 1;
    section.name = "GNU";
    section.description = {1,2,3,4};

    REQUIRE( section.byteCountAligned() == 32 );
  }
}
