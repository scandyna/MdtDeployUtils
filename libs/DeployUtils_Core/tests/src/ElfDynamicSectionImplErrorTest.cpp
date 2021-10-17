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
#include "ElfDynamicSectionImplTestCommon.h"

using Mdt::DeployUtils::ExecutableFileReadError;

TEST_CASE("getSoName")
{
  DynamicSection section;
  section.addEntry( makeStringTableSizeEntry(1) );

  uchar stringTable[8] = {'\0','S','o','N','a','m','e','\0'};
  section.setStringTable( stringTableFromCharArray( stringTable, sizeof(stringTable) ) );

  SECTION("SoName string table index out of bound")
  {
    section.addEntry( makeSoNameEntry(8) );
    REQUIRE_THROWS_AS( section.getSoName(), ExecutableFileReadError );
  }
}

TEST_CASE("getNeededSharedLibraries")
{
  DynamicSection section;
  section.addEntry( makeStringTableSizeEntry(1) );

  uchar stringTable[17] = {
    '\0',
    'l','i','b','A','.','s','o','\0',
    'l','i','b','B','.','s','o','\0'
  };
  section.setStringTable( stringTableFromCharArray( stringTable, sizeof(stringTable) ) );

  SECTION("libA.so string table index out of bound")
  {
    section.addEntry( makeNeededEntry(35) );
    REQUIRE_THROWS_AS( section.getNeededSharedLibraries(), ExecutableFileReadError );
  }
}

TEST_CASE("getRunPath string table index out of bound")
{
  DynamicSection section;
  section.addEntry( makeStringTableSizeEntry(1) );

  uchar stringTable[13] = {
    '\0',
    '/','t','m','p',':',
    '/','p','a','t','h','2','\0'
  };
  section.setStringTable( stringTableFromCharArray( stringTable, sizeof(stringTable) ) );

  SECTION("/tmp:/path2")
  {
    section.addEntry( makeRunPathEntry(47) );
    REQUIRE_THROWS_AS( section.getRunPath(), ExecutableFileReadError );
  }
}
