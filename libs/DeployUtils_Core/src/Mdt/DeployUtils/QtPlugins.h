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
#include "Platform.h"
#include "OverwriteBehavior.h"
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

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    void makeDestinationDirectoryStructure(const QStringList & qtPluginsDirectories, const DestinationDirectory & destination);
    CopiedSharedLibraryFileList copyPluginsToDestination(const QtPluginFileList & plugins,
                                                         const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior);
    void copySharedLibrariesPluginsDependsOn(const QtPluginFileList & plugins, const DestinationDirectory & destination);
    void setRPathToCopiedPlugins(const CopiedSharedLibraryFileList & copiedPlugins, const DestinationDirectory & destination);

    std::shared_ptr<SharedLibrariesDeployer> mShLibDeployer;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_PLUGINS_H
