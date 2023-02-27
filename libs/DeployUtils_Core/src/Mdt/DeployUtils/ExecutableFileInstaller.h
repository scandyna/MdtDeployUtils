// SPDX-License-Identifier: LGPL-3.0-or-later
/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2023-2023 Philippe Steinmann.
 **
 ****************************************************************************/
#ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_INSTALLER_H
#define MDT_DEPLOY_UTILS_EXECUTABLE_FILE_INSTALLER_H

#include "ExecutableFileToInstall.h"
#include "CopiedExecutableFile.h"
#include "OverwriteBehavior.h"
#include "Platform.h"
#include "RPath.h"
#include "PathList.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper class to install executables and shared libraries
   */
  class MDT_DEPLOYUTILSCORE_EXPORT ExecutableFileInstaller : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     *
     * \pre \a platform must not be null
     */
    explicit ExecutableFileInstaller(const Platform & platform, QObject *parent = nullptr) noexcept;

    /*! \brief Set the overwrite behaviour
     *
     * By default, the \a overwriteBehavior is OverwriteBehavior::Fail.
     *
     * \sa FileCopier::copyFile()
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

    /*! \brief Install given file to given directory
     *
     * If given directory does not exist it will be created.
     * \sa FileCopier::createDirectory()
     *
     * If the source and destination locations for a file are the same,
     * it will not be copied, and its rpath informations will not be changed at all,
     * and this regardless of overwrite behaviour
     * (The only descent case this happens is a UNIX system wide installation,
     * for a target that depends on shared libraries allready installed system wide).
     *
     * Otherwise given file is copied to given directory respecting overwrite behaviour.
     * \sa setOverwriteBehavior()
     * \sa FileCopier::copyFile()
     *
     * For platform that supports it,
     * rapth will be updated, if required, to given \a installRPath .
     *
     * \pre \a directoryPath must be a absolute path
     */
    void install(const ExecutableFileToInstall & file, const QFileInfo & directoryPath, const RPath & installRPath);

    /*! \internal Check if given Rpath has to be changed for given file
     *
     * \sa https://gitlab.com/scandyna/mdtdeployutils/-/issues/3
     */
    bool hasToUpdateRpath(const CopiedExecutableFile & file, const RPath & rpath, const PathList & systemWideLocations) const noexcept;

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    void readRPathFromSourceFile(CopiedExecutableFile & file);

    /*! \brief Set given rpath to given copied files
     *
     * \pre current platform must support RPath
     */
    void setRPathToCopiedFiles(const CopiedExecutableFileList & copiedFiles, const RPath & rpath);

    OverwriteBehavior mOverwriteBehavior = OverwriteBehavior::Fail;
    Platform mPlatform;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_INSTALLER_H
