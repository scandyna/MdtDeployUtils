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
#include "Mdt/DeployUtils/Impl/Elf/DynamicSection.h"
#include <QLatin1String>

#include <QDebug>


TEST_CASE("isNull")
{
  DynamicSection section;

  SECTION("default constructed")
  {
    REQUIRE( section.isNull() );
  }

  SECTION("add a entry")
  {
    section.addEntry( makeNeededEntry() );
    REQUIRE( !section.isNull() );
  }
}

TEST_CASE("add_entry_clear")
{
  DynamicSection section;

  section.addEntry( makeNeededEntry() );
  REQUIRE( section.entriesCount() == 1 );

  section.clear();
  REQUIRE( section.entriesCount() == 0 );
  REQUIRE( section.isNull() );
}

TEST_CASE("getSoName")
{
  DynamicSection section;
  section.addEntry( makeNullEntry() );

  const uchar stringTable[8] = {'\0','S','o','N','a','m','e','\0'};
  section.setStringTable( stringTableFromCharArray( stringTable, sizeof(stringTable) ) );

  SECTION("no DT_SONAME present")
  {
    REQUIRE( section.getSoName().isEmpty() );
  }

  SECTION("SoName")
  {
    section.addEntry( makeSoNameEntry(1) );
    REQUIRE( section.getSoName() == QLatin1String("SoName") );
  }
}

TEST_CASE("getNeededSharedLibraries")
{
  DynamicSection section;
  section.addEntry( makeNullEntry() );

  const uchar stringTable[17] = {
    '\0',
    'l','i','b','A','.','s','o','\0',
    'l','i','b','B','.','s','o','\0'
  };
  section.setStringTable( stringTableFromCharArray( stringTable, sizeof(stringTable) ) );

  SECTION("no DT_NEEDED present")
  {
    REQUIRE( section.getNeededSharedLibraries().isEmpty() );
  }

  SECTION("libA.so")
  {
    section.addEntry( makeNeededEntry(1) );
    REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
  }

  SECTION("libA.so libB.so")
  {
    section.addEntry( makeNeededEntry(1) );
    section.addEntry( makeNeededEntry(9) );
    REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so","libB.so"}) );
  }
}

TEST_CASE("getRunPath")
{
  DynamicSection section;
  section.addEntry( makeNullEntry() );

  const uchar stringTable[13] = {
    '\0',
    '/','t','m','p',':',
    '/','p','a','t','h','2','\0'
  };
  section.setStringTable( stringTableFromCharArray( stringTable, sizeof(stringTable) ) );

  SECTION("no DT_RUNPATH present")
  {
    REQUIRE( section.getRunPath().isEmpty() );
  }

  SECTION("/tmp:/path2")
  {
    section.addEntry( makeRunPathEntry(1) );
    REQUIRE( section.getRunPath() == QLatin1String("/tmp:/path2") );
  }
}
