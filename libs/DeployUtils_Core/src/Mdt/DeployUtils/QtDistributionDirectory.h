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
#ifndef MDT_DEPLOY_UTILS_QT_DISTRIBUTION_DIRECTORY_H
#define MDT_DEPLOY_UTILS_QT_DISTRIBUTION_DIRECTORY_H

#include "QtConf.h"
#include "ReadQtConfError.h"
#include "OperatingSystem.h"
#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QLatin1String>
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a Qt distribution directory
   *
   * Qt can be installed by various ways, which influences where different components are installed.
   *
   * The common layout of a Qt installation is:
   * \code
   * QTDIR
   *   |-bin
   *   |  |-qt.conf
   *   |-lib
   *   |-plugins
   * \endcode
   *
   * On a Debian multi-arch system wide installation, it could be:
   * \code
   * /usr/lib/x86_64-linux-gnu
   *                  |-libQt5Core.so
   *                  |-qt5
   *                     |-plugins
   *                     |-qt.conf
   * \endcode
   *
   * Example of a Qt Conan package (Linux) provided by Conan-center:
   * \code
   * QTDIR
   *   |-bin
   *   |  |-archdatadir
   *   |  |    |-plugins
   *   |  |-qt.conf
   *   |-lib
   *   |-plugins
   * \endcode
   *
   * A reliable way to find the structure is to use the qt.conf file provided by the Qt distribution.
   * If not available, or some entries not exist in it, they can be guessed
   * by commonly known structures.
   *
   * \note It could be tempting to use QLibraryInfo to obtain the structure of the Qt distribution.
   * This will probably not work, because it will refer to the Qt library used by MdtDeployUtils itself.
   * The targeted Qt library will probably be a other one.
   *
   * \todo remove (QtDistributionDirectoryQtConf.h will probably not exist)
   * \sa QtDistributionDirectoryQtConf.h
   * 
   * \sa https://doc.qt.io/qt-6/qt-conf.html
   * 
   *
   * \todo only rel paths here. Maybe other class, like QtDistributionDirectoryPaths
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtDistributionDirectory
  {
   public:

    /*! \brief Check if this directory is null
     *
     * This directory will be null as long as:
     * - the path is not set
     *   \sa rootAbsolutePath()
     * - the shared libraries directory is not set
     *   \sa sharedLibrariesDirectoryAbsolutePath()
     *
     */
    bool isNull() const noexcept
    {
      if( !hasRootPath() ){
        return true;
      }
      if( !hasSharedLibrariesDirectory() ){
        return true;
      }

      return false;
    }

    /*! \brief Check if this directory represents a valid existing Qt distribution
     *
     * If a mandatory element is missing,
     * false is returned.
     * \sa isNull()
     *
     * If each directory that has been set exists on the file system,
     * and they are directories,
     * true is returned, otherwise false.
     */
    bool isValidExisting() const noexcept;

    /*! \brief Clear this directory
     */
    void clear() noexcept;

    /*! \brief Setup this directory from given path to a Qt shared library
     *
     * \exception ReadQtConfError
     * \pre This directory must be null before calling this method.
     * The caller must explicitly call clear() before if a new setup must be done.
     * This prevents accidental repeated setup.
     * \pre \a qtLibraryPath must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \pre \a qtLibraryPath must be a Qt shared library
     * \sa QtSharedLibraryFile::isQtSharedLibrary()
     * \pre \a os must be valid
     */
    void setupFromQtSharedLibrary(const QFileInfo & qtLibraryPath, OperatingSystem os);

    /*! \brief Set the entries present in given Qt config to this directory
     *
     * Will set directories present in \a qtConf.
     *
     * On Unix, if \a qtConf has the Libraries entry,
     * it will be used to set the shared libraries directory path.
     *
     * A qt.conf file can have a relative Prefix entry.
     * The Qt documentation is not so clear on how to interpret a relative Prefix for a Qt installation itself.
     * Looking at some installed Qt distributions,
     * it seems that the Prefix is relative to the location of the qt.conf file
     * (or it is absolute, for example on a Linux system-wide installation).
     *
     * \note This method will not check if the directories exists on the file system
     * \sa isValidExisting()
     * \pre \a os must be valid
     * \pre \a qtConfFilePath must be a absolute path
     */
    void setEntriesFromQtConf(const QtConf & qtConf, OperatingSystem os, const QFileInfo & qtConfFilePath) noexcept;

    /*! \brief Set the absolute path to the root of this directory
     *
     * \note This method will not check if \a path is a existing directory on the file system
     * \sa isValidExisting()
     * \pre \a path must be a absolute path
     */
    void setRootAbsolutePath(const QString & path) noexcept;

    /*! \brief Guess the absolute path to the root of a Qt distribution from given Qt shared library
     *
     * \pre \a qtLibraryPath must have its absolute file path set
     * \sa fileInfoIsAbsolutePath()
     * \pre \a qtLibraryPath must be a Qt shared library
     * \sa QtSharedLibraryFile::isQtSharedLibrary()
     */
    static
    QString guessRootAbsolutePathFromQtSharedLibrary(const QFileInfo & qtLibraryPath) noexcept;

    /*! \brief Check if this directory has its root path set
     */
    bool hasRootPath() const noexcept
    {
      return !mRootAbsolutePath.isEmpty();
    }

    /*! \brief Get the absolute path to the root of this directory
     */
    const QString & rootAbsolutePath() const noexcept
    {
      return mRootAbsolutePath;
    }

    /*! \brief Set the relative path to the directory for shared libraries
     *
     * \sa sharedLibrariesDirectoryAbsolutePath()
     * \note This method will not check if \a path is a existing directory on the file system
     * \sa isValidExisting()
     * \pre \a path must be a relative path
     */
    void setSharedLibrariesDirectoryRelativePath(const QString & path) noexcept;

    /*! \brief Guess the relative path to the shared libraries directory from given Qt shared library and os
     *
     * \pre \a qtLibraryPath must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \pre \a qtLibraryPath must be a Qt shared library
     * \sa QtSharedLibraryFile::isQtSharedLibrary()
     */
    static
    QString guessSharedLibrariesDirectoryRelativePathFromQtSharedLibrary(const QFileInfo & qtLibraryPath) noexcept;

    /*! \brief Check if this directory has its shared libraries directory set
     */
    bool hasSharedLibrariesDirectory() const noexcept
    {
      return !mSharedLibrariesDirectoryRelativePath.isEmpty();
    }

    /*! \brief Get the absolute path to the directory for shared libraries
     *
     * This is a sub-directory of \a rootAbsolutePath() , that depends on the targeted platform
     * (commonly bin on Windows and something like lib on other OS).
     *
     * \pre this directory must have is root path set
     * \sa hasRootPath()
     */
    QString sharedLibrariesDirectoryAbsolutePath() const noexcept;

    /*! \brief Check if this distribution contains given shared library
     *
     * \pre \a sharedLibrary must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \pre This directory must not be null
     * \sa isNull()
     */
    bool containsSharedLibrary(const QFileInfo & sharedLibraryPath) const noexcept;

    /*! \brief Set the relative path to the plugins root
     *
     * The default relative path is the plugins directory.
     *
     * \sa pluginsRootAbsolutePath()
     * \note This method will not check if \a path is a existing directory on the file system
     * \sa isValidExisting()
     * \pre \a path must be a relative path
     */
    void setPluginsRootRelativePath(const QString & path) noexcept;

    /*! \brief Get the absolute path to the plugins root
     *
     * \note Will always return a path to some sub-directory relative to rootAbsolutePath(),
     *  but it could not exist on the file system
     * \sa setPluginsRootRelativePath()
     * \sa isValidExisting()
     * \pre this directory must have is root path set
     * \sa hasRootPath()
     */
    QString pluginsRootAbsolutePath() const noexcept;

    /*! \brief Check if given file is a plugin
     *
     * Returns true if \a fileInfo is a shared library
     * located in the Qt plugins directory of this distribution
     *
     * \pre \a fileInfo must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \pre this directory must have is root path set
     * \sa hasRootPath()
     */
    bool fileIsPlugin(const QFileInfo & fileInfo) const noexcept;

    /*! \brief Find the Qt conf file from given Qt shared library
     *
     * Will search in known directories starting from the one
     * given Qt shared library lives.
     *
     * Returns the absolute path to the qt.conf file if found,
     * otherwise a empty string.
     *
     * \pre \a qtLibraryPath must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \pre \a qtLibraryPath must be a Qt shared library
     * \sa QtSharedLibraryFile::isQtSharedLibrary()
     */
    static
    QString findQtConfFileFromQtSharedLibrary(const QFileInfo & qtLibraryPath) noexcept;

   private:

    QString mRootAbsolutePath;
    QString mSharedLibrariesDirectoryRelativePath;
    QString mPluginsRootRelativePath = QLatin1String("plugins");
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_DISTRIBUTION_DIRECTORY_H
