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
#ifndef MDT_DEPLOY_UTILS_IMPL_EXECUTABLE_FILE_READER_UTILS_H
#define MDT_DEPLOY_UTILS_IMPL_EXECUTABLE_FILE_READER_UTILS_H

#include "ByteArraySpan.h"
#include <QtEndian>
#include <initializer_list>
#include <cstdint>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   *
   * \pre \a start must not be a nullptr
   * \pre \a count must be >= 0
   */
  inline
  bool arraysAreEqual(const unsigned char * const start, int64_t count, std::initializer_list<unsigned char> reference) noexcept
  {
    assert( start != nullptr );
    assert( count >= 0 );

    const auto first = start;
    const auto last = start + count;

    return std::equal( first, last, reference.begin(), reference.end() );
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be at least 2 bytes
   */
  inline
  uint16_t get16BitValueLE(const ByteArraySpan & array) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 2 );

    return qFromLittleEndian<uint16_t>(array.data);
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be at least 4 bytes
   */
  inline
  uint32_t get32BitValueLE(const ByteArraySpan & array) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 4 );

    return qFromLittleEndian<uint32_t>(array.data);
  }

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array size must be at least 8 bytes
   */
  inline
  uint64_t get64BitValueLE(const ByteArraySpan & array) noexcept
  {
    assert( !array.isNull() );
    assert( array.size >= 8 );

    return qFromLittleEndian<uint64_t>(array.data);
  }

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_EXECUTABLE_FILE_READER_UTILS_H
