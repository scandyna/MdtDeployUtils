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
    REQUIRE( table.byteCount() == 1 );
    REQUIRE( table.stringAtIndex(0) == "" );
  }
}

TEST_CASE("fromCharArray")
{
  StringTable table;
  ByteArraySpan charArraySpan;

  SECTION("1 null char -> empty table")
  {
    uchar charArray[1] = {0};
    charArraySpan = arraySpanFromArray( charArray, sizeof(charArray) );
    table = StringTable::fromCharArray(charArraySpan);
    REQUIRE( table.byteCount() == 1 );
    REQUIRE( table.isEmpty() );
  }

  SECTION("name.")
  {
    uchar charArray[7] = {'\0','n','a','m','e','.','\0'};
    charArraySpan = arraySpanFromArray( charArray, sizeof(charArray) );
    table = StringTable::fromCharArray(charArraySpan);
    REQUIRE( table.byteCount() == 7 );
    REQUIRE( !table.isEmpty() );
  }
}

StringTable stringTableFromCharArray(unsigned char * const array, qint64 size)
{
  return StringTable::fromCharArray( arraySpanFromArray(array, size) );
}

TEST_CASE("clear")
{
  uchar charArray[9] = {'\0','n','a','m','e','.','\0','A','\0'};
  StringTable table = stringTableFromCharArray( charArray, sizeof(charArray) );
  REQUIRE( table.byteCount() == 9 );

  table.clear();
  REQUIRE( table.byteCount() == 1 );
  REQUIRE( table.isEmpty() );
  REQUIRE( table.stringAtIndex(0).empty() );
}

TEST_CASE("indexIsValid")
{
  StringTable table;

  SECTION("empty table")
  {
    uchar charArray[1] = {0};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.indexIsValid(0) );
    REQUIRE( !table.indexIsValid(1) );
  }

  SECTION("name. A")
  {
    uchar charArray[9] = {'\0','n','a','m','e','.','\0','A','\0'};
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
    uchar charArray[1] = {0};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.stringAtIndex(0).empty() );
  }

  SECTION("name.")
  {
    uchar charArray[7] = {'\0','n','a','m','e','.','\0'};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "name." );
    REQUIRE( table.stringAtIndex(2) == "ame." );
  }

  SECTION("name. A")
  {
    uchar charArray[9] = {'\0','n','a','m','e','.','\0','A','\0'};
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
    uchar charArray[1] = {0};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.unicodeStringAtIndex(0).isEmpty() );
  }

  SECTION("name.")
  {
    uchar charArray[7] = {'\0','n','a','m','e','.','\0'};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.unicodeStringAtIndex(0).isEmpty() );
    REQUIRE( table.unicodeStringAtIndex(1) == QLatin1String("name.") );
    REQUIRE( table.unicodeStringAtIndex(2) == QLatin1String("ame.") );
  }

  SECTION("name. A")
  {
    uchar charArray[9] = {'\0','n','a','m','e','.','\0','A','\0'};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );
    REQUIRE( table.unicodeStringAtIndex(0).isEmpty() );
    REQUIRE( table.unicodeStringAtIndex(1) == QLatin1String("name.") );
    REQUIRE( table.unicodeStringAtIndex(7) == QLatin1String("A") );
  }
}

TEST_CASE("appendString")
{
  StringTable table;
  uint64_t index;

  SECTION("add a string to a empty table")
  {
    index = table.appendString("libA.so");
    REQUIRE( index == 1 );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "libA.so" );
    REQUIRE( table.byteCount() == 1+7+1 );
  }

  SECTION("add a string to the end of a non empty table")
  {
    uchar charArray[6] = {
      '\0',
      '/','t','m','p','\0'
    };
    table = stringTableFromCharArray( charArray, sizeof(charArray) );

    index = table.appendString("libA.so");
    REQUIRE( index == 6 );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "/tmp" );
    REQUIRE( table.stringAtIndex(6) == "libA.so" );
    REQUIRE( table.byteCount() == 1+4+1+7+1 );
  }
}

TEST_CASE("removeStringAtIndex")
{
  StringTable table;
  int64_t offset;

  SECTION("remove the only string the table contains")
  {
    uchar charArray[6] = {
      '\0',
      '/','t','m','p','\0'
    };
    table = stringTableFromCharArray( charArray, sizeof(charArray) );

    offset = table.removeStringAtIndex(1);
    REQUIRE( offset == -5 );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.byteCount() == 1 );
  }

  SECTION("remove the second string")
  {
    uchar charArray[14] = {
      '\0',
      '/','t','m','p','\0',
      'l','i','b','A','.','s','o','\0'
    };
    table = stringTableFromCharArray( charArray, sizeof(charArray) );

    offset = table.removeStringAtIndex(6);
    REQUIRE( offset == -8 );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "/tmp" );
    REQUIRE( table.byteCount() == 1+4+1 );
  }

  SECTION("remove the first string")
  {
    uchar charArray[14] = {
      '\0',
      '/','t','m','p','\0',
      'l','i','b','A','.','s','o','\0'
    };
    table = stringTableFromCharArray( charArray, sizeof(charArray) );

    offset = table.removeStringAtIndex(1);
    REQUIRE( offset == -5 );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "libA.so" );
    REQUIRE( table.byteCount() == 1+7+1 );
  }
}

TEST_CASE("setStringAtIndex")
{
  StringTable table;
  int64_t offset;

  SECTION("add a string to a empty table")
  {
    uchar charArray[1] = {0};
    table = stringTableFromCharArray( charArray, sizeof(charArray) );

    offset = table.setStringAtIndex(1, "libA.so");
    REQUIRE( offset == 7 );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "libA.so" );
    REQUIRE( table.byteCount() == 1+7+1 );
  }

  SECTION("replace a string in a table with 1 string")
  {
    uchar charArray[6] = {
      '\0',
      'n','a','m','e','\0'
    };
    table = stringTableFromCharArray( charArray, sizeof(charArray) );

    SECTION("new string is shorter")
    {
      offset = table.setStringAtIndex(1, "abc");
      REQUIRE( offset == -1 );
      REQUIRE( table.stringAtIndex(0).empty() );
      REQUIRE( table.stringAtIndex(1) == "abc" );
      REQUIRE( table.byteCount() == 1+3+1 );
    }

    SECTION("new string is same length")
    {
      offset = table.setStringAtIndex(1, "abcd");
      REQUIRE( offset == 0 );
      REQUIRE( table.stringAtIndex(0).empty() );
      REQUIRE( table.stringAtIndex(1) == "abcd" );
      REQUIRE( table.byteCount() == 1+4+1 );
    }

    SECTION("new string is longer")
    {
      offset = table.setStringAtIndex(1, "abcde");
      REQUIRE( offset == 1 );
      REQUIRE( table.stringAtIndex(0).empty() );
      REQUIRE( table.stringAtIndex(1) == "abcde" );
      REQUIRE( table.byteCount() == 1+5+1 );
    }
  }

  SECTION("replace a string in a table with 2 strings")
  {
    uchar charArray[14] = {
      '\0',
      'n','a','m','e','\0',
      'l','i','b','A','.','s','o','\0'
    };
    table = stringTableFromCharArray( charArray, sizeof(charArray) );

    SECTION("replace first string")
    {
      SECTION("new string is shorter")
      {
        offset = table.setStringAtIndex(1, "abc");
        REQUIRE( offset == -1 );
        REQUIRE( table.stringAtIndex(0).empty() );
        REQUIRE( table.stringAtIndex(1) == "abc" );
        REQUIRE( table.stringAtIndex(6-1) == "libA.so" );
        REQUIRE( table.byteCount() == 1+3+1+7+1 );
      }

      SECTION("new string is same length")
      {
        offset = table.setStringAtIndex(1, "abcd");
        REQUIRE( offset == 0 );
        REQUIRE( table.stringAtIndex(0).empty() );
        REQUIRE( table.stringAtIndex(1) == "abcd" );
        REQUIRE( table.stringAtIndex(6) == "libA.so" );
        REQUIRE( table.byteCount() == 1+4+1+7+1 );
      }

      SECTION("new string is longer")
      {
        offset = table.setStringAtIndex(1, "abcde");
        REQUIRE( offset == 1 );
        REQUIRE( table.stringAtIndex(0).empty() );
        REQUIRE( table.stringAtIndex(1) == "abcde" );
        REQUIRE( table.stringAtIndex(6+1) == "libA.so" );
        REQUIRE( table.byteCount() == 1+5+1+7+1 );
      }
    }

    SECTION("replace second string")
    {
      SECTION("new string is shorter")
      {
        offset = table.setStringAtIndex(6, "abc");
        REQUIRE( offset == -4 );
        REQUIRE( table.stringAtIndex(0).empty() );
        REQUIRE( table.stringAtIndex(1) == "name" );
        REQUIRE( table.stringAtIndex(6) == "abc" );
        REQUIRE( table.byteCount() == 1+4+1+3+1 );
      }

      SECTION("new string is same length")
      {
        offset = table.setStringAtIndex(6, "abcdefg");
        REQUIRE( offset == 0 );
        REQUIRE( table.stringAtIndex(0).empty() );
        REQUIRE( table.stringAtIndex(1) == "name" );
        REQUIRE( table.stringAtIndex(6) == "abcdefg" );
        REQUIRE( table.byteCount() == 1+4+1+7+1 );
      }

      SECTION("new string is longer")
      {
        offset = table.setStringAtIndex(6, "abcdefgh");
        REQUIRE( offset == 1 );
        REQUIRE( table.stringAtIndex(0).empty() );
        REQUIRE( table.stringAtIndex(1) == "name" );
        REQUIRE( table.stringAtIndex(6) == "abcdefgh" );
        REQUIRE( table.byteCount() == 1+4+1+8+1 );
      }
    }
  }
}

/*
 * Current implementation is a call to appendString(),
 * we don't have to do all possible tests here
 */
TEST_CASE("appendUnicodeString")
{
  StringTable table;
  uint64_t index;

  SECTION("add a string to a empty table")
  {
    index = table.appendUnicodeString( QLatin1String("libA.so") );
    REQUIRE( index == 1 );
    REQUIRE( table.stringAtIndex(0).empty() );
    REQUIRE( table.stringAtIndex(1) == "libA.so" );
    REQUIRE( table.byteCount() == 1+7+1 );
  }
}

/*
 * Current implementation is a call to setStringAtIndex(),
 * we don't have to do all possible tests here
 */
TEST_CASE("setUnicodeStringAtIndex")
{
  StringTable table;
  int64_t offset;

  SECTION("replace the first string in a table with 2 strings")
  {
    uchar charArray[14] = {
      '\0',
      'n','a','m','e','\0',
      'l','i','b','A','.','s','o','\0'
    };
    table = stringTableFromCharArray( charArray, sizeof(charArray) );

    SECTION("new string is shorter")
    {
      offset = table.setUnicodeStringAtIndex( 1, QLatin1String("abc") );
      REQUIRE( offset == -1 );
      REQUIRE( table.stringAtIndex(0).empty() );
      REQUIRE( table.stringAtIndex(1) == "abc" );
      REQUIRE( table.stringAtIndex(6-1) == "libA.so" );
      REQUIRE( table.byteCount() == 1+3+1+7+1 );
    }

    SECTION("new string is same length")
    {
      offset = table.setUnicodeStringAtIndex( 1, QLatin1String("abcd") );
      REQUIRE( offset == 0 );
      REQUIRE( table.stringAtIndex(0).empty() );
      REQUIRE( table.stringAtIndex(1) == "abcd" );
      REQUIRE( table.stringAtIndex(6) == "libA.so" );
      REQUIRE( table.byteCount() == 1+4+1+7+1 );
    }

    SECTION("new string is longer")
    {
      offset = table.setUnicodeStringAtIndex( 1, QLatin1String("abcde") );
      REQUIRE( offset == 1 );
      REQUIRE( table.stringAtIndex(0).empty() );
      REQUIRE( table.stringAtIndex(1) == "abcde" );
      REQUIRE( table.stringAtIndex(6+1) == "libA.so" );
      REQUIRE( table.byteCount() == 1+5+1+7+1 );
    }
  }
}
