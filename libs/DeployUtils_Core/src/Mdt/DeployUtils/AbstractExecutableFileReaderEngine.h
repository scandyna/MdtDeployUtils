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
#ifndef MDT_DEPLOY_UTILS_ABSTRACT_EXECUTABLE_FILE_READER_ENGINE_H
#define MDT_DEPLOY_UTILS_ABSTRACT_EXECUTABLE_FILE_READER_ENGINE_H

#include "FileOpenError.h"
#include "ExecutableFileReadError.h"
#include "Platform.h"
#include "Mdt/DeployUtils/Impl/FileMapper.h"
#include "mdt_deployutils_export.h"
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Interface to a minimal executable file reader
   */
  class MDT_DEPLOYUTILS_EXPORT AbstractExecutableFileReaderEngine : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Construct a file reader
     */
    explicit AbstractExecutableFileReaderEngine(QObject *parent = nullptr);

    /*! \brief Check if this reader supports given platform
     */
    bool supportsPlatform(const Platform & platform) const noexcept
    {
      return doSupportsPlatform(platform);
    }

    /*! \brief Open a file
     *
     * This method does not check if \a fileInfo refers to a executable file of any format.
     *
     * \pre \a fileInfo must have a file path set
     * \pre this reader must not allready have a file open
     * \sa isOpen()
     * \sa close()
     * \exception FileOpenError
     */
    void openFile(const QFileInfo & fileInfo);

    /*! \brief Check if this reader has a open file
     *
     * \sa openFile()
     * \sa close()
     */
    bool isOpen() const noexcept
    {
      return mFile.isOpen();
    }

    /*! \brief Close the file that was maybe open
     */
    void close();

    /*! \brief Check if this reader refers to a ELF file (Linux)
     *
     * \pre this reader must have a open file
     * \sa isOpen()
     * \exception ExecutableFileReadError
     * \note static library archive (libSomeLib.a) are not supported
     */
    bool isElfFile();

    /*! \brief Check if this reader refers to a PE image file (Windows)
     *
     * \pre this reader must have a open file
     * \sa isOpen()
     * \exception ExecutableFileReadError
     * \note static library archive (libSomeLib.a) are not supported
     */
    bool isPeImageFile();

    /*! \brief Get the platorm of the file this reader refers to
     *
     * \pre this reader must have a file open
     * \sa isOpen()
     * \exception ExecutableFileReadError
     */
    Platform getFilePlatform();

    /*! \brief Check if this reader refers to a executable or a shared library
     *
     * \pre this reader must have a open file
     * \sa isOpen()
     * \exception ExecutableFileReadError
     */
    bool isExecutableOrSharedLibrary();

    /*! \brief Get a list of needed shared libraries the file this reader refers to
     *
     * \pre this reader must have a open file which is a executable or a shared library
     * \sa isOpen()
     * \sa isExecutableOrSharedLibrary()
     * \exception ExecutableFileReadError
     */
    QStringList getNeededSharedLibraries();

    /*! \brief Get the run path for the file this reader refers to
     *
     * Will only return a result for executable formats that supports run path
     *
     * \pre this reader must have a open file which is a executable or a shared library
     * \sa isOpen()
     * \sa isExecutableOrSharedLibrary()
     * \exception ExecutableFileReadError
     */
    QStringList getRunPath();

   protected:

    /*! \brief Get the size of the file
     *
     * \pre this reader must have a open file
     * \sa isOpen()
     */
    qint64 fileSize() const noexcept;

    /*! \brief Get the name of the file
     *
     * \pre this reader must have a open file
     * \sa isOpen()
     */
    QString fileName() const noexcept;

    /*! \brief Map the file into memory
     *
     * \pre this reader must have a open file
     * \sa isOpen()
     * \pre \a offset must be >= 0
     * \pre \a size must be > 0
     * \pre \a file size must be at least \a offset + \a size
     * \exception FileOpenError
     */
    Impl::ByteArraySpan mapIfRequired(qint64 offset, qint64 size);

   private:

    virtual void newFileOpen(const QString & fileName) = 0;
    virtual void fileClosed() = 0;

    virtual bool doSupportsPlatform(const Platform & platform) const noexcept = 0;

    virtual bool doIsElfFile()
    {
      return false;
    }

    virtual bool doIsPeImageFile()
    {
      return false;
    }

    virtual Platform doGetFilePlatform() = 0;
    virtual bool doIsExecutableOrSharedLibrary() = 0;
    virtual QStringList doGetNeededSharedLibraries() = 0;

    virtual QStringList doGetRunPath()
    {
      return QStringList();
    }

    Impl::FileMapper mFileMapper;
    QFile mFile;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ABSTRACT_EXECUTABLE_FILE_READER_ENGINE_H
