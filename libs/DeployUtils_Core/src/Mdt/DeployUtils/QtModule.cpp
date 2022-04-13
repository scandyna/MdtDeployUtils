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
#include "QtModule.h"
#include <QCoreApplication>
#include <QString>
#include <QStringRef>
#include <QLatin1String>
#include <algorithm>
#include <iterator>

namespace Mdt{ namespace DeployUtils{

void throwUnknownQtModuleError(const LibraryName & libraryName)
{
  const QString msg = QCoreApplication::translate(
    "Mdt::DeployUtils::qtModuleFromLibraryName()",
    "given library '%1' is not part of a known Qt module"
  ).arg( libraryName.fullName() );

  throw UnknownQtModuleError(msg);
}

QtModule qtModuleFromLibraryName(const LibraryName & libraryName)
{
  const QString libraryBaseName = libraryName.nameWithoutDebugSuffix();

  if( libraryBaseName.size() < 4 ){
    throwUnknownQtModuleError(libraryName);
  }
  if( !libraryBaseName.startsWith( QLatin1String("Qt") ) ){
    throwUnknownQtModuleError(libraryName);
  }

  /*
   * Extract the name without Qt5 or Qt6
   * Here, we should have somting like Core, Gui, ...
   */
  const QStringRef qtLibraryBaseName = libraryBaseName.rightRef(libraryBaseName.size() - 3);

  struct BaseNameModulePair
  {
    const char *libraryBaseName;
    QtModule module;
  };

  /*
   * Table that maps a Qt library name to a Qt Module
   *
   * Some modules, like Qt Multimedia, ships several shared libraries
   * (f.ex. libQt5Multimedia.so, libQt5MultimediaWidgets.so, libQt5MultimediaQuick.so, ...).
   * MultimediaWidgets, MultimediaQuick, etc.. all depends on Multimedia (checked with ldd),
   * so only Multimedia has to be present in this table.
   * (f.ex. if MultimediaWidgets is used in a app, Multimedia will be present in the dependencies list)
   */
  static
  const BaseNameModulePair baseNameModuleMap[] = {
    // Qt Essentials
    {"Core",QtModule::Core},
    {"Gui",QtModule::Gui},
    {"Widgets",QtModule::Widgets},
    {"DBus",QtModule::DBus},
    {"Network",QtModule::Network},
    {"Test",QtModule::Test},
    {"Qml",QtModule::Qml},
    {"Quick",QtModule::Quick},
    {"QuickControls2",QtModule::QuickControls},
    {"QuickControls2Impl",QtModule::QuickControls},
    {"QuickDialogs2",QtModule::QuickDialogs},
//     {"QuickDialogs2QuickImpl",QtModule::QuickDialogs},
    {"QuickDialogs2Utils",QtModule::QuickDialogs},
    {"QuickLayouts",QtModule::QuickLayouts},
    {"QuickTest",QtModule::QuickTest},
    // Qt Addons
    {"Sql",QtModule::Sql},
    {"PrintSupport",QtModule::PrintSupport},
    {"Multimedia",QtModule::Multimedia}
    // Additional Qt libraries
  };

  const auto pred = [&qtLibraryBaseName](const BaseNameModulePair & mln){
    return QStringRef::compare(qtLibraryBaseName, QLatin1String(mln.libraryBaseName), Qt::CaseSensitive) == 0;
  };

  const auto it = std::find_if(std::cbegin(baseNameModuleMap), std::cend(baseNameModuleMap), pred);
  if( it == std::cend(baseNameModuleMap) ){
    throwUnknownQtModuleError(libraryName);
  }

  return it->module;
}

}} // namespace Mdt{ namespace DeployUtils{
