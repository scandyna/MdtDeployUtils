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
#include "Mdt/DeployUtils/Impl/Elf/NoteSectionReader.h"
#include "Mdt/DeployUtils/Impl/Elf/NoteSectionWriter.h"

using namespace Mdt::DeployUtils::Impl::Elf;
using Mdt::DeployUtils::Impl::ByteArraySpan;

TEST_CASE("noteSectionFromArray")
{
  NoteSection section;
  FileHeader fileHeader;
  ByteArraySpan array;

  SECTION("32-bit big-endian")
  {
    SECTION("name size is 0 (indicates a corrupted section)")
    {
      uchar arrayData[20] = {
        // name size
        0,0,0,0,  // 0
        // description size
        0,0,0,0,  // 0
        // type
        0x12,0x34,0x56,0x78,  // 0x12345678
        // name
        'N','a','m','e',  // Name
        '\0',0,0,0
      };
      fileHeader = make32BitBigEndianFileHeader();
      array = arraySpanFromArray( arrayData, sizeof(arrayData) );

      REQUIRE_THROWS_AS( section = NoteSectionReader::noteSectionFromArray(array, fileHeader.ident), NoteSectionReadError );
    }

    SECTION("name size is to large (indicates a corrupted section)")
    {
      uchar arrayData[20] = {
        // name size
        5,0,0,0,  // 0
        // description size
        0,0,0,0,  // 0
        // type
        0x12,0x34,0x56,0x78,  // 0x12345678
        // name
        'N','a','m','e',  // Name
        '\0',0,0,0
      };
      fileHeader = make32BitBigEndianFileHeader();
      array = arraySpanFromArray( arrayData, sizeof(arrayData) );

      REQUIRE_THROWS_AS( section = NoteSectionReader::noteSectionFromArray(array, fileHeader.ident), NoteSectionReadError );
    }

    SECTION("description size is to large (indicates a corrupted section)")
    {
      uchar arrayData[20] = {
        // name size
        0,0,0,5,
        // description size
        6,0,0,0,
        // type
        0x12,0x34,0x56,0x78,  // 0x12345678
        // name
        'N','a','m','e',  // Name
        '\0',0,0,0
      };
      fileHeader = make32BitBigEndianFileHeader();
      array = arraySpanFromArray( arrayData, sizeof(arrayData) );

      REQUIRE_THROWS_AS( section = NoteSectionReader::noteSectionFromArray(array, fileHeader.ident), NoteSectionReadError );
    }
  }
}
