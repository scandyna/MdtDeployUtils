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
#ifndef MDT_DEPLOY_UTILS_QT_PLUGINS_SET_H
#define MDT_DEPLOY_UTILS_QT_PLUGINS_SET_H

#include "QtPluginFile.h"
#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QStringList>
#include <QMap>
// #include <vector>
// #include <string>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Set of Qt plugins to deploy
   *
   * QtModulePlugins will take Qt plugins depending on the used Qt Modules.
   * For example, if Qt GUI is used (i.e. libQt5Gui.so or Qt5Gui.dll is part of the application dependencies),
   * all plugins from iconengines, imageformats, platforms, platforminputcontexts, etc will be deployed.
   *
   * The application may not need to support all possible platforms for example,
   * but only xcb on Linux and windows on Windows.
   *
   * To limit the set of plugins to deploy,
   * QtPluginsSet can be used.
   *
   * By default, the set is empty,
   * meaning all available plugins for the required "plugins directories" will be deployed.
   *
   * \code
   * QtPluginsSet set;
   *
   * set.setImageFormats({"jpeg","svg"});
   * set.setPlatforms({"xcb","vnc","eglfs","windows","direct2d"});
   * set.set("wayland-shell-integration", {"wl-shell"});
   * \endcode
   *
   * In above example, only jpeg and svg imageformats plugins will be deployed.
   *
   * For platform plugins,
   * we expressed plugins for Linux and Windows.
   * On Linux, only xcb, vnc and eglfs will be deployed.
   * On Windows, only windows and direct2d plugins will be deployed.
   *
   * \sa https://doc.qt.io/qt-6/plugins-howto.html
   * \sa https://doc.qt.io/qt-6/qpa.html
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtPluginsSet
  {
   public:

    /*! \brief Set the list of plugins for imageformats
     *
     * \pre \a pluginBaseNames must not be empty
     *
     * \todo define if this API is usefull or not,
     *  then also update global class doc
     */
//     void setImageFormats(const QStringList & pluginBaseNames) noexcept;

    /*! \brief Set the list of plugins for given directory name
     *
     * \pre \a directoryName must not be empty
     * \pre \a pluginBaseNames must not be empty
     */
//     void set(const std::string & directoryName, const std::vector<std::string> & pluginBaseNames) noexcept;

    /*! \brief Set the list of plugins for given directory name
     *
     * \pre \a directoryName must not be empty
     * \pre \a pluginBaseNames must not be empty
     */
    void set(const QString & directoryName, const QStringList & pluginBaseNames) noexcept;

    /*! \brief Check if this set contains given pluginBaseName for given directoryName
     */
    bool contains(const QString & directoryName, const QString & pluginBaseName) const noexcept
    {
      return mMap.value(directoryName).contains(pluginBaseName);
    }

    /*! \brief Check if given plugin should be deployed
     */
    bool shouldDeployPlugin(const QtPluginFile & plugin) const noexcept;

//     /*! \internal
//      */
//     QString toDebugString() const noexcept;

   private:

    QMap<QString, QStringList> mMap;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_PLUGINS_SET_H
