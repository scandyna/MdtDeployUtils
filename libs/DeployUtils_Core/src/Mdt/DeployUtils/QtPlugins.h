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

#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper class to deploy Qt plugins
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtPlugins : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit QtPlugins(QObject *parent = nullptr) noexcept
     : QObject(parent)
    {
    }

    /*! \brief Set the destination qt plugins root
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
     * that must match the setting in this file.
     *
     * The given path must be a relative path,
     * that will be added to the application destination directory path.
     *
     * \pre \a path must be relative
     * \sa https://doc.qt.io/qt-6/deployment-plugins.html
     * \sa https://doc.qt.io/qt-6/qt-conf.html
     */
    void setDestinationQtPluginsRootRelativePath(const QString & path) noexcept;

    /*! \brief Get the destination qt plugins root
     *
     * \sa setDestinationQtPluginsRootRelativePath()
     */
    const QString & destinationQtPluginsRootRelativePath() const noexcept
    {
    }

    /*! \brief Check if \a qtPluginsRoot could be a Qt plugins root
     */
    static
    bool pathIsAbsoluteAndCouldBePluginsRoot(const QFileInfo & qtPluginsRoot) noexcept;

  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_PLUGINS_H
