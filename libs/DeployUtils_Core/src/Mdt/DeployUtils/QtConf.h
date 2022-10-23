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
#ifndef MDT_DEPLOY_UTILS_QT_CONF_H
#define MDT_DEPLOY_UTILS_QT_CONF_H

#include "mdt_deployutilscore_export.h"
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Settings of a qt.conf file
   *
   * \sa https://doc.qt.io/qt-6/qt-conf.html
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtConf
  {
   public:

    /*! \brief Set the Prefix path
     *
     * \note \a path should be a relative path,
     *  so the distributed application is relocatable
     * \pre \a path must not be empty
     */
    void setPrefixPath(const QString & path) noexcept;

    /*! \brief Check if this config contains a Prefix path
     */
    bool containsPrefixPath() const noexcept
    {
      return !mPrefixPath.isEmpty();
    }

    /*! \brief Check if the prefix path of this config is absolute
     */
    bool prefixPathIsAbsolute() const noexcept;

    /*! \brief Get the Prefix path
     */
    const QString & prefixPath() const noexcept
    {
      return mPrefixPath;
    }

    /*! \brief Set the path to the libraries
     *
     * \sa librariesPath()
     * \pre \a path must not be empty
     */
    void setLibrariesPath(const QString & path) noexcept;

    /*! \brief Check if this config has a Libraries path
     *
     * \sa librariesPath()
     */
    bool containsLibrariesPath() const noexcept
    {
      return !mLibrariesPath.isEmpty();
    }

    /*! \brief Get the Libraries path
     *
     * \note This is not always the shared libraries path
     *  (on Windows, shared libraries are probably installed to the Binaries path)
     */
    const QString & librariesPath() const noexcept
    {
      return mLibrariesPath;
    }

    /*! \brief Set the Plugins path
     *
     * \pre \a path must not be empty
     */
    void setPluginsPath(const QString & path) noexcept;

    /*! \brief Check if this config has a Plugins path
     */
    bool containsPluginsPath() const noexcept
    {
      return !mPluginsPath.isEmpty();
    }

    /*! \brief Get the Plugins path
     */
    const QString & pluginsPath() const noexcept
    {
      return mPluginsPath;
    }

   private:

    QString mPrefixPath;
    QString mLibrariesPath;
    QString mPluginsPath;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_CONF_H
