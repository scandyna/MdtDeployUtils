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

bool sectionContainsRunPathEntry(const DynamicSection & section)
{
  for(DynamicStruct s : section){
    if( s.tagType() == DynamicSectionTagType::Runpath ){
      return true;
    }
  }

  return false;
}


TEST_CASE("DynamicStruct")
{
  SECTION("default constructed")
  {
    DynamicStruct ds;
    REQUIRE( ds.tagType() == DynamicSectionTagType::Null );
  }

  SECTION("construct with a tag")
  {
    DynamicStruct ds(DynamicSectionTagType::Runpath);
    REQUIRE( ds.tagType() == DynamicSectionTagType::Runpath );
  }
}

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
  const uchar stringTable[8] = {'\0','S','o','N','a','m','e','\0'};
  section.setStringTable( stringTableFromCharArray( stringTable, sizeof(stringTable) ) );
  REQUIRE( section.stringTable().byteCount() == 8 );

  section.addEntry( makeSoNameEntry(1) );
  REQUIRE( section.entriesCount() == 1 );

  section.clear();
  REQUIRE( section.entriesCount() == 0 );
  REQUIRE( section.stringTable().byteCount() == 1 );
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

TEST_CASE("removeRunPath")
{
  DynamicSection section;
  section.addEntry( makeNullEntry() );

  SECTION("section does not have the DT_RUNPATH")
  {
    section.removeRunPath();
    REQUIRE( !sectionContainsRunPathEntry(section) );
    REQUIRE( section.stringTable().isEmpty() );
  }

  SECTION("section contains the DT_RUNPATH")
  {
    const uchar initialStringTable[6] = {
      '\0',
      '/','t','m','p','\0'
    };
    section.setStringTable( stringTableFromCharArray( initialStringTable, sizeof(initialStringTable) ) );
    section.addEntry( makeRunPathEntry(1) );
    REQUIRE( sectionContainsRunPathEntry(section) );
    REQUIRE( section.getRunPath() == QLatin1String("/tmp") );

    section.removeRunPath();
    REQUIRE( !sectionContainsRunPathEntry(section) );
    REQUIRE( section.stringTable().isEmpty() );
  }

  SECTION("section contains the DT_RUNPATH and a DT_NEEDED")
  {
    const uchar initialStringTable[14] = {
      '\0',
      '/','t','m','p','\0',
      'l','i','b','A','.','s','o','\0'
    };
    section.setStringTable( stringTableFromCharArray( initialStringTable, sizeof(initialStringTable) ) );
    section.addEntry( makeRunPathEntry(1) );
    section.addEntry( makeNeededEntry(6) );
    REQUIRE( sectionContainsRunPathEntry(section) );
    REQUIRE( section.getRunPath() == QLatin1String("/tmp") );
    REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );

    section.removeRunPath();
    REQUIRE( !sectionContainsRunPathEntry(section) );
    REQUIRE( section.stringTable().byteCount() == 1+7+1 );
    REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
  }
}

TEST_CASE("setRunPath")
{
  DynamicSection section;
  section.addEntry( makeNullEntry() );

  SECTION("there is initially no entry in the section (string table is also empty)")
  {
    section.setRunPath( QLatin1String("/path1:/path2") );
    REQUIRE( section.getRunPath() == QLatin1String("/path1:/path2") );
  }

  SECTION("there is initially only a DT_NEEDED entry")
  {
    const uchar initialStringTable[9] = {
      '\0',
      'l','i','b','A','.','s','o','\0'
    };
    section.setStringTable( stringTableFromCharArray( initialStringTable, sizeof(initialStringTable) ) );
    section.addEntry( makeNeededEntry(1) );
    REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
    REQUIRE( section.getRunPath().isEmpty() );

    section.setRunPath( QLatin1String("/path1:/path2") );
    REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
    REQUIRE( section.getRunPath() == QLatin1String("/path1:/path2") );
  }

  SECTION("there is initially only the DT_RUNPATH entry (string table contains only a DT_RUNPATH string)")
  {
    const uchar initialStringTable[6] = {
      '\0',
      '/','t','m','p','\0'
    };
    section.setStringTable( stringTableFromCharArray( initialStringTable, sizeof(initialStringTable) ) );
    section.addEntry( makeRunPathEntry(1) );
    REQUIRE( section.getRunPath() == QLatin1String("/tmp") );

    SECTION("set a RunPath")
    {
      section.setRunPath( QLatin1String("/path1:/path2") );
      REQUIRE( section.getRunPath() == QLatin1String("/path1:/path2") );
    }

    SECTION("set a empty RunPath")
    {
      REQUIRE( sectionContainsRunPathEntry(section) );
      section.setRunPath( QString() );
      REQUIRE( !sectionContainsRunPathEntry(section) );
      REQUIRE( section.stringTable().isEmpty() );
      REQUIRE( section.getRunPath().isEmpty() );
    }
  }

  SECTION("there is initially a DT_NEEDED entry and the DT_RUNPATH (string table contains related strings)")
  {
    const uchar initialStringTable[14] = {
      '\0',
      'l','i','b','A','.','s','o','\0',
      '/','t','m','p','\0'
    };
    section.setStringTable( stringTableFromCharArray( initialStringTable, sizeof(initialStringTable) ) );

    SECTION("add DT_NEEDED then DT_RUNPATH (indexes: 1,9 -> ascending)")
    {
      section.addEntry( makeNeededEntry(1) );
      section.addEntry( makeRunPathEntry(9) );
      REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
      REQUIRE( section.getRunPath() == QLatin1String("/tmp") );

      section.setRunPath( QLatin1String("/path1:/path2") );
      REQUIRE( section.getRunPath() == QLatin1String("/path1:/path2") );
      REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
    }

    SECTION("add DT_RUNPATH then DT_NEEDED (indexes: 9,1)")
    {
      section.addEntry( makeRunPathEntry(9) );
      section.addEntry( makeNeededEntry(1) );
      REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
      REQUIRE( section.getRunPath() == QLatin1String("/tmp") );

      section.setRunPath( QLatin1String("/path1:/path2") );
      REQUIRE( section.getRunPath() == QLatin1String("/path1:/path2") );
      REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
    }
  }

  SECTION("there is initially the DT_RUNPATH entry, then a DT_NEEDED entry (string table contains related strings)")
  {
    const uchar initialStringTable[14] = {
      '\0',
      '/','t','m','p','\0',
      'l','i','b','A','.','s','o','\0'
    };
    section.setStringTable( stringTableFromCharArray( initialStringTable, sizeof(initialStringTable) ) );

    SECTION("add DT_RUNPATH then DT_NEEDED (indexes: 1,6 -> ascending)")
    {
      section.addEntry( makeRunPathEntry(1) );
      section.addEntry( makeNeededEntry(6) );
      REQUIRE( section.getRunPath() == QLatin1String("/tmp") );
      REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );

      SECTION("set a shorter RunPath")
      {
        section.setRunPath( QLatin1String("/a") );
        REQUIRE( section.getRunPath() == QLatin1String("/a") );
        REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
      }

      SECTION("set a longer RunPath")
      {
        section.setRunPath( QLatin1String("/path1:/path2") );
        REQUIRE( section.getRunPath() == QLatin1String("/path1:/path2") );
        REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
      }
    }

    SECTION("add DT_NEEDED then DT_RUNPATH (indexes: 6,1)")
    {
      section.addEntry( makeNeededEntry(6) );
      section.addEntry( makeRunPathEntry(1) );
      REQUIRE( section.getRunPath() == QLatin1String("/tmp") );
      REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );

      SECTION("set a shorter RunPath")
      {
        section.setRunPath( QLatin1String("/a") );
        REQUIRE( section.getRunPath() == QLatin1String("/a") );
        REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
      }

      SECTION("set a longer RunPath")
      {
        section.setRunPath( QLatin1String("/path1:/path2") );
        REQUIRE( section.getRunPath() == QLatin1String("/path1:/path2") );
        REQUIRE( section.getNeededSharedLibraries() == qStringListFromUtf8Strings({"libA.so"}) );
      }
    }
  }
}
