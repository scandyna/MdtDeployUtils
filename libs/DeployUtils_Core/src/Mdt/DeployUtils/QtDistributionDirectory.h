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
#ifndef MDT_DEPLOY_UTILS_QT_DISTRIBUTION_DIRECTORY_H
#define MDT_DEPLOY_UTILS_QT_DISTRIBUTION_DIRECTORY_H

#include "mdt_deployutilscore_export.h"
#include <QString>
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
   *                  |  |-plugins
   *                  |-qt.conf
   * \endcode
   *
   * Example of a Qt Conan package provided by Conan-center:
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

    
    /*! \brief Check if this distribution contains given shared library
     *
     * \pre \a sharedLibrary must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     */
    bool containsSharedLibrary(const QFileInfo & sharedLibraryPath) const noexcept;

    /*! \brief Get the (absolute) path to the plugins root
     */
    QString pluginsRootPath() const noexcept;

   private:

    
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_DISTRIBUTION_DIRECTORY_H
