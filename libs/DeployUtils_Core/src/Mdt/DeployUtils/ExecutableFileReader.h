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
#ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_READER_H
#define MDT_DEPLOY_UTILS_EXECUTABLE_FILE_READER_H

#include "FileOpenError.h"
#include "ExecutableFileReadError.h"
#include "Platform.h"
#include "ExecutableFileIoEngine.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QFileInfo>
#include <QStringList>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Reader to extract some informations from a executable file
   *
   * Here is a simplified example of searching dependencies for a given tagert:
   * \code
   * ExecutableFileReader reader;
   * Platform platform;
   * LibrariesDepencies dependencies;
   *
   * reader.openFile(targetExecutable);
   * platform = reader.getFilePlatform();
   * if( !reader.isDynamicLinkedExecutableOrLibrary() ){
   *   // error
   * }
   * dependencies.add( reader.getNeededSharedLibraries(), reader.getRunPath() );
   * reader.close();
   *
   * for( const auto & file : dependencies.firstLevelDependencies() ){
   *   reader.openFile(file, platform);
   *   if( reader.isDynamicLinkedExecutableOrLibrary() ){
   *     libraries.append( reader.getNeededSharedLibraries() );
   *     dependencies.add( reader.getNeededSharedLibraries(), reader.getRunPath() );
   *   }
   *   reader.close();
   * }
   * \endcode
   *
   * Note that \a dependencies magicly gives the full path of its previously added libraries.
   * This example is here incomplete, but the purpose is to show how ExecutableFileReader can be used.
   */
  class MDT_DEPLOYUTILSCORE_EXPORT ExecutableFileReader : public QObject
  {
   Q_OBJECT

   public:

    /*! \brief Construct a file reader
     */
    explicit ExecutableFileReader(QObject *parent = nullptr);

    /*! \brief Close this file reader and free resources
     */
    ~ExecutableFileReader() noexcept = default;

    /*! \brief Open a file
     *
     * \pre \a fileInfo must have a file path set
     * \pre this reader must not allready have a file open
     * \sa isOpen()
     * \sa close()
     * \exception FileOpenError
     */
    void openFile(const QFileInfo & fileInfo);

    /*! \brief Open a file for a expected platform
     *
     * \pre \a fileInfo must have a file path set
     * \pre \a platform must be valid
     * \pre this reader must not allready have a file open
     * \sa isOpen()
     * \sa close()
     * \exception FileOpenError
     */
    void openFile(const QFileInfo & fileInfo, const Platform & platform);

    /*! \brief Check if this reader has a open file
     *
     * \sa openFile()
     * \sa close()
     */
    bool isOpen() const noexcept;

    /*! \brief Close the file that was maybe open
     */
    void close();

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
     * \note There is not method to check if this reader refers to a shared library.
     * The reason is that there is no difference between
     * a dynamicly linked executable and a shared library on some platform, like ELF.
     * For some explanations,
     * see https://unix.stackexchange.com/questions/472449/what-is-the-difference-between-lsb-executable-et-exec-and-lsb-shared-object
     *
     * \todo See Program Header type - PT_INTERP (see for Section Header equivalent)
     */
    bool isExecutableOrSharedLibrary();

    /*! \brief Check if this reader refers to a executable that contains debug symbols
     *
     * \pre this reader must have a open file which is a executable or a shared library
     * \sa isOpen()
     * \sa isExecutableOrSharedLibrary()
     * \exception ExecutableFileReadError
     */
    bool containsDebugSymbols();

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

   private:

    ExecutableFileIoEngine mEngine;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_EXECUTABLE_FILE_READER_H
