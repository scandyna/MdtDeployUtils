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
#include "Mdt/DeployUtils/QtPluginFile.h"
#include <QTemporaryDir>
#include <QLatin1String>
#include <QString>
#include <vector>
#include <string>

using namespace Mdt::DeployUtils;


TEST_CASE("QtPluginFile")
{
  QString pluginPath;
  QtPluginFile qtPlugin;

  SECTION("plugins/platforms/somelib.so")
  {
    pluginPath = QLatin1String("/tmp/plugins/platforms/somelib.so");

    qtPlugin = QtPluginFile::fromQFileInfoUnchecked(pluginPath);

    REQUIRE( qtPlugin.directoryName() == QLatin1String("platforms") );
  }
}


QtPluginFileList makeQtPluginsForDirectoryNames(const std::vector<std::string> & directoryNames)
{
  QtPluginFileList qtPlugins;

  for(const std::string & dirName : directoryNames){
    const QFileInfo qtPluginPath = QLatin1String("/tmp/plugins/") + QString::fromStdString(dirName) + QLatin1String("/somelib.so");
    qtPlugins.emplace_back( QtPluginFile::fromQFileInfoUnchecked(qtPluginPath) );
  }

  return qtPlugins;
}

QtPluginFileList makeQtPluginsForFilePaths(const std::vector<std::string> & filePaths)
{
  QtPluginFileList qtPlugins;

  for(const std::string & filePath : filePaths){
    const QFileInfo & qtPluginPath = QString::fromStdString(filePath);
    qtPlugins.emplace_back( QtPluginFile::fromQFileInfoUnchecked(qtPluginPath) );
  }

  return qtPlugins;
}

TEST_CASE("getQtPluginsDirectoryNames")
{
  QtPluginFileList qtPlugins;
  QStringList directoryNames;

  SECTION("empty list")
  {
    REQUIRE( getQtPluginsDirectoryNames(qtPlugins).isEmpty() );
  }

  SECTION("platforms")
  {
    qtPlugins = makeQtPluginsForDirectoryNames({"platforms"});

    directoryNames = getQtPluginsDirectoryNames(qtPlugins);

    REQUIRE( directoryNames.size() == 1 );
    REQUIRE( directoryNames.contains( QLatin1String("platforms") ) );
  }

  SECTION("platforms,audio,platforms")
  {
    qtPlugins = makeQtPluginsForDirectoryNames({"platforms","audio","platforms"});

    directoryNames = getQtPluginsDirectoryNames(qtPlugins);

    REQUIRE( directoryNames.size() == 2 );
    REQUIRE( directoryNames.contains( QLatin1String("platforms") ) );
    REQUIRE( directoryNames.contains( QLatin1String("audio") ) );
  }
}

TEST_CASE("getQtPluginsFilePathList")
{
  QtPluginFileList qtPlugins;
  QStringList filePathList;

  SECTION("empty list")
  {
    REQUIRE( getQtPluginsFilePathList(qtPlugins).isEmpty() );
  }

  SECTION("imageformats/libqjpeg.so,imageformats/libqsvg.so,platforms/libqxcb.so")
  {
    qtPlugins = makeQtPluginsForFilePaths(
      {"/tmp/plugins/imageformats/libqjpeg.so",
      "/tmp/plugins/imageformats/libqsvg.so",
      "/tmp/plugins/platforms/libqxcb.so"}
    );

    filePathList = getQtPluginsFilePathList(qtPlugins);

    REQUIRE( filePathList.size() == 3 );
    REQUIRE( filePathList.contains( makeAbsolutePath("/tmp/plugins/imageformats/libqjpeg.so") ) );
    REQUIRE( filePathList.contains( makeAbsolutePath("/tmp/plugins/imageformats/libqsvg.so") ) );
    REQUIRE( filePathList.contains( makeAbsolutePath("/tmp/plugins/platforms/libqxcb.so") ) );
  }
}

TEST_CASE("toFileInfoList")
{
  QtPluginFileList qtPlugins;
  QFileInfoList files;

  SECTION("empty list")
  {
    REQUIRE( toFileInfoList(qtPlugins).isEmpty() );
  }

  SECTION("imageformats/libqjpeg.so,imageformats/libqsvg.so,platforms/libqxcb.so")
  {
    qtPlugins = makeQtPluginsForFilePaths(
      {"/tmp/plugins/imageformats/libqjpeg.so",
      "/tmp/plugins/imageformats/libqsvg.so",
      "/tmp/plugins/platforms/libqxcb.so"}
    );

    files = toFileInfoList(qtPlugins);

    REQUIRE( files.size() == 3 );
    REQUIRE( files.contains( makeAbsolutePath("/tmp/plugins/imageformats/libqjpeg.so") ) );
    REQUIRE( files.contains( makeAbsolutePath("/tmp/plugins/imageformats/libqsvg.so") ) );
    REQUIRE( files.contains( makeAbsolutePath("/tmp/plugins/platforms/libqxcb.so") ) );
  }
}
