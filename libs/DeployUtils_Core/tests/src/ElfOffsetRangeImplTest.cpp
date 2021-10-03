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
#include "Mdt/DeployUtils/Impl/Elf/OffsetRange.h"

using Mdt::DeployUtils::Impl::Elf::OffsetRange;
using Mdt::DeployUtils::Impl::Elf::ProgramHeader;
using Mdt::DeployUtils::Impl::Elf::SectionHeader;

TEST_CASE("default constructed")
{
  OffsetRange range;

  REQUIRE( range.begin() == 0 );
  REQUIRE( range.end() == 0 );
  REQUIRE( range.byteCount() == 0 );
  REQUIRE( range.isEmpty() );
}

TEST_CASE("minimumSizeToAccessRange")
{
  OffsetRange range;

  SECTION("Null range")
  {
    range = OffsetRange::fromBeginAndEndOffsets(0, 0);

    REQUIRE( range.minimumSizeToAccessRange() == 0 );
  }

  SECTION("range 10-11")
  {
    range = OffsetRange::fromBeginAndEndOffsets(10, 12);

    REQUIRE( range.lastOffset() == 11 );
    REQUIRE( range.minimumSizeToAccessRange() == 12 );
  }
}

TEST_CASE("fromBeginAndEndOffsets")
{
  OffsetRange range;

  SECTION("Null range")
  {
    range = OffsetRange::fromBeginAndEndOffsets(0, 0);

    REQUIRE( range.isEmpty() );
  }

  SECTION("1 byte range")
  {
    range = OffsetRange::fromBeginAndEndOffsets(10, 11);

    REQUIRE( range.begin() == 10 );
    REQUIRE( range.byteCount() == 1 );
    REQUIRE( !range.isEmpty() );
    REQUIRE( range.lastOffset() == 10 );
    REQUIRE( range.end() == 11 );
  }
}

TEST_CASE("fromProgrameHeader")
{
  OffsetRange range;
  ProgramHeader header;

  SECTION("null segment")
  {
    header.offset = 0;
    header.filesz = 0;

    range = OffsetRange::fromProgrameHeader(header);

    REQUIRE( range.isEmpty() );
  }

  SECTION("1 byte segment")
  {
    header.offset = 10;
    header.filesz = 1;

    range = OffsetRange::fromProgrameHeader(header);

    REQUIRE( range.begin() == 10 );
    REQUIRE( range.byteCount() == 1 );
    REQUIRE( !range.isEmpty() );
    REQUIRE( range.lastOffset() == 10 );
    REQUIRE( range.end() == 11 );
  }
}

TEST_CASE("fromSectionHeader")
{
  OffsetRange range;
  SectionHeader header;

  SECTION("1 byte section")
  {
    header.offset = 10;
    header.size = 1;

    range = OffsetRange::fromSectionHeader(header);

    REQUIRE( range.begin() == 10 );
    REQUIRE( range.byteCount() == 1 );
    REQUIRE( !range.isEmpty() );
    REQUIRE( range.lastOffset() == 10 );
    REQUIRE( range.end() == 11 );
  }
}
