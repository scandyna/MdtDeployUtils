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
#ifndef MDT_DEPLOY_UTILS_DEPLOY_APPLICATION_H
#define MDT_DEPLOY_UTILS_DEPLOY_APPLICATION_H

#include "DeployApplicationRequest.h"
#include "DeployApplicationError.h"
#include "FindDependencyError.h"
#include "OperatingSystem.h"
#include "Platform.h"
#include "DestinationDirectory.h"
#include "OverwriteBehavior.h"
#include "SharedLibrariesDeployer.h"
#include "QtDistributionDirectory.h"
#include "QtPluginFile.h"
#include "DestinationDirectoryStructure.h"
#include "BinaryDependenciesResult.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Deploy a application to a destination directory
   *
   * Some variables that figure here are taken from DeployApplicationRequest .
   *
   * If the executable, defined by \a targetFilePath ,
   * allready exists in the destination directory, it will be replaced.
   *
   * The rules to overwrite existing shared libraries are influenced by \a shLibOverwriteBehavior .
   * \sa CopySharedLibrariesTargetDependsOn
   *
   * By default, on platform that supports rpath,
   * the rpath informations of the copied executable is set
   * to the relative location of the installed shared libraries
   * (i.e. $ORIGIN/../lib for ELF files on platforms like Linux).
   * If \a removeRpath is true, the rpath informations are removed for the copied executable.
   *
   * The rules for rpath informations is similar for copied shared libraries.
   * \sa CopySharedLibrariesTargetDependsOn
   *
   * To find dependencies, \a searchPrefixPathList will be used.
   * \sa BinaryDependencies
   * \sa CopySharedLibrariesTargetDependsOn
   *
   * \todo document the diretctory structure
   */
  class MDT_DEPLOYUTILSCORE_EXPORT DeployApplication : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit DeployApplication(QObject *parent = nullptr);

    /*! \brief Deploy a application to a destination directory
     *
     * \pre request's \a targetFilePath must be specified
     * \pre request's \a destinationDirectoryPath must be specified
     * \pre request's \a runtimeDestination must be specified
     * \pre request's \a libraryDestination must be specified
     * \exception DeployApplicationError
     */
    void execute(const DeployApplicationRequest & request);

    /*! \internal Get the destination directory structure from given runtime and library destination
     *
     * \pre request's \a runtimeDestination must be specified
     * \pre request's \a libraryDestination must be specified
     * \pre \a os must be defined
     */
    static
    DestinationDirectoryStructure
    destinationDirectoryStructureFromRuntimeAndLibraryDestination(const DeployApplicationRequest & request, OperatingSystem os) noexcept;

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    QString getMissingLibrariesListText(const BinaryDependenciesResult & result) const noexcept;
    void throwApplicationDependenciesNotSolvedError(const BinaryDependenciesResult & result) const;
    void throwQtPluginsDependenciesNotSolvedError(const BinaryDependenciesResultList & resultList) const;

    void setupShLibDeployer(const DeployApplicationRequest & request);
    void makeDirectoryStructure(const DestinationDirectory & destination);
    void installExecutable(const DeployApplicationRequest & request, const DestinationDirectoryStructure & destinationStructure);

    void installSharedLibraries(const BinaryDependenciesResultList & libraries);

    QtPluginFileList getRequiredQtPlugins(const BinaryDependenciesResult & libraries, const DeployApplicationRequest & request);
    BinaryDependenciesResultList findSharedLibrariesQtPluginsDependsOn(const QtPluginFileList & plugins);

    void installQtPlugins(const QtPluginFileList & plugins, const DestinationDirectory & destination, OverwriteBehavior overwriteBehavior);

    void writeQtConfFile(const DestinationDirectory & destination);

    static
    QString osName(OperatingSystem os) noexcept;

    Platform mPlatform;
    QString mBinDirDestinationPath;
    QString mLibDirDestinationPath;
    std::shared_ptr<QtDistributionDirectory> mQtDistributionDirectory;
    std::shared_ptr<SharedLibrariesDeployer> mShLibDeployer;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_DEPLOY_APPLICATION_H
