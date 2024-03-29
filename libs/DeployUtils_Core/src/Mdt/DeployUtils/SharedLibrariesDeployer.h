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
#ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARIES_DEPLOYER_H
#define MDT_DEPLOY_UTILS_SHARED_LIBRARIES_DEPLOYER_H

#include "PathList.h"
#include "FileCopierFile.h"
#include "CompilerLocationRequest.h"
#include "OverwriteBehavior.h"
#include "Platform.h"
#include "BinaryDependencies.h"
#include "BinaryDependenciesResult.h"
#include "BinaryDependenciesResultList.h"
#include "QtDistributionDirectory.h"
#include "RPath.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QFileInfoList>
#include <memory>
#include <vector>

namespace Mdt{ namespace DeployUtils{

  /*! \internal
   */
  struct CopiedSharedLibraryFile
  {
    FileCopierFile file;
    RPath rpath;
  };

  /*! \internal
   */
  using CopiedSharedLibraryFileList = std::vector<CopiedSharedLibraryFile>;

  /*! \brief Deploy a set of shared libraries some target depends on
   *
   * If the source and destination locations for a shared library are the same,
   * it will not be copied, and its rpath informations will not be changed at all,
   * and this regardless of \a removeRpath and \a overwriteBehavior .
   * (The only descent case this happens is a UNIX system wide installation,
   * for a target that depends on shared libraries allready installed system wide).
   *
   * To find dependencies, \a searchPrefixPathList will be used.
   *
   * \sa setOverwriteBehavior()
   * \sa setRemoveRpath()
   * \sa setSearchPrefixPathList()
   * \sa BinaryDependencies
   */
  class MDT_DEPLOYUTILSCORE_EXPORT SharedLibrariesDeployer : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a qtDistributionDirectory must be a valid pointer
     */
    explicit SharedLibrariesDeployer(std::shared_ptr<QtDistributionDirectory> & qtDistributionDirectory,
                                     QObject *parent = nullptr) noexcept;

    /*! \brief Set the list of paths prefixes where to locate the shared libraries
     */
    void setSearchPrefixPathList(const PathList & pathList) noexcept;

    /*! \brief Get the list of paths prefixes where to locate the shared libraries
     *
     * \sa setSearchPrefixPathList()
     */
    const PathList & searchPrefixPathList() const noexcept
    {
      return mSearchPrefixPathList;
    }

    /*! \brief Set the compiler location
     *
     * Some compiler specific shared libraries could be necessary to be copied.
     * Those libraries are probably provided in the installation of the compiler
     * used to build the project.
     *
     * To find those libraries, the locations of the used compiler should be provided here.
     *
     * \a location is composed on a type and a value:
     * - CompilerLocationType::FromEnv: the value is the name of a environment variable, like VcInstallDir .
     * - CompilerLocationType::CompilerPath: the value is the full path to the compiler. This can be used with CMAKE_CXX_COMPILER .
     *
     * \note This is mostly required for MSVC
     * \pre \a location must not be null
     * \exception FindCompilerError
     * \sa CompilerFinder
     */
    void setCompilerLocation(const CompilerLocationRequest & location);

    /*! \brief Set the overwrite behaviour
     *
     * If a shared library allready exists at the destination location,
     * but its not the same as the source (source and destination locations are different),
     * the behavior is defined by \a overwriteBehavior .
     *
     * - If \a overwriteBehavior is OverwriteBehavior::Keep, the destination library will not be changed at all.
     * - If \a overwriteBehavior is OverwriteBehavior::Overwrite, the destination library will replaced.
     * - If \a overwriteBehavior is OverwriteBehavior::Fail, a fatal error is thrown.
     *
     * By default, the \a overwriteBehavior is OverwriteBehavior::Fail.
     */
    void setOverwriteBehavior(OverwriteBehavior overwriteBehavior) noexcept;

    /*! \brief Get the overwrite behaviour
     *
     * \sa setOverwriteBehavior()
     */
    OverwriteBehavior overwriteBehavior() const noexcept
    {
      return mOverwriteBehavior;
    }

    /*! \brief Set the Rpath removal
     *
     * By default, on platform that supports rpath,
     * the rpath informations is set to the relative location of the shared library
     * (i.e. $ORIGIN for ELF files on platforms like Linux) for each one that has been copied.
     * If \a remove is true, the rpath informations are removed for each shared library that has been copied.
     */
    void setRemoveRpath(bool remove) noexcept;

    /*! \brief Get the Rpath removal
     *
     * \todo rename to something like removeRpathIsSet()
     * \sa setRemoveRpath()
     */
    bool removeRpath() const noexcept
    {
      return mRemoveRpath;
    }

    /*! \brief Check if given Rpath has to be changed for given file
     *
     * \sa https://gitlab.com/scandyna/mdtdeployutils/-/issues/3
     */
    //[[deprecated]]
    bool hasToUpdateRpath(const CopiedSharedLibraryFile & file, const RPath & rpath, const PathList & systemWideLocations) const noexcept;

    /*! \brief Get a list of shared libraries given target depends on
     *
     * \pre \a target must be a absolute file path
     *
     * \exception FindCompilerError
     * \exception FileOpenError
     * \exception ExecutableFileReadError
     */
    BinaryDependenciesResult findSharedLibrariesTargetDependsOn(const QFileInfo & target);

    /*! \brief Get a list of shared libraries given targets depends on
     *
     * \pre \a targets must not be a empty list
     * \pre each target in \a targets must be a absolute file path
     *
     * \exception FindCompilerError
     * \exception FileOpenError
     * \exception ExecutableFileReadError
     */
    BinaryDependenciesResultList findSharedLibrariesTargetsDependsOn(const QFileInfoList & targets);

    /*! \brief Install shared libraries to given destination
     *
     * \pre \a libraries must be a solved result
     * \pre \a destinationDirectoryPath must not be empty
     * \exception FileCopyError
     * \sa setOverwriteBehavior()
     * \sa setRemoveRpath()
     */
    void installSharedLibraries(const BinaryDependenciesResultList & libraries, const QString & destinationDirectoryPath);

    /*! \brief Copy a set of shared libraries to given destination
     *
     * \exception FileCopyError
     */
    CopiedSharedLibraryFileList copySharedLibraries(const BinaryDependenciesResultList & resultList, const QString & destinationDirectoryPath);

    /*! \brief Copy shared libraries given target depends on to given destination
     *
     * \pre \a targetFilePath must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \pre \a destinationDirectoryPath must be a absolute path
     *
     * \exception FindCompilerError
     * \exception FileOpenError
     * \exception ExecutableFileReadError
     * \exception FindDependencyError
     * \exception FileCopyError
     * \exception ExecutableFileWriteError
     *
     * \sa copySharedLibrariesTargetsDependsOn()
     */
    void copySharedLibrariesTargetDependsOn(const QFileInfo & targetFilePath, const QString & destinationDirectoryPath);

    /*! \brief Set given rpath to given copied shared libraries
     *
     * \pre current platform must support RPath
     */
    void setRPathToCopiedSharedLibraries(const CopiedSharedLibraryFileList & copiedFiles, const RPath & rpath);

    /*! \brief Get the current platform
     *
     * The current platform is null until
     * copySharedLibrariesTargetDependsOn()
     * or
     * copySharedLibrariesTargetsDependsOn()
     * have been called.
     */
    const Platform & currentPlatform() const noexcept
    {
      return mPlatform;
    }

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    void setCurrentPlatformFromFile(const QFileInfo & file);
    void copySharedLibrariesTargetsDependsOnImpl(const QFileInfoList & targetFilePathList, const QString & destinationDirectoryPath);
    void setRPathToCopiedDependencies(const CopiedSharedLibraryFileList & copiedFiles);
    void emitStartMessage(const QFileInfo & target) const noexcept;
    void emitStartMessage(const QFileInfoList & targetFilePathList) const;
    void emitSearchPrefixPathListMessage() const;
    void emitResultMessages(const BinaryDependenciesResult & result) const noexcept;
    void emitFoundDependenciesMessage(const BinaryDependenciesResult & result) const noexcept;
    void emitFoundDependenciesMessage(const BinaryDependenciesResultList & resultList) const noexcept;

    static
    QString overwriteBehaviorToString(OverwriteBehavior overwriteBehavior) noexcept;

    OverwriteBehavior mOverwriteBehavior = OverwriteBehavior::Fail;
    bool mRemoveRpath = false;
    PathList mSearchPrefixPathList;
    BinaryDependencies mBinaryDependencies;
    Platform mPlatform;
    std::shared_ptr<QtDistributionDirectory> mQtDistributionDirectory;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_SHARED_LIBRARIES_DEPLOYER_H
