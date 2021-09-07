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
#include "Mdt/DeployUtils/Impl/Elf/FileWriter.h"
#include "Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.h"

// #include <QDebug>

using namespace Mdt::DeployUtils;
using Impl::ByteArraySpan;

bool arraysAreEqual(const ByteArraySpan & array, std::initializer_list<unsigned char> reference)
{
  return Impl::arraysAreEqual(array.data, array.size, reference);
}

TEST_CASE("setHalfWord")
{
  using Impl::Elf::setHalfWord;
  using Impl::Elf::DataFormat;

  uchar charArray[2] = {};
  ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

  SECTION("little-endian")
  {
    setHalfWord(array, 0x1234, DataFormat::Data2LSB);
    REQUIRE( arraysAreEqual(array, {0x34,0x12}) );
  }

  SECTION("big-endian")
  {
    setHalfWord(array, 0x1234, DataFormat::Data2MSB);
    REQUIRE( arraysAreEqual(array, {0x12,0x34}) );
  }
}

TEST_CASE("set32BitWord")
{
  using Impl::Elf::set32BitWord;
  using Impl::Elf::DataFormat;

  uchar charArray[4] = {};
  ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

  SECTION("little-endian")
  {
        set32BitWord(array, 0x12345678, DataFormat::Data2LSB);
    REQUIRE( arraysAreEqual(array, {0x78,0x56,0x34,0x12}) );
  }

  SECTION("big-endian")
  {
        set32BitWord(array, 0x12345678, DataFormat::Data2MSB);
    REQUIRE( arraysAreEqual(array, {0x12,0x34,0x56,0x78}) );
  }
}

TEST_CASE("set64BitWord")
{
  using Impl::Elf::set64BitWord;
  using Impl::Elf::DataFormat;

  uchar charArray[8] = {0};
  ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

  SECTION("little-endian")
  {
    set64BitWord(array, 0x123456789ABCDEF0, DataFormat::Data2LSB);
    REQUIRE( arraysAreEqual(array, {0xF0,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}) );
  }

  SECTION("big-endian")
  {
    set64BitWord(array, 0x123456789ABCDEF0, DataFormat::Data2MSB);
    REQUIRE( arraysAreEqual(array, {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0}) );
  }
}

TEST_CASE("setNWord")
{
  using Impl::Elf::setNWord;
  using Impl::Elf::DataFormat;
  using Impl::Elf::Ident;

  Ident ident;

  SECTION("32-bit little-endian")
  {
    ident = make32BitLittleEndianIdent();
    uchar charArray[4] = {};
    ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

    setNWord(array, 0x12345678, ident);
    REQUIRE( arraysAreEqual(array, {0x78,0x56,0x34,0x12}) );
  }

  SECTION("32-bit big-endian")
  {
    ident = make32BitBigEndianIdent();
    uchar charArray[4] = {};
    ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

    setNWord(array, 0x12345678, ident);
    REQUIRE( arraysAreEqual(array, {0x12,0x34,0x56,0x78}) );
  }

  SECTION("64-bit little-endian")
  {
    ident = make64BitLittleEndianIdent();
    uchar charArray[8] = {};
    ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

    setNWord(array, 0x123456789ABCDEF0, ident);
    REQUIRE( arraysAreEqual(array, {0xF0,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}) );
  }

  SECTION("64-bit big-endian")
  {
    ident = make64BitBigEndianIdent();
    uchar charArray[8] = {};
    ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

    setNWord(array, 0x123456789ABCDEF0, ident);
    REQUIRE( arraysAreEqual(array, {0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0}) );
  }
}

TEST_CASE("setAddress")
{
  using Impl::Elf::setAddress;
  using Impl::Elf::DataFormat;
  using Impl::Elf::Ident;

  Ident ident;

  SECTION("64-bit little-endian")
  {
    ident = make64BitLittleEndianIdent();
    uchar charArray[8] = {};
    ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

    setAddress(array, 0x123456789ABCDEF0, ident);
    REQUIRE( arraysAreEqual(array, {0xF0,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}) );
  }
}

TEST_CASE("setOffset")
{
  using Impl::Elf::setOffset;
  using Impl::Elf::DataFormat;
  using Impl::Elf::Ident;

  Ident ident;

  SECTION("64-bit little-endian")
  {
    ident = make64BitLittleEndianIdent();
    uchar charArray[8] = {};
    ByteArraySpan array = arraySpanFromArray( charArray, sizeof(charArray) );

    setOffset(array, 0x123456789ABCDEF0, ident);
    REQUIRE( arraysAreEqual(array, {0xF0,0xDE,0xBC,0x9A,0x78,0x56,0x34,0x12}) );
  }
}
