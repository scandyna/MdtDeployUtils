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
#ifndef MDT_DEPLOY_UTILS_DESTINATION_DIRECTORY_STRUCTURE_H
#define MDT_DEPLOY_UTILS_DESTINATION_DIRECTORY_STRUCTURE_H

#include "OperatingSystem.h"
#include "mdt_deployutilscore_export.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a destination directory structure
   */
  class MDT_DEPLOYUTILSCORE_EXPORT DestinationDirectoryStructure
  {
   public:

    /*! \brief Construct a null directory structure
     *
     * \sa isNull()
     */
    DestinationDirectoryStructure() noexcept = default;

    /*! \brief Copy construct a directory structure from \a other
     */
    DestinationDirectoryStructure(const DestinationDirectoryStructure & other) = default;

    /*! \brief Copy assign \a other to this directory structure
     */
    DestinationDirectoryStructure & operator=(const DestinationDirectoryStructure & other) = default;

    /*! \brief Move construct a directory structure from \a other
     */
    DestinationDirectoryStructure(DestinationDirectoryStructure && other) noexcept = default;

    /*! \brief Move assign \a other to this directory structure
     */
    DestinationDirectoryStructure & operator=(DestinationDirectoryStructure && other) noexcept = default;

    /*! \brief Check if this directory structure is null
     *
     * This structure will be null as long as the directories
     * to executables and shared libraries are not defined
     */
    bool isNull() const noexcept
    {
      if( mExecutablesDirectory.isEmpty() ){
        return true;
      }
      if( mSharedLibrariesDirectory.isEmpty() ){
        return true;
      }

      return false;
    }

    /*! \brief Set the directory for executables
     *
     * \pre \a directory must not be empty
     * \pre \a directory must not be a absolute path
     */
    void setExecutablesDirectory(const QString & directory) noexcept;

    /*! \brief Get the directory for executables
     */
    const QString & executablesDirectory() const noexcept
    {
      return mExecutablesDirectory;
    }

    /*! \brief Set the directory for shared libraries
     *
     * \pre \a directory must not be empty
     * \pre \a directory must not be a absolute path
     */
    void setSharedLibrariesDirectory(const QString & directory) noexcept;

    /*! \brief Get the directory for shared libraries
     */
    const QString & sharedLibrariesDirectory() const noexcept
    {
      return mSharedLibrariesDirectory;
    }

    /*! \brief Set the qt plugins root
     *
     * Qt plugins must be deployed in some specific directories,
     * for example:
     * \code
     * destinationRoot
     *     |-bin
     *        |-app.exe
     *        |-platforms
     * \endcode
     *
     * This seems also to work on Linux:
     * \code
     * destinationRoot
     *     |-bin
     *        |-app
     *     |-lib
     *     |-plugins
     *        |-platforms
     * \endcode
     *
     * If using a qt.conf file, this can be set to a other directory,
     * that must match the setting in the file.
     *
     * \pre \a path must be relative
     * \sa https://doc.qt.io/qt-6/deployment-plugins.html
     * \sa https://doc.qt.io/qt-6/qt-conf.html
     * \sa setQtConfPathEntries()
     */
    void setQtPluginsRootDirectory(const QString & path) noexcept;

    /*! \brief Get the qt plugins root
     *
     * \sa setQtPluginsRootRelativePath()
     */
    const QString & qtPluginsRootDirectory() const noexcept
    {
      return mQtPluginsRootRelativePath;
    }

    /*! \brief Get the relative path from Qt plugins to the shared libraries directory
     *
     * This can be used to set the Rpath to Qt plugins.
     *
     * As example:
     * \code
     * destinationRoot
     *     |-bin
     *     |-lib
     *     |-plugins
     *        |-platforms
     * \endcode
     * for this structure,
     * ../../lib will be returned
     *
     * \pre the shared libraries directory must be set
     * \pre the Qt plugins root must be set
     * \sa setSharedLibrariesDirectory()
     * \sa setQtPluginsRootDirectory()
     */
    QString qtPluginsToSharedLibrariesRelativePath() const noexcept;

    /*! \brief Get the relative path from executable directory to shared libraries directory
     *
     * This can be used to set Rpath to executables
     *
     * As example:
     * \code
     * destinationRoot
     *     |-bin
     *     |-lib
     * \endcode
     * for this structure,
     * ../lib will be returned
     *
     * \pre the shared libraries directory must be set
     * \pre the executables directory must be set
     * \sa setSharedLibrariesDirectory()
     * \sa setExecutablesDirectory()
     */
    QString executablesToSharedLibrariesRelativePath() const noexcept;

    /*! \brief Get the relative path from executable directory to the root of this structure
     *
     * Example:
     * \code
     * destinationRoot
     *     |-bin
     * \endcode
     * for this structure,
     * .. will be returned
     *
     * \pre the executables directory must be set
     * \sa setExecutablesDirectory()
     */
    QString executablesToRootRelativePath() const noexcept;

    /*! \brief Get the (default) directory for executables for given OS
     *
     * \pre \a os must be defined
     */
    static
    QString executablesDirectoryFromOs(OperatingSystem os) noexcept;

    /*! \brief Get the (default) directory for shared libraries for given OS
     *
     * \pre \a os must be defined
     */
    static
    QString sharedLibrariesDirectoryFromOs(OperatingSystem os) noexcept;

    /*! \brief Get the (default) Qt plugins root for given OS
     *
     * \pre \a os must be defined
     */
    static
    QString qtPluginsRootDirectoryFromOs(OperatingSystem os) noexcept;

    /*! \brief Create a directory structure for given OS
     *
     * \pre \a os must be defined
     */
    static
    DestinationDirectoryStructure fromOperatingSystem(OperatingSystem os) noexcept;

   private:

    static
    QString directoryAtoBrelativePath(const QString & a, const QString & b, int aAdditionalLevel = 0) noexcept;

    QString mExecutablesDirectory;
    QString mSharedLibrariesDirectory;
    QString mQtPluginsRootRelativePath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_DESTINATION_DIRECTORY_STRUCTURE_H
