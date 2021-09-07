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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_READER_WRITER_COMMON_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_READER_WRITER_COMMON_H

#include "FileHeader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \pre \a array must not be null
   */
  inline
  bool identArraySizeIsBigEnough(const ByteArraySpan & array) noexcept
  {
    assert( !array.isNull() );

    return array.size >= 16;
  }

  /*! \internal
   *
   * \pre \a ident must be valid
   */
  inline
  int64_t minimumSizeToReadFileHeader(const Ident & ident) noexcept
  {
    assert( ident.isValid() );

    if( ident._class == Class::Class32 ){
      return 52;
    }
    assert( ident._class == Class::Class64 );

    return 64;
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a ident must be valid
   */
  inline
  bool fileHeaderArraySizeIsBigEnough(const ByteArraySpan & array, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );

    return array.size >= minimumSizeToReadFileHeader(ident);
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_HEADER_READER_WRITER_COMMON_H
