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
#ifndef MDT_DEPLOY_UTILS_QT_MODULE_PLUGINS_H
#define MDT_DEPLOY_UTILS_QT_MODULE_PLUGINS_H

#include "QtModule.h"
#include "QtModuleList.h"
#include "QtSharedLibraryFile.h"
#include "QtPluginFile.h"
#include "FindQtPluginError.h"
#include "DestinationDirectory.h"
#include "OverwriteBehavior.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Helper class for Qt plugins associated with a Qt module
   *
   * \todo see various todo in the CPP file
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtModulePlugins : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Constructor
     */
    explicit QtModulePlugins(QObject *parent = nullptr) noexcept
     : QObject(parent)
    {
    }

    /*! \brief Get Qt plugins required for given list of Qt libraries
     */
    QtPluginFileList getQtPluginsQtLibrariesDependsOn(const QtSharedLibraryFileList & qtLibraries) const noexcept;

    /*! \brief Get a list of Qt plugins directories required for a given Qt module
     *
     * \note this function returns a list defined in a map,
     * and can also return directories that are not present.
     *
     * \sa getExistingPluginsDirectoriesForModule()
     * \sa https://doc.qt.io/qt-6/plugins-howto.html
     * \sa https://doc.qt.io/qt-6/qtvirtualkeyboard-deployment-guide.html
     */
    static
    QStringList getPluginsDirectoriesForModule(QtModule module) noexcept;

    /*! \brief Get a list of existing Qt plugins directories required for a given Qt module
     *
     * This is like getPluginsDirectoriesForModule(),
     * but it will only include directories that exists in \a qtPluginsRoot
     *
     * \pre \a qtPluginsRoot must be a absolute path to a existing Qt plugins directory
     * \sa pathIsAbsoluteAndCouldBePluginsRoot()
     */
    static
    QStringList getExistingPluginsDirectoriesForModule(QtModule module, const QFileInfo & qtPluginsRoot) noexcept;

    /*! \brief Get a list of Qt plugins directories required for given Qt modules
     *
     * \sa getPluginsDirectoriesForModule()
     */
    static
    QStringList getPluginsDirectoriesForModules(const QtModuleList & modules) noexcept;

    /*! \brief Get a list of Qt plugins directories required for given Qt modules
     *
     * \pre \a qtPluginsRoot must be a absolute path to a existing Qt plugins directory
     * \sa pathIsAbsoluteAndCouldBePluginsRoot()
     *
     * \sa getExistingPluginsDirectoriesForModule()
     */
    static
    QStringList getExistingPluginsDirectoriesForModules(const QtModuleList & modules, const QFileInfo & qtPluginsRoot) noexcept;

    /*! \brief Get a list of Qt plugins required for given Qt modules
     *
     * \note current implementation simply returns all the plugins
     *  available in the required sub-directories
     *  (like platforms, printsupport, ...)
     *  for given modules.
     * In future implementation,
     * the set of plugins to include should be
     * defined by some set.
     *
     * \pre \a qtPluginsRoot must be a absolute path to a existing Qt plugins directory
     * \sa pathIsAbsoluteAndCouldBePluginsRoot()
     *
     * \sa https://doc.qt.io/qt-6/plugins-howto.html
     * \sa https://doc.qt.io/qt-6/deployment-plugins.html
     * \sa https://doc.qt.io/qt-6/deployment.html
     * \sa https://doc.qt.io/qt-6/qpa.html
     * \sa https://doc.qt.io/qt-6/windows-deployment.html
     */
    QtPluginFileList getPluginsForModules(const QtModuleList & modules, const QFileInfo & qtPluginsRoot) const noexcept;

    /*! \brief Check if \a qtPluginsRoot could be a Qt plugins root
     *
     * \sa QtPlugins::pathIsAbsoluteAndCouldBePluginsRoot()
     */
    static
    bool pathIsAbsoluteAndCouldBePluginsRoot(const QFileInfo & qtPluginsRoot) noexcept;

    /*! \brief Find the path to the plugins root of a Qt installation given \a qtLibraryPath
     *
     * The common layout of a Qt installation is:
     * \code
     * QTDIR
     *   |-bin
     *   |-lib
     *   |-plugins
     * \endcode
     *
     * On a Debian multi-arch system wide installation, it could be:
     * \code
     * /usr/lib/x86_64-linux-gnu
     *                  |-qt5
     *                     |-plugins
     * \endcode
     *
     * \pre \a qtLibraryPath must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \exception FindQtPluginError
     * \sa findPluginsRootFromQtLibrary()
     */
    static
    QString findPluginsRootFromQtLibraryPath(const QFileInfo & qtLibraryPath);

    /*! \brief Get the path to the plugins root of a Qt installation given \a qtLibrary
     *
     * \exception FindQtPluginError
     * \sa findPluginsRootFromQtLibraryPath()
     */
    static
    QString findPluginsRootFromQtLibrary(const QtSharedLibraryFile & qtLibrary);

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_MODULE_PLUGINS_H
