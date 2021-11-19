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
#include "ElfFileIoTestUtils.h"
#include "Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.h"

using namespace Mdt::DeployUtils::Impl;

TEST_CASE("stringFromBoundedUnsignedCharArray")
{
  ByteArraySpan span;

  SECTION("span is larger that string")
  {
    SECTION("A")
    {
      unsigned char array[2] = {'A','\0'};
      span = arraySpanFromArray( array, sizeof(array) );

      REQUIRE( stringFromBoundedUnsignedCharArray(span) == "A" );
    }
  }

  SECTION("span is just large enought")
  {
    SECTION("A (string not null terminated)")
    {
      unsigned char array[1] = {'A'};
      span = arraySpanFromArray( array, sizeof(array) );

      REQUIRE( stringFromBoundedUnsignedCharArray(span) == "A" );
    }

    SECTION("A (string is null terminated)")
    {
      unsigned char array[2] = {'A','\0'};
      span = arraySpanFromArray( array, sizeof(array) );

      REQUIRE( stringFromBoundedUnsignedCharArray(span) == "A" );
    }
  }
}

TEST_CASE("containsEndOfString")
{
  ByteArraySpan span;

  SECTION("empty")
  {
    unsigned char array[1] = {};
    span.data = array;

    SECTION("size: 0")
    {
      span.size = 0;
      REQUIRE( !containsEndOfString(span) );
    }
  }

  SECTION("ABC")
  {
    unsigned char array[4] = {'A','B','C','\0'};
    span.data = array;

    SECTION("size: 3")
    {
      span.size = 3;
      REQUIRE( !containsEndOfString(span) );
    }

    SECTION("size: 4")
    {
      span.size = 4;
      REQUIRE( containsEndOfString(span) );
    }
  }
}

TEST_CASE("qStringFromUft8UnsignedCharArray")
{
  ByteArraySpan span;

  SECTION("A")
  {
    unsigned char array[2] = {'A','\0'};
    span.data = array;
    span.size = 2;
    REQUIRE( qStringFromUft8UnsignedCharArray(span) == QLatin1String("A") );
  }
}
