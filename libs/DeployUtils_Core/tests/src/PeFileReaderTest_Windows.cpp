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
#include "TestUtils.h"
#include "Mdt/DeployUtils/PeFileReader.h"
#include <QString>
#include <QLatin1String>

// #include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
// #include "Mdt/DeployUtils/Impl/FileMapper.h"
// #include <QFile>
// #include <QDebug>
// #include <iostream>

/// \todo must be defined by CMake
// #define TEST_SHARED_LIBRARY_FILE_PATH "/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Widgets.dll"
// #define QT5_CORE_FILE_PATH "/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Cored.dll"
// #define TEST_STATIC_LIBRARY_FILE_PATH "/home/philippe/.wine/drive_c/TDM-GCC-32/lib/libaclui.a"
// #define TEST_DYNAMIC_EXECUTABLE_FILE_PATH "/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/moc.exe"

using namespace Mdt::DeployUtils;

TEST_CASE("isPeImageFile")
{
  PeFileReader reader;

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( reader.isPeImageFile() );
  }

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.isPeImageFile() );
  }
}

TEST_CASE("isExecutableOrSharedLibrary")
{
  PeFileReader reader;

  SECTION("shared library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
    REQUIRE( reader.isExecutableOrSharedLibrary() );
    reader.close();
  }

  SECTION("static library")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_STATIC_LIBRARY_FILE_PATH) );
    REQUIRE( !reader.isExecutableOrSharedLibrary() );
    reader.close();
  }

  SECTION("dynamic linked executable")
  {
    reader.openFile( QString::fromLocal8Bit(TEST_DYNAMIC_EXECUTABLE_FILE_PATH) );
    REQUIRE( reader.isExecutableOrSharedLibrary() );
    reader.close();
  }
}

TEST_CASE("getNeededSharedLibraries")
{
  PeFileReader reader;
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
    libraries = reader.getNeededSharedLibraries();
    REQUIRE( !libraries.isEmpty() );
    REQUIRE( containsTestSharedLibrary(libraries) );
    REQUIRE( containsQt5Core(libraries) );
    reader.close();
  }

  /// \todo FIXME
//   SECTION("dynamic linked executable with delay load table")
//   {
//     reader.openFile( QString::fromLocal8Bit("/home/philippe/Software/Windows/vs_community__1782162290.1623490489.exe") );
//     libraries = reader.getNeededSharedLibraries();
//     REQUIRE( !libraries.isEmpty() );
//     REQUIRE( libraries.contains( QLatin1String("KERNEL32.dll") ) );
//     REQUIRE( libraries.contains( QLatin1String("USER32.dll") ) );
//     reader.close();
//   }
}

TEST_CASE("open_2_consecutive_files_with_1_instance")
{
  PeFileReader reader;

  /*
   * Open a big library first
   */

  reader.openFile( QString::fromLocal8Bit(QT5_CORE_FILE_PATH) );
  REQUIRE( !reader.getNeededSharedLibraries().isEmpty() );
  reader.close();

  reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
  REQUIRE( !reader.getNeededSharedLibraries().isEmpty() );
  reader.close();
}

TEST_CASE("call_many_members_one_1_instance")
{
  PeFileReader reader;

  reader.openFile( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
  REQUIRE( reader.isExecutableOrSharedLibrary() );
  REQUIRE( !reader.getNeededSharedLibraries().isEmpty() );
  reader.close();
}


// TEST_CASE("sandbox")
// {
//   using Impl::ByteArraySpan;
//   using Impl::FileMapper;
// 
// //   qDebug() << "TEST_SHARED_LIBRARY_FILE_PATH: " << TEST_SHARED_LIBRARY_FILE_PATH;
// // 
// //   QFile file;
// // //   file.setFileName( QLatin1String("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Cored.dll") );
// //   file.setFileName( QString::fromLocal8Bit(TEST_SHARED_LIBRARY_FILE_PATH) );
// //   REQUIRE( file.open(QIODevice::ReadOnly) );
// // 
// //   FileMapper mapper;
// //   const ByteArraySpan map = mapper.mapIfRequired( file, 0, file.size() );
// //   
// //   qDebug() << "file size: " << map.size;
// 
//   SECTION("Qt5Core")
//   {
//     qDebug() << "\nQt5Core.dll";
// 
//     PeFileReader reader;
//     reader.openFile( QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Core.dll") );
//     std::cout << reader.getNeededSharedLibraries().join( QLatin1Char('\n') ).toStdString() << std::endl;
//   }
// 
//   SECTION("Qt5Cored")
//   {
//     qDebug() << "\nQt5Cored.dll";
// 
//     PeFileReader reader;
//     reader.openFile( QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Cored.dll") );
//     std::cout << reader.getNeededSharedLibraries().join( QLatin1Char('\n') ).toStdString() << std::endl;
//   }
// 
//   SECTION("Qt5Gui")
//   {
//     qDebug() << "\nQt5Gui.dll";
// 
//     PeFileReader reader;
//     reader.openFile( QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Qt/Qt5.6.2/5.6/mingw49_32/bin/Qt5Gui.dll") );
//     std::cout << reader.getNeededSharedLibraries().join( QLatin1Char('\n') ).toStdString() << std::endl;
//   }
// 
// //   SECTION("win media player")
// //   {
// //     qDebug() << "\nwin media player";
// // 
// //     PeFileReader reader;
// //     reader.openFile( QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Program Files/Windows Media Player/wmplayer.exe") );
// //     reader.sandbox();
// //   }
// 
//   SECTION("7z")
//   {
//     qDebug() << "\n7z";
// 
//     PeFileReader reader;
//     reader.openFile( QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Program Files/7-Zip/7z.exe") );
//     std::cout << reader.getNeededSharedLibraries().join( QLatin1Char('\n') ).toStdString() << std::endl;
//   }
// 
//   SECTION("Qt installer")
//   {
//     qDebug() << "\nQt installer";
// 
//     PeFileReader reader;
//     reader.openFile( QString::fromLocal8Bit("/home/philippe/.wine/drive_c/Qt/qt-unified-windows-x86-4.1.1-online.exe") );
//     std::cout << reader.getNeededSharedLibraries().join( QLatin1Char('\n') ).toStdString() << std::endl;
//   }
// 
//   SECTION("Visual Studio (installer)")
//   {
//     qDebug() << "\nVisual Studio (installer)";
// 
//     PeFileReader reader;
//     reader.openFile( QString::fromLocal8Bit("/home/philippe/Software/Windows/vs_community__1782162290.1623490489.exe") );
//     std::cout << reader.getNeededSharedLibraries().join( QLatin1Char('\n') ).toStdString() << std::endl;
//   }
// }
