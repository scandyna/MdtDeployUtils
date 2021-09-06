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

using namespace Mdt::DeployUtils;
using Mdt::DeployUtils::Impl::Elf::StringTable;
using Mdt::DeployUtils::Impl::ByteArraySpan;
using Mdt::DeployUtils::Impl::StringTableError;

TEST_CASE("fromCharArray")
{
  StringTable table;
  ByteArraySpan charArraySpan;

  SECTION("1 not null char")
  {
    uchar charArray[1] = {1};
    charArraySpan = arraySpanFromArray( charArray, sizeof(charArray) );
    REQUIRE_THROWS_AS(StringTable::fromCharArray(charArraySpan), StringTableError);
  }

  SECTION("name. not null terminated")
  {
    uchar charArray[6] = {'\0','n','a','m','e','.'};
    charArraySpan = arraySpanFromArray( charArray, sizeof(charArray) );
    REQUIRE_THROWS_AS(StringTable::fromCharArray(charArraySpan), StringTableError);
  }

}
