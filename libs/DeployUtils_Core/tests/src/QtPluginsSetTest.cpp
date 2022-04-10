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
#include "TestUtils.h"
#include "Mdt/DeployUtils/QtPluginsSet.h"
#include <QLatin1String>
#include <string>

using namespace Mdt::DeployUtils;

QtPluginFile makeQtPlugin(const std::string & path)
{
  return QtPluginFile::fromQFileInfoUnchecked( QString::fromStdString(path) );
}


TEST_CASE("set_contains")
{
  QtPluginsSet set;

  set.set( QLatin1String("platforms"), qStringListFromUtf8Strings({"xcb","vnc"}) );

  REQUIRE( set.contains( QLatin1String("platforms"), QLatin1String("xcb") ) );
  REQUIRE( set.contains( QLatin1String("platforms"), QLatin1String("vnc") ) );
  REQUIRE( !set.contains( QLatin1String("platforms"), QLatin1String("linuxfb") ) );
  REQUIRE( !set.contains( QLatin1String("imageformats"), QLatin1String("xcb") ) );
}

TEST_CASE("shouldDeployPlugin")
{
  QtPluginsSet set;

  const QtPluginFile libQjpegSo = makeQtPlugin("/tmp/plugins/imageformats/libqjpeg.so");
  const QtPluginFile qjpegDll = makeQtPlugin("/tmp/plugins/imageformats/qjpeg.dll");
  const QtPluginFile qjpegDdll = makeQtPlugin("/tmp/plugins/imageformats/qjpegd.dll");
  const QtPluginFile libQsvgSo = makeQtPlugin("/tmp/plugins/imageformats/libqsvg.so");
  const QtPluginFile qsvgDll = makeQtPlugin("/tmp/plugins/imageformats/qsvg.dll");
  const QtPluginFile libQxcbSo = makeQtPlugin("/tmp/plugins/platforms/libqxcb.so");
  const QtPluginFile qwindowsDll = makeQtPlugin("/tmp/plugins/platforms/qwindows.dll");
  const QtPluginFile libQminimalSo = makeQtPlugin("/tmp/plugins/platforms/libqminimal.so");
  const QtPluginFile qminimalDll = makeQtPlugin("/tmp/plugins/platforms/qminimal.dll");

  SECTION("empty set - deploy all plugins")
  {
    REQUIRE( set.shouldDeployPlugin(libQjpegSo) );
    REQUIRE( set.shouldDeployPlugin(qjpegDll) );
    REQUIRE( set.shouldDeployPlugin(qjpegDdll) );
    REQUIRE( set.shouldDeployPlugin(libQsvgSo) );
    REQUIRE( set.shouldDeployPlugin(qsvgDll) );
    REQUIRE( set.shouldDeployPlugin(libQxcbSo) );
    REQUIRE( set.shouldDeployPlugin(qwindowsDll) );
    REQUIRE( set.shouldDeployPlugin(libQminimalSo) );
    REQUIRE( set.shouldDeployPlugin(qminimalDll) );
  }

  SECTION("for imageformats only deploy jpeg")
  {
    set.set( QLatin1String("imageformats"), qStringListFromUtf8Strings({"jpeg"}) );

    // imageformats plugins
    REQUIRE( set.shouldDeployPlugin(libQjpegSo) );
    REQUIRE( set.shouldDeployPlugin(qjpegDll) );
    REQUIRE( set.shouldDeployPlugin(qjpegDdll) );
    REQUIRE( !set.shouldDeployPlugin(libQsvgSo) );
    REQUIRE( !set.shouldDeployPlugin(qsvgDll) );
    // platforms plugins
    REQUIRE( set.shouldDeployPlugin(libQxcbSo) );
    REQUIRE( set.shouldDeployPlugin(qwindowsDll) );
    REQUIRE( set.shouldDeployPlugin(libQminimalSo) );
    REQUIRE( set.shouldDeployPlugin(qminimalDll) );
  }

  SECTION("for platforms only deploy xcb and windows")
  {
    set.set( QLatin1String("platforms"), qStringListFromUtf8Strings({"xcb","windows"}) );

    // imageformats plugins
    REQUIRE( set.shouldDeployPlugin(libQjpegSo) );
    REQUIRE( set.shouldDeployPlugin(qjpegDll) );
    REQUIRE( set.shouldDeployPlugin(qjpegDdll) );
    REQUIRE( set.shouldDeployPlugin(libQsvgSo) );
    REQUIRE( set.shouldDeployPlugin(qsvgDll) );
    // platforms plugins
    REQUIRE( set.shouldDeployPlugin(libQxcbSo) );
    REQUIRE( set.shouldDeployPlugin(qwindowsDll) );
    REQUIRE( !set.shouldDeployPlugin(libQminimalSo) );
    REQUIRE( !set.shouldDeployPlugin(qminimalDll) );
  }
}
