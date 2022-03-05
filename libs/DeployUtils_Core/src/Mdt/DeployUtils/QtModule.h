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
#ifndef MDT_DEPLOY_UTILS_QT_MODULE_H
#define MDT_DEPLOY_UTILS_QT_MODULE_H

#include "LibraryName.h"
#include "UnknownQtModuleError.h"
#include "mdt_deployutilscore_export.h"

namespace Mdt{ namespace DeployUtils{

  /*! \brief Qt modules
   *
   * A Qt module could be confused with a library.
   * For example, Qt Core ends up to a shared library
   * (libQt5Core.so , Qt5Core.dll, depending on the platform).
   *
   * In our context, Qt Core is a module,
   * and libQt5Core.so is a Qt shared library.
   *
   * \sa https://doc.qt.io/qt.html
   */
  enum class QtModule
  {
    Core,     /*!< Qt Core */
    Gui,      /*!< Qt GUI */
    Widgets,  /*!< Qt Widgets */
    DBus,     /*!< Qt D-Bus */
    Network,  /*!< Qt Network */
    Qml,      /*!< Qt QML */
    Quick,    /*!< Qt Quick */
    QuickControls,  /*!< Qt Quick Controls */
    QuickDialogs,   /*!< Qt Quick Dialogs */
    QuickLayouts,   /*!< Qt Quick Layouts */
    QuickTest,      /*!< Qt Quick Test */
    VirtualKeyboard /*!< Qt Virtual Keyboard */
  };

  /*! \brief Get the Qt module in which the Qt library is part of
   *
   * \exception UnknownQtModuleError
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QtModule qtModuleFromLibraryName(const LibraryName & libraryName);

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_MODULE_H
