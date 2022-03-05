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

  static
  const BaseNameModulePair baseNameModuleMap[] = {
    {"Core",QtModule::Core},
    {"Gui",QtModule::Gui},
    {"Widgets",QtModule::Widgets},
    {"DBus",QtModule::DBus},
    {"Network",QtModule::Network}
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
