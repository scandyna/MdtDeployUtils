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
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
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

    /*! \brief Deploy given Qt plugins to given destination directory
     */
    void deployQtPlugins(const QtPluginFileList & plugins, const DestinationDirectory & destination);

    /*! \brief Check if \a qtPluginsRoot could be a Qt plugins root
     */
    static
    bool pathIsAbsoluteAndCouldBePluginsRoot(const QFileInfo & qtPluginsRoot) noexcept;

   signals:

    void statusMessage(const QString & message) const;
    void verboseMessage(const QString & message) const;
    void debugMessage(const QString & message) const;

   private:

    void makeDestinationDirectoryStructure(const QStringList & qtPluginsDirectories, const DestinationDirectory & destination);
    void copyPluginsToDestination(const QtPluginFileList & plugins, const DestinationDirectory & destination);
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_PLUGINS_H
