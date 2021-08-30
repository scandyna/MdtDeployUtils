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
#ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_WRITER_H
#define MDT_DEPLOY_UTILS_EXECUTABLE_FILE_WRITER_H

#include "FileOpenError.h"
#include "ExecutableFileReadError.h"
#include "ExecutableFileWriteError.h"
#include "ExecutableFileIoEngine.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Writer to set some attributes to a executable file
   *
   * Here is a example to set the RPath to a executable
   * (if executable format supports RPath):
   * \code
   * ExecutableFileWriter writer;
   *
   * writer.openFile(targetLibrary);
   * writer.setRunPath({"$ORIGIN"});
   * writer.close();
   * \endcode
   *
   * \todo Update above doc once RPath class have been created
   */
  class MDT_DEPLOYUTILSCORE_EXPORT ExecutableFileWriter : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Construct a file writer
     */
    explicit ExecutableFileWriter(QObject *parent = nullptr);

    /*! \brief Close this file writer and free resources
     */
    ~ExecutableFileWriter() noexcept = default;

    /*! \brief Open a file
     *
     * \pre \a fileInfo must have a file path set
     * \pre this writer must not allready have a file open
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
    bool isOpen() const noexcept;

    /*! \brief Close the file that was maybe open
     */
    void close();

    /*! \brief Check if this writer refers to a executable or a shared library
     *
     * \pre this writer must have a open file
     * \sa isOpen()
     * \exception ExecutableFileReadError
     * \sa ExecutableFileReader::isExecutableOrSharedLibrary()
     */
    bool isExecutableOrSharedLibrary();

    /*! \brief Set the run path this writer refers to to \a rPath
     *
     * For executable that do not support RPath,
     * this method does nothing.
     *
     * \pre this writer must have a open file which is a executable or a shared library
     * \sa isOpen()
     * \sa isExecutableOrSharedLibrary()
     * \exception ExecutableFileWriteError
     */
    void setRunPath(const QStringList & rPath);

   private:

    ExecutableFileIoEngine mEngine;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_WRITER_H
