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
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"

using namespace Mdt::DeployUtils;
using Impl::ByteArraySpan;


ByteArraySpan spanFromArray(const unsigned char * const data, int64_t size)
{
  ByteArraySpan span;

  span.data = data;
  span.size = size;

  return span;
}

TEST_CASE("Construct")
{
  SECTION("default constructed")
  {
    ByteArraySpan span;
    REQUIRE( span.isNull() );
  }
}

TEST_CASE("isInRange")
{
  unsigned char array[4] = {1,2,3,4};
  const ByteArraySpan span = spanFromArray( array, sizeof(array) );

  SECTION("offset 0, size 1")
  {
    REQUIRE( span.isInRange(0, 1) );
  }

  SECTION("offset 1, size 3")
  {
    REQUIRE( span.isInRange(1, 3) );
  }

  SECTION("offset 1, size 4")
  {
    REQUIRE( !span.isInRange(1, 4) );
  }
}

TEST_CASE("subSpan")
{
  unsigned char array[4] = {1,2,3,4};
  const ByteArraySpan span = spanFromArray( array, sizeof(array) );

  SECTION("offset 0, size 1")
  {
    const ByteArraySpan subSpan = span.subSpan(0, 1);
    REQUIRE( subSpan.size == 1 );
    REQUIRE( subSpan.data[0] == 1 );
  }

  SECTION("offset 1, size 3")
  {
    const ByteArraySpan subSpan = span.subSpan(1, 3);
    REQUIRE( subSpan.size == 3 );
    REQUIRE( subSpan.data[0] == 2 );
    REQUIRE( subSpan.data[1] == 3 );
    REQUIRE( subSpan.data[2] == 4 );
  }

  SECTION("get remaining from offset 1")
  {
    const ByteArraySpan subSpan = span.subSpan(1);
    REQUIRE( subSpan.size == 3 );
    REQUIRE( subSpan.data[0] == 2 );
    REQUIRE( subSpan.data[1] == 3 );
    REQUIRE( subSpan.data[2] == 4 );
  }
}
