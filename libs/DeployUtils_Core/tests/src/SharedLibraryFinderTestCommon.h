// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#include "TestFileUtils.h"
#include "Mdt/DeployUtils/OperatingSystem.h"
#include <QTemporaryDir>
#include <QString>
#include <QLatin1String>
#include <cassert>

bool makeValidQtDistributionDirectory(const QTemporaryDir & qtRoot,
                                      Mdt::DeployUtils::OperatingSystem os)
{
  using Mdt::DeployUtils::OperatingSystem;

  assert( qtRoot.isValid() );

  /*
   * QTDIR
   *   |-bin
   *   |-lib
   *   |-plugins
   */
  const QString qtBinDir = makePath(qtRoot, "bin");
  const QString qtLibDir = makePath(qtRoot, "lib");
  const QString qtPluginsDir = makePath(qtRoot, "plugins");

  if( !createDirectoryFromPath(qtBinDir) ){
    return false;
  }
  if( !createDirectoryFromPath(qtLibDir) ){
    return false;
  }
  if( !createDirectoryFromPath(qtPluginsDir) ){
    return false;
  }

  QString qt5CoreLibraryPath;
  if(os == OperatingSystem::Windows){
    qt5CoreLibraryPath = qtBinDir + QLatin1String("/Qt5Core.dll");
  }else{
    qt5CoreLibraryPath = qtLibDir + QLatin1String("/libQt5Core.so");
  }

  if( !createTextFileUtf8( qt5CoreLibraryPath, QLatin1String("qt5") ) ){
    return false;
  }

  return true;
}
