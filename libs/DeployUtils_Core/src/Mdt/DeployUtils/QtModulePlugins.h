/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2022-2023 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_QT_MODULE_PLUGINS_H
#define MDT_DEPLOY_UTILS_QT_MODULE_PLUGINS_H

#include "QtModule.h"
#include "QtModuleList.h"
#include "QtSharedLibraryFile.h"
#include "QtPluginFile.h"
#include "QtPluginsSet.h"
#include "FindQtPluginError.h"
#include "QtDistributionDirectory.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper class for Qt plugins associated with a Qt module
   *
   * \todo see various todo in the CPP file
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtModulePlugins : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit QtModulePlugins(QObject *parent = nullptr) noexcept
     : QObject(parent)
    {
    }

    /*! \brief Get Qt plugins required for given list of Qt libraries
     *
     * If \a qtLibraries is empty,
     * a empty plugins list is returned.
     *
     * \pre If \a qtLibraries is not empty,
     * \a qtDistributionDirectory must be valid and existing
     * \sa QtDistributionDirectory::isValidExisting()
     */
    QtPluginFileList getQtPluginsQtLibrariesDependsOn(const QtSharedLibraryFileList & qtLibraries,
                                                      const QtPluginsSet & pluginsSet,
                                                      const QtDistributionDirectory & qtDistributionDirectory) const noexcept;

    /*! \brief Get a list of Qt plugins directories required for a given Qt module
     *
     * \note this function returns a list defined in a map,
     * and can also return directories that are not present.
     *
     * \sa getExistingPluginsDirectoriesForModule()
     * \sa https://doc.qt.io/qt-6/plugins-howto.html
     * \sa https://doc.qt.io/qt-6/qtvirtualkeyboard-deployment-guide.html
     */
    static
    QStringList getPluginsDirectoriesForModule(QtModule module) noexcept;

    /*! \brief Get a list of existing Qt plugins directories required for a given Qt module
     *
     * This is like getPluginsDirectoriesForModule(),
     * but it will only include directories that exists in \a qtDistributionDirectory
     *
     * \pre \a qtDistributionDirectory must be valid and existing
     * \sa QtDistributionDirectory::isValidExisting()
     */
    static
    QStringList getExistingPluginsDirectoriesForModule(QtModule module, const QtDistributionDirectory & qtDistributionDirectory) noexcept;

    /*! \brief Get a list of Qt plugins directories required for given Qt modules
     *
     * \sa getPluginsDirectoriesForModule()
     */
    static
    QStringList getPluginsDirectoriesForModules(const QtModuleList & modules) noexcept;

    /*! \brief Get a list of Qt plugins directories required for given Qt modules
     *
     * \pre \a qtDistributionDirectory must be valid and existing
     * \sa QtDistributionDirectory::isValidExisting()
     *
     * \sa getExistingPluginsDirectoriesForModule()
     */
    static
    QStringList getExistingPluginsDirectoriesForModules(const QtModuleList & modules, const QtDistributionDirectory & qtDistributionDirectory) noexcept;

    /*! \brief Get a list of Qt plugins required for given Qt modules
     *
     * \pre \a qtDistributionDirectory must be valid and existing
     * \sa QtDistributionDirectory::isValidExisting()
     *
     * \sa https://doc.qt.io/qt-6/plugins-howto.html
     * \sa https://doc.qt.io/qt-6/deployment-plugins.html
     * \sa https://doc.qt.io/qt-6/deployment.html
     * \sa https://doc.qt.io/qt-6/qpa.html
     * \sa https://doc.qt.io/qt-6/windows-deployment.html
     */
    QtPluginFileList getPluginsForModules(const QtModuleList & modules, const QtDistributionDirectory & qtDistributionDirectory,
                                          const QtPluginsSet & pluginsSet) const noexcept;

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_MODULE_PLUGINS_H
