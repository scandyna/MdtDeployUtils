/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2022 Philippe Steinmann.
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
#include "Mdt/DeployUtils/QtModuleList.h"
#include <QLatin1String>
#include <QString>
#include <QDir>
#include <vector>
#include <exception>

using namespace Mdt::DeployUtils;


QString qtLibraryNameFromModule(QtModule module)
{
  switch(module){
    case QtModule::Core:
      return QLatin1String("libQt5Core.so");
    case QtModule::Gui:
      return QLatin1String("libQt5Gui.so");
    default:
      break;
  }

  throw std::runtime_error("tests, qtLibraryNameFromModule(): given unknown Qt module");
}

QtSharedLibraryFile makeQtSharedLibrary(QtModule module)
{
  const QString libraryPath = QDir::cleanPath( QLatin1String("/tmp/") + qtLibraryNameFromModule(module) );

  return QtSharedLibraryFile::fromQFileInfo(libraryPath);
}

QtSharedLibraryFileList makeQtSharedLibraries(const std::vector<QtModule> & modules)
{
  QtSharedLibraryFileList list;

  for(const QtModule module : modules){
    list.push_back( makeQtSharedLibrary(module) );
  }

  return list;
}


TEST_CASE("add_checkBasicAttributes")
{
  QtModuleList list;

  SECTION("initially the list is empty")
  {
    REQUIRE( list.count() == 0 );
    REQUIRE( list.isEmpty() );
  }

  SECTION("add a module")
  {
    list.addModule(QtModule::Core);

    REQUIRE( list.count() == 1 );
    REQUIRE( !list.isEmpty() );
    REQUIRE( list.at(0) == QtModule::Core );
  }
}

TEST_CASE("contains")
{
  QtModuleList list;

  SECTION("empty list")
  {
    REQUIRE( !list.contains(QtModule::Core) );
    REQUIRE( !list.contains(QtModule::Gui) );
  }

  SECTION("list contains Qt Core")
  {
    list.addModule(QtModule::Core);

    REQUIRE( list.contains(QtModule::Core) );
    REQUIRE( !list.contains(QtModule::Gui) );
  }
}

TEST_CASE("add_checkUnique")
{
  QtModuleList list;

  SECTION("add Qt Core twice")
  {
    list.addModule(QtModule::Core);
    REQUIRE( list.count() == 1 );

    list.addModule(QtModule::Core);
    REQUIRE( list.count() == 1 );
    REQUIRE( list.contains(QtModule::Core) );
  }

  SECTION("Add Qt Core and Qt Gui")
  {
    list.addModule(QtModule::Core);
    REQUIRE( list.count() == 1 );

    list.addModule(QtModule::Gui);
    REQUIRE( list.count() == 2 );
    REQUIRE( list.contains(QtModule::Core) );
    REQUIRE( list.contains(QtModule::Gui) );
  }
}

TEST_CASE("fromQtSharedLibraries")
{
  QtModuleList qtModules;
  QtSharedLibraryFileList qtLibraries;

  SECTION("libQt5Core.so,libQt5Gui.so,libQt5Core.so")
  {
    qtLibraries = makeQtSharedLibraries({QtModule::Core,QtModule::Gui,QtModule::Core});

    qtModules = QtModuleList::fromQtSharedLibraries(qtLibraries);

    REQUIRE( qtModules.count() == 2 );
    REQUIRE( qtModules.contains(QtModule::Core) );
    REQUIRE( qtModules.contains(QtModule::Gui) );
  }
}
