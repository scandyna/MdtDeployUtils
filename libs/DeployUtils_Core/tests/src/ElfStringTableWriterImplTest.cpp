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
#include "Mdt/DeployUtils/Impl/Elf/StringTableWriter.h"

using Mdt::DeployUtils::Impl::ByteArraySpan;
using Mdt::DeployUtils::Impl::Elf::FileHeader;
using Mdt::DeployUtils::Impl::Elf::Ident;
using Mdt::DeployUtils::Impl::Elf::StringTable;

TEST_CASE("stringTableToArray")
{
  using Mdt::DeployUtils::Impl::Elf::stringTableToArray;

  StringTable stringTable;
  ByteArraySpan array;
  ByteArraySpan expectedArray;

  uchar expectedArrayData[14] = {'\0','/','t','m','p','\0','l','i','b','A','.','s','o','\0'};
  expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

  stringTable = StringTable::fromCharArray(expectedArray);

  uchar arrayData[14] = {};
  array = arraySpanFromArray( arrayData, sizeof(arrayData) );

  stringTableToArray(array, stringTable);
  REQUIRE( arraysAreEqual(array, expectedArray) );
}
