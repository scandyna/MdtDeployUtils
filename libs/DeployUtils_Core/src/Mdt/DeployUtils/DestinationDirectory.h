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
#ifndef MDT_DEPLOY_UTILS_DESTINATION_DIRECTORY_H
#define MDT_DEPLOY_UTILS_DESTINATION_DIRECTORY_H

#include "DestinationDirectoryStructure.h"
#include "OperatingSystem.h"
#include "mdt_deployutilscore_export.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a destination directory
   */
  class MDT_DEPLOYUTILSCORE_EXPORT DestinationDirectory
  {
   public:

    DestinationDirectory() = delete;

    /*! \brief Copy construct a directory from \a other
     */
    DestinationDirectory(const DestinationDirectory & other) = default;

    /*! \brief Copy assign \a other to this directory
     */
    DestinationDirectory & operator=(const DestinationDirectory & other) = default;

    /*! \brief Move construct a directory from \a other
     */
    DestinationDirectory(DestinationDirectory && other) noexcept = default;

    /*! \brief Move assign \a other to this directory
     */
    DestinationDirectory & operator=(DestinationDirectory && other) noexcept = default;

    /*! \brief Get the path to this directory
     */
    const QString & path() const noexcept
    {
      return mPath;
    }

    /*! \brief Get the path to the directory for executables
     */
    QString executablesDirectoryPath() const noexcept;

    /*! \brief Get the path to the directory for shared libraries
     */
    QString sharedLibrariesDirectoryPath() const noexcept;

    /*! \brief Get the path to the Qt plugins root directory
     */
    QString qtPluginsRootDirectoryPath() const noexcept;

    /*! \brief Access the structure of this directory
     */
    const DestinationDirectoryStructure & structure() const noexcept
    {
      return mStructure;
    }

    /*! \brief Create a destination directory from given path and structure
     *
     * \pre \a path must not be empty
     * \pre \a structure must not be null
     */
    static
    DestinationDirectory fromPathAndStructure(const QString & path, const DestinationDirectoryStructure & structure) noexcept;

    /*! \brief Create a destination directory from given path and OS
     *
     * \pre \a path must not be empty
     * \pre \a os must be defined
     */
    static
    DestinationDirectory fromPathAndOs(const QString & path, OperatingSystem os) noexcept;

   private:

    DestinationDirectory(const QString & path, const DestinationDirectoryStructure & structure) noexcept
     : mPath(path),
       mStructure(structure)
    {
    }

    QString mPath;
    DestinationDirectoryStructure mStructure;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_DESTINATION_DIRECTORY_H
