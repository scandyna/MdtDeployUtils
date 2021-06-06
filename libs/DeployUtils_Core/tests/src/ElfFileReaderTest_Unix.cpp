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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/ElfFileReader.h"
#include "Mdt/DeployUtils/LibraryName.h"
#include <QString>
#include <QLatin1String>

using namespace Mdt::DeployUtils;

bool containsQt5Core(const QStringList & libraries)
{
  for(const QString & library : libraries){
    const LibraryName libraryName(library);
    if( libraryName.name() == QLatin1String("Qt5Core") ){
      return true;
    }
  }

  return false;
}

TEST_CASE("isElfFile")
{
  SECTION("shared library")
  {
    REQUIRE( ElfFileReader::isElfFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) ) );
  }

  /*
   * Would require to open the archive and inspect contents,
   * not supported now
   */
//   SECTION("static library")
//   {
//     REQUIRE( ElfFileReader::isElfFile( QString::fromLocal8Bit(TEST_STATIC_LIBRARY_FILE_PATH) ) );
//   }

  SECTION("dynamic linked executable")
  {
    REQUIRE( ElfFileReader::isElfFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) ) );
  }
}

TEST_CASE("isDynamicLinkedExecutableOrLibrary")
{
  ElfFileReader reader;

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( reader.isDynamicLinkedExecutableOrLibrary() );
    reader.close();
  }

  SECTION("static library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_STATIC_LIBRARY_FILE_PATH) );
    REQUIRE( !reader.isDynamicLinkedExecutableOrLibrary() );
    reader.close();
  }

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.isDynamicLinkedExecutableOrLibrary() );
    reader.close();
  }
}

TEST_CASE("getNeededSharedLibraries")
{
  ElfFileReader reader;
  QStringList libraries;

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    libraries = reader.getNeededSharedLibraries();
    REQUIRE( !libraries.isEmpty() );
    REQUIRE( containsQt5Core(libraries) );
    reader.close();
  }

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( !reader.getNeededSharedLibraries().isEmpty() );
    reader.close();
  }
}

TEST_CASE("getRunPath")
{
  ElfFileReader reader;

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( !reader.getRunPath().isEmpty() );
    reader.close();
  }

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( !reader.getRunPath().isEmpty() );
    reader.close();
  }
}

TEST_CASE("getSoName")
{
  ElfFileReader reader;

  SECTION("libtestSharedLibrary.so")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( reader.getSoName() == QLatin1String("libtestSharedLibrary.so") );
    reader.close();
  }
}

TEST_CASE("open_2_consecutive_files_with_1_instance")
{
  ElfFileReader reader;

  /*
   * Open a big library first
   */

  reader.openFile( QString::fromLocal8Bit(QT5_CORE_FILE_PATH) );
  REQUIRE( reader.getSoName() == QLatin1String("libQt5Core.so.5") );
  reader.close();

  reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
  REQUIRE( reader.getSoName() == QLatin1String("libtestSharedLibrary.so") );
  reader.close();
}

TEST_CASE("call_many_members_one_1_instance")
{
  ElfFileReader reader;

  reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
  REQUIRE( reader.isDynamicLinkedExecutableOrLibrary() );
  REQUIRE( reader.getSoName() == QLatin1String("libtestSharedLibrary.so") );
  REQUIRE( !reader.getNeededSharedLibraries().isEmpty() );
  REQUIRE( !reader.getRunPath().isEmpty() );
  reader.close();
}
