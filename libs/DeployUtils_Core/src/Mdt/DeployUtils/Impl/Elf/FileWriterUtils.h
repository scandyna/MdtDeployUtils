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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_UTILS_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_UTILS_H

#include "FileHeader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <QtEndian>
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be a least 2 bytes long
   * \pre \a dataFormat must be valid
   */
  inline
  void setHalfWord(ByteArraySpan array, uint16_t value, DataFormat dataFormat) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 2 );
    assert( dataFormat != DataFormat::DataNone );

    if(dataFormat == DataFormat::Data2LSB){
      qToLittleEndian<quint16>(value, array.data);
    }else{
      assert(dataFormat == DataFormat::Data2MSB);
      qToBigEndian<quint16>(value, array.data);
    }
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be a least 4 bytes long
   * \pre \a dataFormat must be valid
   */
  inline
  void set32BitWord(ByteArraySpan array, uint32_t value, DataFormat dataFormat) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 4 );
    assert( dataFormat != DataFormat::DataNone );

    if(dataFormat == DataFormat::Data2LSB){
      qToLittleEndian<quint32>(value, array.data);
    }else{
      assert(dataFormat == DataFormat::Data2MSB);
      qToBigEndian<quint32>(value, array.data);
    }
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be a least 8 bytes long
   * \pre \a dataFormat must be valid
   */
  inline
  void set64BitWord(ByteArraySpan array, uint64_t value, DataFormat dataFormat) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 8 );
    assert( dataFormat != DataFormat::DataNone );

    if(dataFormat == DataFormat::Data2LSB){
      qToLittleEndian<quint64>(value, array.data);
    }else{
      assert(dataFormat == DataFormat::Data2MSB);
      qToBigEndian<quint64>(value, array.data);
    }
  }

  /*! \internal Set a (unsigned) word to \a array
   *
   * Depending on the machine (32-bit or 64-bit), defined in \a ident ,
   * the value will be encoded as a uint32_t or a uint64_t.
   *
   * The endianness, also defined in \a ident ,
   * is also taken into account for the encoding.
   *
   * \pre \a array must not be null
   * \pre \a array must be of a size that can hold the value
   *  (at least 4 bytes for a 32-bit file, at least 8 bytes for a 64-bit file)
   * \pre \a ident must be valid
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  inline
  void setNWord(ByteArraySpan array, uint64_t value, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 4 );
    assert( ident.isValid() );

    if( ident._class == Class::Class32 ){
        set32BitWord(array, static_cast<uint32_t>(value), ident.dataFormat);
      return;
    }

    assert( ident._class == Class::Class64 );
    assert( array.size >= 8 );

    set64BitWord(array, value, ident.dataFormat);
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array must be of a size that can hold the value
   *  (at least 4 bytes for a 32-bit file, at least 8 bytes for a 64-bit file)
   * \pre \a ident must be valid
   */
  inline
  void setAddress(ByteArraySpan array, uint64_t address, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 4 );
    assert( ident.isValid() );

    setNWord(array, address, ident);
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array must be of a size that can hold the value
   *  (at least 4 bytes for a 32-bit file, at least 8 bytes for a 64-bit file)
   * \pre \a ident must be valid
   */
  inline
  void setOffset(ByteArraySpan array, uint64_t offset, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 4 );
    assert( ident.isValid() );

    setNWord(array, offset, ident);
  }

  /*! \internal Set a signed word to \a array
   *
   * Depending on the machine (32-bit or 64-bit), defined in \a ident ,
   * the value will be encoded as a Elf32_Sword (int32_t)
   * or a Elf64_Sxword (int64_t).
   *
   * The endianness, also defined in \a ident ,
   * is also taken into account for the encoding.
   *
   * \pre \a array must not be null
   * \pre \a array must be of a size that can hold the value
   *  (at least 4 bytes for a 32-bit file, at least 8 bytes for a 64-bit file)
   * \pre \a ident must be valid
   *
   * \sa https://manpages.debian.org/stretch/manpages/elf.5.en.html
   */
  inline
  void setSignedNWord(ByteArraySpan array, int64_t value, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );

    if( ident._class == Class::Class32 ){
      assert( array.size >= 4 );
      if( ident.dataFormat == DataFormat::Data2MSB ){
        qToBigEndian<qint32>(static_cast<qint32>(value), array.data);
      }else{
        assert( ident.dataFormat == DataFormat::Data2LSB );
        qToLittleEndian<qint32>(static_cast<qint32>(value), array.data);
      }
    }else{
      assert( ident._class == Class::Class64 );
      assert( array.size >= 8 );
      if( ident.dataFormat == DataFormat::Data2MSB ){
        qToBigEndian<qint64>(value, array.data);
      }else{
        assert( ident.dataFormat == DataFormat::Data2LSB );
        qToLittleEndian<qint64>(value, array.data);
      }
    }
  }
}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_UTILS_H
