/****************************************************************************
 **
 ** MdtDeployUtils - A C++ library to help deploy C++ compiled binaries
 **
 ** Copyright (C) 2021-2021 Philippe Steinmann.
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
#ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_IO_ENGINE_H
#define MDT_DEPLOY_UTILS_EXECUTABLE_FILE_IO_ENGINE_H

#include "FileOpenError.h"
#include "ExecutableFileReadError.h"
#include "Platform.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <QIODevice>
#include <memory>
#include <cassert>

namespace Mdt{ namespace DeployUtils{

  /// \todo Wrong !
  class AbstractExecutableFileReaderEngine;

  /*! \internal Helper class to instanciate a executable file I/O engine
   */
  class MDT_DEPLOYUTILSCORE_EXPORT ExecutableFileIoEngine : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief File open mode
     */
    enum OpenMode
    {
      ReadOnly,   /*!< Read only */
      ReadWrite   /*!< Read and write */
    };

    /*! \brief Construct a file I/O engine
     */
    explicit ExecutableFileIoEngine(QObject *parent = nullptr);

    /*! \brief Close this file engine and free resources
     */
    ~ExecutableFileIoEngine() noexcept;

    /*! \brief Open a file
     *
     * \pre \a fileInfo must have a file path set
     * \pre this engine must not allready have a file open
     * \sa isOpen()
     * \sa close()
     * \exception FileOpenError
     */
    void openFile(const QFileInfo & fileInfo, OpenMode mode);

    /*! \brief Open a file for a expected platform
     *
     * \pre \a fileInfo must have a file path set
     * \pre \a platform must be valid
     * \pre this engine must not allready have a file open
     * \sa isOpen()
     * \sa close()
     * \exception FileOpenError
     */
    void openFile(const QFileInfo & fileInfo, OpenMode mode, const Platform & platform);

    /*! \brief Check if this engine has a open file
     *
     * \sa openFile()
     * \sa close()
     */
    bool isOpen() const noexcept;

    /*! \brief Close the file that was maybe open
     */
    void close();

    /*! \brief Get the platorm of the file this engine refers to
     *
     * \pre this engine must have a file open
     * \sa isOpen()
     * \exception ExecutableFileReadError
     */
    Platform getFilePlatform();

    /*! \brief Access the engine
     *
     * \pre this engine must have a file open
     * \sa isOpen()
     */
    std::unique_ptr<AbstractExecutableFileReaderEngine> & engine() noexcept
    {
      assert( isOpen() );

      return mIoEngine;
    }

   private:

    void instanciateEngine(ExecutableFileFormat format) noexcept;

    static
    QIODevice::OpenMode qIoDeviceOpenModeFromOpenMode(OpenMode mode) noexcept;

    std::unique_ptr<AbstractExecutableFileReaderEngine> mIoEngine;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_IO_ENGINE_H
