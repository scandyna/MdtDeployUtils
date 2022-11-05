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
#include "Mdt/DeployUtils/QtDistributionDirectory.h"
#include "TestFileUtils.h"
#include <QTemporaryDir>
#include <QDir>
#include <QFileInfo>
#include <QLatin1String>
#include <QString>
#include <QDebug>

using namespace Mdt::DeployUtils;

bool createQtConfFile(const QString & filePath)
{
  return createTextFileUtf8( filePath, QLatin1String("[Paths]") );
}


TEST_CASE("Construct")
{
  QtDistributionDirectory directory;

  REQUIRE( directory.isNull() );
}

TEST_CASE("rootAbsolutePath")
{
  QtDistributionDirectory directory;

  SECTION("default")
  {
    REQUIRE( !directory.hasRootPath() );
  }

  SECTION("set to /opt")
  {
    directory.setRootAbsolutePath( QLatin1String("/opt") );

    REQUIRE( directory.rootAbsolutePath() == QLatin1String("/opt") );
    REQUIRE( directory.hasRootPath() );
  }
}

TEST_CASE("guessRootAbsolutePathFromQtSharedLibrary")
{
  SECTION("common Linux distribution")
  {
    const QFileInfo qtLibrary( QLatin1String("/opt/qt/lib/libQt5Core.so") );

    REQUIRE( QtDistributionDirectory::guessRootAbsolutePathFromQtSharedLibrary(qtLibrary) == makeAbsolutePath("/opt/qt") );
  }

  SECTION("common Windows distribution")
  {
    const QFileInfo qtLibrary( QLatin1String("/qt/bin/Qt5Core.dll") );

    REQUIRE( QtDistributionDirectory::guessRootAbsolutePathFromQtSharedLibrary(qtLibrary) == makeAbsolutePath("/qt") );
  }

  SECTION("/usr/lib/x86_64-linux-gnu/libQt5Core.so")
  {
    const QFileInfo qtLibrary( QLatin1String("/usr/lib/x86_64-linux-gnu/libQt5Core.so") );

    REQUIRE( QtDistributionDirectory::guessRootAbsolutePathFromQtSharedLibrary(qtLibrary) == makeAbsolutePath("/usr") );
  }
}

TEST_CASE("setSharedLibrariesDirectoryRelativePath")
{
  QtDistributionDirectory directory;

  directory.setRootAbsolutePath( QLatin1String("/opt") );
  directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("lib") );

  REQUIRE( directory.sharedLibrariesDirectoryAbsolutePath() == QLatin1String("/opt/lib") );
}

TEST_CASE("guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary")
{
  SECTION("common Linux distribution")
  {
    const QFileInfo qtLibrary( QLatin1String("/opt/qt/lib/libQt5Core.so") );

    REQUIRE(
      QtDistributionDirectory::guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary(qtLibrary)
      ==
      QLatin1String("lib")
    );
  }

  SECTION("debian multi-arch")
  {
    const QFileInfo qtLibrary( QLatin1String("/usr/lib/x86_64-linux-gnu/libQt5Core.so") );

    REQUIRE(
      QtDistributionDirectory::guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary(qtLibrary)
      ==
      QLatin1String("lib/x86_64-linux-gnu")
    );
  }

  SECTION("common Windows distribution")
  {
    const QFileInfo qtLibrary( QLatin1String("/qt/bin/Qt5Core.dll") );

    REQUIRE(
      QtDistributionDirectory::guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary(qtLibrary)
      ==
      QLatin1String("bin")
    );
  }
}

TEST_CASE("plugins root")
{
  QtDistributionDirectory directory;
  directory.setRootAbsolutePath( QLatin1String("/opt/qt5") );

  SECTION("default")
  {
    REQUIRE( directory.pluginsRootAbsolutePath() == QLatin1String("/opt/qt5/plugins") );
  }

  SECTION("set a other plugins relative path")
  {
    directory.setPluginsRootRelativePath( QLatin1String("bin/archdatadir/plugins") );

    REQUIRE( directory.pluginsRootAbsolutePath() == QLatin1String("/opt/qt5/bin/archdatadir/plugins") );
  }
}

TEST_CASE("isNull")
{
  QtDistributionDirectory directory;

  SECTION("default")
  {
    REQUIRE( directory.isNull() );
  }

  SECTION("only root path")
  {
    directory.setRootAbsolutePath( QLatin1String("/opt") );

    REQUIRE( directory.isNull() );
  }

  SECTION("only shared libraries directory")
  {
    directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("lib") );

    REQUIRE( directory.isNull() );
  }

  SECTION("set root path and shared libraries directory")
  {
    directory.setRootAbsolutePath( QLatin1String("/opt") );
    directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("lib") );

    REQUIRE( !directory.isNull() );
  }
}

TEST_CASE("isValidExisting")
{
  QtDistributionDirectory directory;

  QTemporaryDir qtRoot;
  REQUIRE( qtRoot.isValid() );
  qtRoot.setAutoRemove(true);

  /*
   * QTDIR
   *   |-bin
   *   |-lib
   *   |-plugins
   */
  const QString qtBinDir = makePath(qtRoot, "bin");
  const QString qtLibDir = makePath(qtRoot, "lib");
  const QString qtPluginsDir = makePath(qtRoot, "plugins");

  REQUIRE( createDirectoryFromPath(qtBinDir) );
  REQUIRE( createDirectoryFromPath(qtLibDir) );
  REQUIRE( createDirectoryFromPath(qtPluginsDir) );

  SECTION("valid Qt distribution")
  {
    directory.setRootAbsolutePath( qtRoot.path() );
    directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("lib") );
    directory.setPluginsRootRelativePath( QLatin1String("plugins") );

    REQUIRE( directory.isValidExisting() );
  }

  SECTION("not valid: plugins dir not exists")
  {
    directory.setRootAbsolutePath( qtRoot.path() );
    directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("lib") );
    directory.setPluginsRootRelativePath( QLatin1String("bin/plugins") );

    REQUIRE( !directory.isValidExisting() );
  }
}

TEST_CASE("clear")
{
  QtDistributionDirectory directory;

  directory.setRootAbsolutePath( QLatin1String("/opt") );
  directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("lib") );
  directory.setPluginsRootRelativePath( QLatin1String("bin/plugins") );

  REQUIRE( directory.hasRootPath() );
  REQUIRE( directory.hasSharedLibrariesDirectory() );
  REQUIRE( directory.pluginsRootAbsolutePath() == makeAbsolutePath("/opt/bin/plugins") );

  directory.clear();

  REQUIRE( !directory.hasRootPath() );
  REQUIRE( !directory.hasSharedLibrariesDirectory() );
}

TEST_CASE("Change root path")
{
  QtDistributionDirectory directory;

  directory.setRootAbsolutePath( QLatin1String("/opt") );
  directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("lib") );
  REQUIRE( directory.sharedLibrariesDirectoryAbsolutePath() == QLatin1String("/opt/lib") );

  directory.setRootAbsolutePath( QLatin1String("/opt/qt5") );

  REQUIRE( directory.sharedLibrariesDirectoryAbsolutePath() == QLatin1String("/opt/qt5/lib") );
}

TEST_CASE("setEntriesFromQtConf")
{
  QtDistributionDirectory directory;
  QtConf qtConf;

  SECTION("From Qt Linux distribution")
  {
    const QFileInfo qtConfFilePath( QLatin1String("/opt/qt5/bin/qt.conf") );
    qtConf.setPrefixPath( QLatin1String("..") );

    directory.setEntriesFromQtConf(qtConf, OperatingSystem::Linux, qtConfFilePath);

    REQUIRE( directory.rootAbsolutePath() == makeAbsolutePath("/opt/qt5") );
  }

  SECTION("From Qt Windows distribution")
  {
    const QFileInfo qtConfFilePath( QLatin1String("/qt5/bin/qt.conf") );
    qtConf.setPrefixPath( QLatin1String("..") );

    directory.setEntriesFromQtConf(qtConf, OperatingSystem::Linux, qtConfFilePath);

    REQUIRE( directory.rootAbsolutePath() == makeAbsolutePath("/qt5") );
  }

  SECTION("From conan-center Linux distribution")
  {
    const QFileInfo qtConfFilePath( QLatin1String("/opt/conan/qt5/bin/qt.conf") );
    qtConf.setPrefixPath( QLatin1String("..") );
    qtConf.setPluginsPath( QLatin1String("bin/archdatadir/plugins") );

    directory.setEntriesFromQtConf(qtConf, OperatingSystem::Linux, qtConfFilePath);

    REQUIRE( directory.rootAbsolutePath() == makeAbsolutePath("/opt/conan/qt5") );
    REQUIRE( directory.pluginsRootAbsolutePath() == makeAbsolutePath("/opt/conan/qt5/bin/archdatadir/plugins") );
  }

  SECTION("Debian multi-arch distribution")
  {
    const QFileInfo qtConfFilePath( QLatin1String("/usr/lib/x86_64-linux-gnu/qt5/qt.conf") );
    qtConf.setPrefixPath( QLatin1String("/usr") );
    qtConf.setLibrariesPath( QLatin1String("lib/x86_64-linux-gnu") );
    qtConf.setPluginsPath( QLatin1String("lib/x86_64-linux-gnu/qt5/plugins") );

    directory.setEntriesFromQtConf(qtConf, OperatingSystem::Linux, qtConfFilePath);

    REQUIRE( directory.rootAbsolutePath() == makeAbsolutePath("/usr") );
    REQUIRE( directory.sharedLibrariesDirectoryAbsolutePath() == makeAbsolutePath("/usr/lib/x86_64-linux-gnu") );
    REQUIRE( directory.pluginsRootAbsolutePath() == makeAbsolutePath("/usr/lib/x86_64-linux-gnu/qt5/plugins") );
  }
}

TEST_CASE("setupFromQtSharedLibrary")
{
  QtDistributionDirectory directory;

  QTemporaryDir qtRoot;
  REQUIRE( qtRoot.isValid() );
  qtRoot.setAutoRemove(true);

  /*
   * Take a Linux Qt distribution provided by Conan center
   * QTDIR
   *   |-bin
   *   |  |-archdatadir
   *   |  |    |-plugins
   *   |  |-qt.conf
   *   |-lib
   *   |  |-libQt5Core.so
   */
  const QString qtBinDir = makePath(qtRoot, "bin");
  const QString qtLibDir = makePath(qtRoot, "lib");
  const QString qtPluginsDir = makePath(qtRoot, "bin/archdatadir/plugins");

  REQUIRE( createDirectoryFromPath(qtBinDir) );
  REQUIRE( createDirectoryFromPath(qtLibDir) );
  REQUIRE( createDirectoryFromPath(qtPluginsDir) );

  const QString qtConfFilePath = makePath(qtRoot, "bin/qt.conf");
  const QString qtConfFileContens = QLatin1String("[Paths]\nPrefix = ..\nPlugins = bin/archdatadir/plugins");
  REQUIRE( createTextFileUtf8(qtConfFilePath, qtConfFileContens) );

  const QString qtLibraryPath = makePath(qtRoot, "lib/libQt5Core.so");
  const QFileInfo qtLibrary(qtLibraryPath);

  directory.setupFromQtSharedLibrary(qtLibrary, OperatingSystem::Linux);

  REQUIRE( directory.rootAbsolutePath() == qtRoot.path() );
  REQUIRE( directory.sharedLibrariesDirectoryAbsolutePath() == qtLibDir );
  REQUIRE( directory.pluginsRootAbsolutePath() == qtPluginsDir );
}

TEST_CASE("findQtConfFileFromQtSharedLibrary")
{
  QtDistributionDirectory directory;

  QTemporaryDir qtRoot;
  REQUIRE( qtRoot.isValid() );
  qtRoot.setAutoRemove(true);

  SECTION("common distribution")
  {
    const QString qtBinDir = makePath(qtRoot, "bin");
    const QString qtLibDir = makePath(qtRoot, "lib");

    REQUIRE( createDirectoryFromPath(qtBinDir) );
    REQUIRE( createDirectoryFromPath(qtLibDir) );

    SECTION("qt.conf is in bin")
    {
      const QString qtConfFilePath = makePath(qtRoot, "bin/qt.conf");
      REQUIRE( createQtConfFile(qtConfFilePath) );

      SECTION("shared libraries are in bin")
      {
        const QString qtLibraryPath = makePath(qtRoot, "bin/Qt5Core.dll");
        const QFileInfo qtLibrary(qtLibraryPath);

        REQUIRE( QtDistributionDirectory::findQtConfFileFromQtSharedLibrary(qtLibrary) == qtConfFilePath );
      }

      SECTION("shared libraries are in lib")
      {
        const QString qtLibraryPath = makePath(qtRoot, "lib/libQt5Core.so");
        const QFileInfo qtLibrary(qtLibraryPath);

        REQUIRE( QtDistributionDirectory::findQtConfFileFromQtSharedLibrary(qtLibrary) == qtConfFilePath );
      }
    }

    SECTION("no qt.conf provided")
    {
      SECTION("shared libraries are in bin")
      {
        const QString qtLibraryPath = makePath(qtRoot, "bin/Qt5Core.dll");
        const QFileInfo qtLibrary(qtLibraryPath);

        REQUIRE( QtDistributionDirectory::findQtConfFileFromQtSharedLibrary(qtLibrary).isEmpty() );
      }
    }
  }

  SECTION("debian multi-arch distribution")
  {
    const QString qtBinDir = makePath(qtRoot, "usr/bin");
    const QString qtLibDir = makePath(qtRoot, "usr/lib/x86_64-linux-gnu");

    REQUIRE( createDirectoryFromPath(qtBinDir) );
    REQUIRE( createDirectoryFromPath(qtLibDir) );

    SECTION("qt5")
    {
      const QString qtConfDir = makePath(qtRoot, "usr/lib/x86_64-linux-gnu/qt5");
      REQUIRE( createDirectoryFromPath(qtConfDir) );
      const QString qtConfFilePath = makePath(qtRoot, "usr/lib/x86_64-linux-gnu/qt5/qt.conf");
      REQUIRE( createQtConfFile(qtConfFilePath) );

      const QString qtLibraryPath = makePath(qtRoot, "usr/lib/x86_64-linux-gnu/libQt5Core.so");
      const QFileInfo qtLibrary(qtLibraryPath);

      REQUIRE( QtDistributionDirectory::findQtConfFileFromQtSharedLibrary(qtLibrary) == qtConfFilePath );
    }

    SECTION("qt6")
    {
      const QString qtConfDir = makePath(qtRoot, "usr/lib/x86_64-linux-gnu/qt6");
      REQUIRE( createDirectoryFromPath(qtConfDir) );
      const QString qtConfFilePath = makePath(qtRoot, "usr/lib/x86_64-linux-gnu/qt6/qt.conf");
      REQUIRE( createQtConfFile(qtConfFilePath) );

      const QString qtLibraryPath = makePath(qtRoot, "usr/lib/x86_64-linux-gnu/libQt6Core.so");
      const QFileInfo qtLibrary(qtLibraryPath);

      REQUIRE( QtDistributionDirectory::findQtConfFileFromQtSharedLibrary(qtLibrary) == qtConfFilePath );
    }
  }
}

TEST_CASE("containsSharedLibrary")
{
  QtDistributionDirectory directory;

  QTemporaryDir qtRoot;
  REQUIRE( qtRoot.isValid() );
  qtRoot.setAutoRemove(true);

  /*
   * QTDIR
   *   |-bin
   *   |  |-Qt5Core.dll
   *   |-lib
   *   |  |-libQt5Core.so
   *   |-wrong
   *      |-Qt5Core.dll
   *      |-libQt5Core.so
   */
  const QString qtBinDir = makePath(qtRoot, "bin");
  const QString qtLibDir = makePath(qtRoot, "lib");
  const QString wrongQtDir = makePath(qtRoot, "wrong");

  REQUIRE( createDirectoryFromPath(qtBinDir) );
  REQUIRE( createDirectoryFromPath(qtLibDir) );
  REQUIRE( createDirectoryFromPath(wrongQtDir) );

  const QString qt5CoreContents;

  const QString wrongQt5CoreSoFilePath = makePath(qtRoot, "wrong/libQt5Core.so");
  const QString wrongQt5CoreDllFilePath = makePath(qtRoot, "wrong/Qt5Core.dll");

  REQUIRE( createTextFileUtf8(wrongQt5CoreSoFilePath, qt5CoreContents) );
  REQUIRE( createTextFileUtf8(wrongQt5CoreDllFilePath, qt5CoreContents) );

  directory.setRootAbsolutePath( qtRoot.path() );

  QFileInfo library;

  SECTION("Linux")
  {
    const QString qt5CoreSoFilePath = makePath(qtRoot, "lib/libQt5Core.so");
    REQUIRE( createTextFileUtf8(qt5CoreSoFilePath, qt5CoreContents) );

    directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("lib") );

    SECTION("libQt5Core.so is in Qt distribution")
    {
      library.setFile(qt5CoreSoFilePath);

      REQUIRE( directory.containsSharedLibrary(library) );
    }

    SECTION("libQt5Core.so is NOT in Qt distribution")
    {
      library.setFile(wrongQt5CoreSoFilePath);

      REQUIRE( !directory.containsSharedLibrary(library) );
    }

    SECTION("/usr/lib/libQt5Core.so is not in Qt distribution")
    {
      library.setFile( QLatin1String("/usr/lib/libQt5Core.so") );

      REQUIRE( !directory.containsSharedLibrary(library) );
    }
  }

  SECTION("Windows")
  {
    const QString qt5CoreDllFilePath = makePath(qtRoot, "bin/Qt5Core.dll");
    REQUIRE( createTextFileUtf8(qt5CoreDllFilePath, qt5CoreContents) );

    directory.setSharedLibrariesDirectoryRelativePath( QLatin1String("bin") );

    SECTION("Qt5Core.dll is in Qt distribution")
    {
      library.setFile(qt5CoreDllFilePath);

      REQUIRE( directory.containsSharedLibrary(library) );
    }

    SECTION("Qt5Core.dll is NOT in Qt distribution")
    {
      library.setFile(wrongQt5CoreDllFilePath);

      REQUIRE( !directory.containsSharedLibrary(library) );
    }
  }
}

TEST_CASE("fileIsPlugin")
{
  QTemporaryDir qtRoot;
  REQUIRE( qtRoot.isValid() );
  qtRoot.setAutoRemove(true);

  /*
   * QTDIR
   *   |-plugins
   *        |-platforms
   */
  const QString qtPluginsDir = makePath(qtRoot, "plugins");
  const QString qtPlatformsPluginsDir = makePath(qtRoot, "plugins/platforms");

  REQUIRE( createDirectoryFromPath(qtPluginsDir) );
  REQUIRE( createDirectoryFromPath(qtPlatformsPluginsDir) );

  QtDistributionDirectory directory;
  directory.setRootAbsolutePath( qtRoot.path() );

  QString filePath;

  SECTION("plugins/platforms/file.txt NOT a Qt plugin")
  {
    filePath = makePath(qtPlatformsPluginsDir, "file.txt");
    REQUIRE( createTextFileUtf8( filePath, QLatin1String("ABCD") ) );

    REQUIRE( !directory.fileIsPlugin(filePath) );
  }
}
