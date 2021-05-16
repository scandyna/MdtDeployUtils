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

#include "ExecutableFileReadError.h"
#include "mdt_deployutils_export.h"
#include <QObject>
#include <QString>

namespace Mdt{ namespace DeployUtils{

  /*! \brief Minimal ELF file reader
   *
   * This ELF reader can just read a minimal set of informations
   * required for deployemnt of applications.
   *
   * For other purposes, other tools
   * like objdump or readelf should be considered.
   */
  class MDT_DEPLOYUTILS_EXPORT ElfFileReader : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief Construct a file reader
     */
    explicit ElfFileReader(QObject *parent = nullptr);

    /*! \brief Check if \a filePath refers to a ELF file
     *
     * \pre \a filePath must not be empty
     * \exception ExecutableFileReadError
     */
    static
    bool isElfFile(const QString & filePath);
    
    /*! \internal
     *
     * \todo REMOVE !
     */
    static
    void sandbox(const QString & filePath);

  };

}} // namespace Mdt{ namespace DeployUtils{

#endif // #ifndef MDT_DEPLOY_UTILS_ELF_FILE_READER_H
