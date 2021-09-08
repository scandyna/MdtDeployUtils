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
#include "FileWriterUtils.h"
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
    setHalfWord(array.subSpan(0x10, 2), fileHeader.type, fileHeader.ident.dataFormat);
    setHalfWord(array.subSpan(0x12, 2), fileHeader.machine, fileHeader.ident.dataFormat);
    set32BitWord(array.subSpan(0x14, 4), fileHeader.version, fileHeader.ident.dataFormat);
    if(fileHeader.ident._class == Class::Class32){
      setNWord(array.subSpan(0x18, 4), fileHeader.entry, fileHeader.ident);
      setNWord(array.subSpan(0x1C, 4), fileHeader.phoff, fileHeader.ident);
      setNWord(array.subSpan(0x20, 4), fileHeader.shoff, fileHeader.ident);
      set32BitWord(array.subSpan(0x24, 4), fileHeader.flags, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x28, 2), fileHeader.ehsize, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x2A, 2), fileHeader.phentsize, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x2C, 2), fileHeader.phnum, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x2E, 2), fileHeader.shentsize, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x30, 2), fileHeader.shnum, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x32, 2), fileHeader.shstrndx, fileHeader.ident.dataFormat);
    }else{
      assert( fileHeader.ident._class == Class::Class64 );
      setNWord(array.subSpan(0x18, 8), fileHeader.entry, fileHeader.ident);
      setNWord(array.subSpan(0x20, 8), fileHeader.phoff, fileHeader.ident);
      setNWord(array.subSpan(0x28, 8), fileHeader.shoff, fileHeader.ident);
      set32BitWord(array.subSpan(0x30, 4), fileHeader.flags, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x34, 2), fileHeader.ehsize, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x36, 2), fileHeader.phentsize, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x38, 2), fileHeader.phnum, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x3A, 2), fileHeader.shentsize, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x3C, 2), fileHeader.shnum, fileHeader.ident.dataFormat);
      setHalfWord(array.subSpan(0x3E, 2), fileHeader.shstrndx, fileHeader.ident.dataFormat);
    }
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_WRITER_H
