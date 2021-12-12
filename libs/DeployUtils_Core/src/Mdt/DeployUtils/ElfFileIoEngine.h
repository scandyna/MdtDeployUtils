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
#ifndef MDT_DEPLOY_UTILS_ELF_FILE_IO_ENGINE_H
#define MDT_DEPLOY_UTILS_ELF_FILE_IO_ENGINE_H

#include "AbstractExecutableFileIoEngine.h"
#include "FileOpenError.h"
#include "ExecutableFileReadError.h"
#include "RPath.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

namespace Mdt{ namespace DeployUtils{

  namespace Impl{ namespace Elf{

    class FileIoEngine;

  }} // namespace Impl{ namespace Elf{

  /*! \brief Minimal ELF file I/O engine
   *
   * This ELF reader and writer can just read/write a minimal set of informations
   * required for deployement of applications.
   *
   * For other purposes, other tools
   * like objdump or readelf should be considered.
   *
   * \sa ExecutableFileReader
   * \sa ExecutableFileWriter
   */
  class MDT_DEPLOYUTILSCORE_EXPORT ElfFileIoEngine : public AbstractExecutableFileIoEngine
  {
    Q_OBJECT

   public:

    /*! \brief Construct a file I/O engine
     */
    explicit ElfFileIoEngine(QObject *parent = nullptr);

    ~ElfFileIoEngine() noexcept;

    /*! \brief Get the shared object name (SONAME) of the file this engine refers to
     *
     * \pre this engine must have a open file which is a executable or a shared library
     * \sa isOpen()
     * \sa isExecutableOrSharedLibrary()
     * \exception ExecutableFileReadError
     */
    QString getSoName();

   private:

    void newFileOpen(const QString & fileName) override;
    void fileClosed() override;
    bool doSupportsPlatform(const Platform & platform) const noexcept override;
    bool doIsElfFile() override;
    Platform doGetFilePlatform() override;
    bool doIsExecutableOrSharedLibrary() override;
    bool doContainsDebugSymbols() override;
    QStringList doGetNeededSharedLibraries() override;
    RPath doGetRunPath() override;
    void doSetRunPath(const RPath & rPath) override;

    std::unique_ptr<Impl::Elf::FileIoEngine> mImpl;
  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ELF_FILE_IO_ENGINE_H
