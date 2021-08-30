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
#include "ElfFileIoImplTestCommon.h"
#include "Mdt/DeployUtils/Impl/Elf/StringTable.h"
#include <QLatin1String>
#include <vector>

// #include <QDebug>

using namespace Mdt::DeployUtils;
using Mdt::DeployUtils::Impl::Elf::StringTable;
using Mdt::DeployUtils::Impl::ByteArraySpan;


TEST_CASE("construct")
{
  StringTable table;

  SECTION("default constructed")
  {
    REQUIRE( table.isEmpty() );
    REQUIRE( table.byteCount() == 0 );
  }
}

TEST_CASE("fromCharArray")
{
  StringTable table;
  ByteArraySpan charArraySpan;

  SECTION("1 null char -> empty table")
  {
    const uchar charArray[1] = {0};
    charArraySpan = arraySpanFromArray( charArray, sizeof(charArray) );
    table = StringTable::fromCharArray(charArraySpan);
    REQUIRE( table.byteCount() == 1 );
    REQUIRE( table.isEmpty() );
  }

  SECTION("name.")
  {
    const uchar charArray[7] = {'\0','n','a','m','e','.','\0'};
    charArraySpan = arraySpanFromArray( charArray, sizeof(charArray) );
    table = StringTable::fromCharArray(charArraySpan);
    REQUIRE( table.byteCount() == 7 );
    REQUIRE( !table.isEmpty() );
  }
}

StringTable stringTableFromCharArray(const unsigned char * const array, qint64 size)
{
  return StringTable::fromCharArray( arraySpanFromArray(array, size) );
}

TEST_CASE("indexIsValid")
{
  StringTable table;

  SECTION("empty table")
  {
    const uchar charArray[1] = {0};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.indexIsValid(0) );
    REQUIRE( !table.indexIsValid(1) );
  }

  SECTION("name. A")
  {
    const uchar charArray[9] = {'\0','n','a','m','e','.','\0','A','\0'};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.indexIsValid(0) );
    REQUIRE( table.indexIsValid(1) );
    REQUIRE( table.indexIsValid(8) );
    REQUIRE( !table.indexIsValid(9) );
  }
}

TEST_CASE("stringAtIndex")
{
  StringTable table;

  SECTION("empty table")
  {
    const uchar charArray[1] = {0};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.stringAtIndex(0).empty() );
  }

  SECTION("name.")
  {
    const uchar charArray[7] = {'\0','n','a','m','e','.','\0'};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "name." );
    REQUIRE( table.stringAtIndex(2) == "ame." );
  }

  SECTION("name. A")
  {
    const uchar charArray[9] = {'\0','n','a','m','e','.','\0','A','\0'};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "name." );
    REQUIRE( table.stringAtIndex(7) == "A" );
  }
}

TEST_CASE("unicodeStringAtIndex")
{
  StringTable table;

  SECTION("empty table")
  {
    const uchar charArray[1] = {0};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.unicodeStringAtIndex(0).isEmpty() );
  }

  SECTION("name.")
  {
    const uchar charArray[7] = {'\0','n','a','m','e','.','\0'};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.unicodeStringAtIndex(0).isEmpty() );
    REQUIRE( table.unicodeStringAtIndex(1) == QLatin1String("name.") );
    REQUIRE( table.unicodeStringAtIndex(2) == QLatin1String("ame.") );
  }

  SECTION("name. A")
  {
    const uchar charArray[9] = {'\0','n','a','m','e','.','\0','A','\0'};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.unicodeStringAtIndex(0).isEmpty() );
    REQUIRE( table.unicodeStringAtIndex(1) == QLatin1String("name.") );
    REQUIRE( table.unicodeStringAtIndex(7) == QLatin1String("A") );
  }
}
