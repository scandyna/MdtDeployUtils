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
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/QtModulePlugins.h"
#include "Mdt/DeployUtils/Platform.h"
#include <QLatin1String>
#include <QString>
#include <QTemporaryDir>

// #include <QDebug>

using namespace Mdt::DeployUtils;

const QString qtGuiLibraryFilePath = QString::fromLocal8Bit(TEST_QT_GUI_FILE_PATH);


TEST_CASE("getPluginsDirectoriesForModule")
{
  QStringList directories;

  SECTION("Qt GUI")
  {
    directories = QtModulePlugins::getPluginsDirectoriesForModule(QtModule::Gui);

    REQUIRE( directories.count() >= 4 );
    REQUIRE( directories.count( QLatin1String("iconengines") ) == 1 );
    REQUIRE( directories.count( QLatin1String("imageformats") ) == 1 );
    REQUIRE( directories.count( QLatin1String("platforms") ) == 1 );
    REQUIRE( directories.count( QLatin1String("platforminputcontexts") ) == 1 );
  }
}

TEST_CASE("getPluginsDirectoriesForModules")
{
  QStringList directories;
  QtModuleList modules;

  SECTION("Qt GUI, Qt Virtual Keynoard")
  {
    modules.addModule(QtModule::Gui);
    modules.addModule(QtModule::VirtualKeyboard);

    directories = QtModulePlugins::getPluginsDirectoriesForModules(modules);

    REQUIRE( directories.count( QLatin1String("iconengines") ) == 1 );
    REQUIRE( directories.count( QLatin1String("imageformats") ) == 1 );
    REQUIRE( directories.count( QLatin1String("platforms") ) == 1 );
    REQUIRE( directories.count( QLatin1String("platforminputcontexts") ) == 1 );
    REQUIRE( directories.count( QLatin1String("virtualkeyboard") ) == 1 );
  }
}

TEST_CASE("getPluginsForModules")
{
  QtDistributionDirectory qtDistributionDirectory;
  qtDistributionDirectory.setupFromQtSharedLibrary( qtGuiLibraryFilePath, Platform::nativeOperatingSystem() );

  QtPluginFileList plugins;
  QtPluginsSet pluginsSet;
  QtModulePlugins modulePlugins;
  QStringList pluginsDirectories;

  QtModuleList modules;
  modules.addModule(QtModule::Gui);

  plugins = modulePlugins.getPluginsForModules(modules, qtDistributionDirectory, pluginsSet);

  REQUIRE( !plugins.empty() );

  pluginsDirectories = getQtPluginsDirectoryNames(plugins);

  REQUIRE( pluginsDirectories.count( QLatin1String("imageformats") ) == 1 );
  REQUIRE( pluginsDirectories.count( QLatin1String("platforms") ) == 1 );
  /*
   * Some distributions, like Conan center,
   * does not distribute iconengines
   */
//   REQUIRE( pluginsDirectories.count( QLatin1String("iconengines") ) == 1 );
  /*
   * On some platforms (f.ex. some Windows Qt distributions),
   * platforminputcontexts is only present if some optional Qt Modules are installed,
   * like Qt Virtual Keyboard.
   * see also: https://gitlab.com/scandyna/mdtdeployutils/-/pipelines/511260803
   */
//   REQUIRE( pluginsDirectories.count( QLatin1String("platforminputcontexts") ) == 1 );
}
