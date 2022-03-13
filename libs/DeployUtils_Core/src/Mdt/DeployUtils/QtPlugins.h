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
#ifndef MDT_DEPLOY_UTILS_QT_PLUGINS_H
#define MDT_DEPLOY_UTILS_QT_PLUGINS_H

#include "QtPluginFile.h"
#include "DestinationDirectory.h"
#include "SharedLibrariesDeployer.h"

#include "OverwriteBehavior.h"
#include "PathList.h"

#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper class to deploy Qt plugins
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtPlugins : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a shLibDeployer must be a valid pointer
     */
    explicit QtPlugins(std::shared_ptr<SharedLibrariesDeployer> & shLibDeployer , QObject *parent = nullptr) noexcept;

    /*! \brief Set the list of shared libraries that have already been deployed
     *
     * Some Qt plugins could depend on additional shared libraries,
     * which requires to solve them.
     *
     * To avoid doing a full resolution for each plugin,
     * set the list of dependencies that have allready been deployed.
     *
     * The list can contain full paths to the shared libraries,
     * as returned by BinaryDependencies::findDependencies()
     */
    [[deprecated]]
    void setAlreadyDeployedSharedLibraries(const QStringList & libraries) noexcept;

    /*! \brief Set the search prefix path list to find shared libraries
     *
     * Some Qt plugins could depend on additional shared libraries,
     * which requires to solve them.
     *
     * Given path list will be used to find them
     */
    [[deprecated]]
    void setSearchPrefixPathList(const PathList & pathList) noexcept;

    /*! \brief Add \a libraries to the list of already solved shared libraries
     *
     * \todo This would be the responsability of BinaryDependencies
     */
    [[deprecated]]
    void addToAlreadySolvedSharedLibraries(const QStringList & libraries) noexcept;

    /*! \brief Get the list of shared libraries that have already been solved
     */
    [[deprecated]]
    const QStringList & alreadySolvedSharedLibraries() const noexcept
    {
    }

    /*! \brief Deploy given Qt plugins to given destination directory
     *
     * Each Qt plugins, as well as their dependencies,
     * will be copied respecting \a overwriteBehavior .
     *
     * \sa FileCopier::copyFile()
     * \sa setAlreadyDeployedSharedLibraries()
     * \sa setSearchPrefixPathList()
     */
    void deployQtPlugins(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior);

    /*! \brief Check if \a qtPluginsRoot could be a Qt plugins root
     */
    static
    bool pathIsAbsoluteAndCouldBePluginsRoot(const QFileInfo & qtPluginsRoot) noexcept;

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    void makeDestinationDirectoryStructure(const QStringList & qtPluginsDirectories, const DestinationDirectory & destination);
    void copyPluginsToDestination(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior);
    void copySharedLibrariesPluginsDependsOn(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior);

    std::shared_ptr<SharedLibrariesDeployer> mShLibDeployer;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_PLUGINS_H
