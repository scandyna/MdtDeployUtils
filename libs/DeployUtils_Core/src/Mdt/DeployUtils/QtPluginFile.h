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
#ifndef MDT_DEPLOY_UTILS_QT_PLUGIN_FILE_H
#define MDT_DEPLOY_UTILS_QT_PLUGIN_FILE_H

#include "mdt_deployutilscore_export.h"
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Represents a Qt plugin file
   */
  class MDT_DEPLOYUTILSCORE_EXPORT QtPluginFile
  {
   public:

    /*! \brief Construct a null file
     */
    QtPluginFile() = default;

    /*! \brief Copy construct a file from \a other
     */
    QtPluginFile(const QtPluginFile & other) = default;

    /*! \brief Copy assign \a other to this file
     */
    QtPluginFile & operator=(const QtPluginFile & other) = default;

    /*! \brief Move construct a file from \a other
     */
    QtPluginFile(QtPluginFile && other) = default;

    /*! \brief Move assign \a other to this file
     */
    QtPluginFile & operator=(QtPluginFile && other) noexcept = default;

    /*! \brief Get the file info of this file
     */
    const QFileInfo & fileInfo() const noexcept
    {
      return mFile;
    }

    /*! \brief Return the Qt plugin directory name
     *
     * Qt directory name is the name of a subdirectory
     * where the plugin leaves,
     * like \a platforms or \a imageformats .
     *
     * \sa https://doc.qt.io/qt-6/plugins-howto.html
     */
    QString directoryName() const noexcept
    {
      return mFile.dir().dirName();
    }

    /*! \brief Get the absolute path of this file
     */
    QString absoluteFilePath() const noexcept
    {
      return mFile.absoluteFilePath();
    }

    /*! \brief Check if given path could be a Qt plugins directory
     *
     * As example, if the path ends with
     * \code
     * plugins/platforms
     * \endcode
     * and \a platforms is a directory,
     * this function returns true.
     *
     * If the path does not end with a directory,
     * false is returned
     * \code
     * plugins/somelib.so
     * \endcode
     *
     * If the path ends with a directory,
     * but its parent is not named \a plugins ,
     * false is returned
     * \code
     * opt/platforms
     * \endcode
     *
     */
    static
    bool pathCouldBePluginsDirectory(const QFileInfo & path) noexcept;

    /*! \brief Check if given file could be a Qt plugin file
     *
     * Returns true if \a fileInfo is a shared library
     * located in in a Qt plugins directory
     *
     * \pre \a fileInfo must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \sa pathCouldBePluginsDirectory()
     */
    static
    bool fileCouldBePlugin(const QFileInfo & fileInfo) noexcept;

    /*! \brief Construct a file from \a fileInfo
     *
     * \pre \a fileInfo must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     * \pre \a fileInfo must be a Qt plugin
     * \sa fileCouldBePlugin()
     */
    static
    QtPluginFile fromQFileInfo(const QFileInfo & fileInfo) noexcept
    {
      assert( !fileInfo.filePath().isEmpty() );
      assert( fileInfo.isAbsolute() );
      assert( fileCouldBePlugin(fileInfo) );

      return fromQFileInfoUnchecked(fileInfo);
    }

    /*! \internal Construct a file from \a fileInfo
     *
     * This is the same as fromQFileInfo() ,
     * but does not require that \a fileInfo exists on the file system.
     *
     * \pre \a fileInfo must have its absolute file path set
     * \sa doc of QFileInfo::absoluteFilePath()
     */
    static
    QtPluginFile fromQFileInfoUnchecked(const QFileInfo & fileInfo) noexcept
    {
      assert( !fileInfo.filePath().isEmpty() );
      assert( fileInfo.isAbsolute() );

      return QtPluginFile(fileInfo);
    }

   private:

    QtPluginFile(const QFileInfo & fileInfo)
     : mFile(fileInfo)
    {
    }

    QFileInfo mFile;
  };

  /*! \brief List of QtPluginFile
   */
  using QtPluginFileList = std::vector<QtPluginFile>;

  /*! \brief Get a list of Qt plugins directories present in \a qtPlugins
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QStringList getQtPluginsDirectoryNames(const QtPluginFileList & qtPlugins) noexcept;

  /*! \brief Get a list of file paths out from given Qt plugins list
   */
  MDT_DEPLOYUTILSCORE_EXPORT
  QStringList getQtPluginsFilePathList(const QtPluginFileList & qtPlugins) noexcept;

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_QT_PLUGIN_FILE_H
