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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_WRITER_H

#include "FileHeader.h"
#include "FileHeaderReaderWriterCommon.h"
#include "FileWriter.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array must be big enough to hold a ident
   * \pre \a ident must be valid
   * \sa identArraySizeIsBigEnough()
   */
  inline
  void identToArray(ByteArraySpan array, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );
    assert( identArraySizeIsBigEnough(array) );

    array.data[0] = 0x7F;
    array.data[1] = 'E';
    array.data[2] = 'L';
    array.data[3] = 'F';
    array.data[4] = static_cast<unsigned char>(ident._class);
    array.data[5] = static_cast<unsigned char>(ident.dataFormat);
    array.data[6] = ident.version;
    array.data[7] = ident.osabi;
    array.data[8] = ident.abiversion;
    array.data[9] = 0;
    array.data[10] = 0;
    array.data[11] = 0;
    array.data[12] = 0;
    array.data[13] = 0;
    array.data[14] = 0;
    array.data[15] = 0;
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array must be big enough to hold a file header
   * 
   * \todo some preconditions on \a fileHeader ?
   * 
   * \pre \a fileHeader's ident must be valid
   * \sa fileHeaderArraySizeIsBigEnough()
   */
  inline
  void fileHeaderToArray(ByteArraySpan array, const FileHeader & fileHeader) noexcept
  {
    assert( !array.isNull() );
    assert( fileHeader.ident.isValid() );
    assert( fileHeaderArraySizeIsBigEnough(array, fileHeader.ident) );

    identToArray(array.subSpan(0, 16), fileHeader.ident);
    
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_WRITER_H
