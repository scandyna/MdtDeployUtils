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
#ifndef MDT_DEPLOY_UTILS_ELF_FILE_READER_H
#define MDT_DEPLOY_UTILS_ELF_FILE_READER_H

#include "FileOpenError.h"
#include "ExecutableFileReadError.h"
#include "Mdt/DeployUtils/Impl/FileMapper.h"
#include "mdt_deployutils_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  namespace Impl{ namespace Elf{

    class FileReader;

  }} // namespace Impl{ namespace Elf{

  /*! \brief Minimal ELF file reader
   *
   * This ELF reader can just read a minimal set of informations
   * required for deployemnt of applications.
   *
   * For other purposes, other tools
   * like objdump or readelf should be considered.
   *
   * \todo Example wrong for RUNPATH !!
   * Example:
   * \code
   * QStringList libraries;
   * QStringList runPath;
   * ElfFileReader reader;
   *
   * for(const auto & file : fileList){
   *   reader.openFile(file);
   *   if( reader.isDynamicLinkedExecutableOrLibrary() ){
   *     libraries.append( reader.getNeededSharedLibraries() );
   *   }
   *   runPath = reader.getRunPath();
   *   reader.close();
   * }
   * \endcode
   */
  class MDT_DEPLOYUTILS_EXPORT ElfFileReader : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Construct a file reader
     */
    explicit ElfFileReader(QObject *parent = nullptr);

    ~ElfFileReader() noexcept;

    /*! \brief Open a file
     *
     * This method does not check if \a fileInfo refers to a ELF file.
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

    /*! \brief Check if this reader refers to a ELF file
     *
     * \pre this reader must have a open file
     * \sa isOpen()
     * \exception ExecutableFileReadError
     * \note static library archive (libSomeLib.a) are not supported
     */
    bool isElfFile();

    /*! \brief Check if this reader refers to a executable or a shared library
     *
     * This method will only read the first 64 bytes of the file.
     *
     * \pre this reader must have a open file
     * \sa isOpen()
     * \exception ExecutableFileReadError
     */
    bool isExecutableOrSharedLibrary();

    /*! \brief Get the shared object name (SONAME) of the file this reader refers to
     *
     * \pre this reader must have a open file which is a executable or a shared library
     * \sa isOpen()
     * \sa isExecutableOrSharedLibrary()
     * \exception ExecutableFileReadError
     */
    QString getSoName();

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
     * \pre this reader must have a open file which is a executable or a shared library
     * \sa isOpen()
     * \sa isExecutableOrSharedLibrary()
     * \exception ExecutableFileReadError
     */
    QStringList getRunPath();

    /*! \brief Check if \a filePath refers to a ELF file
     *
     * This helper method is similar to:
     * \code
     * ElfFileReader reader;
     *
     * reader.openFile(filePath);
     * bool isElf = reader.isElfFile();
     * reader.close();
     * \endcode
     *
     * \pre \a filePath must not be empty
     * \exception ExecutableFileReadError
     * \sa isElfFile()
     */
    static
    bool isElfFile(const QString & filePath);

   private:

    std::unique_ptr<Impl::Elf::FileReader> mImpl;
    Impl::FileMapper mFileMapper;
    QFile mFile;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ELF_FILE_READER_H
