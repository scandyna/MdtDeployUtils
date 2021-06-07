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

#include "AbstractExecutableFileReaderEngine.h"
#include "FileOpenError.h"
#include "ExecutableFileReadError.h"
#include "mdt_deployutils_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
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
  class MDT_DEPLOYUTILS_EXPORT ElfFileReader : public AbstractExecutableFileReaderEngine
  {
    Q_OBJECT

   public:

    /*! \brief Construct a file reader
     */
    explicit ElfFileReader(QObject *parent = nullptr);

    ~ElfFileReader() noexcept;

    /*! \brief Get the shared object name (SONAME) of the file this reader refers to
     *
     * \pre this reader must have a open file which is a executable or a shared library
     * \sa isOpen()
     * \sa isExecutableOrSharedLibrary()
     * \exception ExecutableFileReadError
     */
    QString getSoName();

   private:

    void newFileOpen(const QString & fileName) override;
    void fileClosed() override;
    bool doIsElfFile() override;
    bool doIsExecutableOrSharedLibrary() override;
    QStringList doGetNeededSharedLibraries() override;
    QStringList doGetRunPath() override;

    std::unique_ptr<Impl::Elf::FileReader> mImpl;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ELF_FILE_READER_H
