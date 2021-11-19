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
#include "Mdt/DeployUtils/Impl/Elf/ProgramInterpreterSectionReader.h"
#include "Mdt/DeployUtils/Impl/Elf/ProgramInterpreterSectionWriter.h"

using namespace Mdt::DeployUtils::Impl::Elf;
using Mdt::DeployUtils::Impl::ByteArraySpan;


TEST_CASE("setProgramInterpreterSectionToArray")
{
  ProgramInterpreterSection section;
  ByteArraySpan array;
  ByteArraySpan expectedArray;

  SECTION("A")
  {
    section.path = "A";

    uchar expectedArrayData[2] = {'A','\0'};
    expectedArray = arraySpanFromArray( expectedArrayData, sizeof(expectedArrayData) );

    uchar arrayData[2];
    array = arraySpanFromArray( arrayData, sizeof(arrayData) );

    setProgramInterpreterSectionToArray(array, section);
    REQUIRE( arraysAreEqual(array, expectedArray) );
  }
}
