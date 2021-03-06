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
#ifndef MDT_DEPLOY_UTILS_IMPL_BYTE_ARRAY_SPAN_H
#define MDT_DEPLOY_UTILS_IMPL_BYTE_ARRAY_SPAN_H

#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{

  /*! \internal
   */
  struct ByteArraySpan
  {
    const unsigned char *data = nullptr;
    int64_t size = 0;

    constexpr
    bool isNull() const noexcept
    {
      return data == nullptr;
    }

    /*! \brief Get a span that is a view over the \a count elements of this span starting at \a offset
     *
     * \pre this span must not be null
     * \pre \a offset must be >= 0
     * \pre \a count must be > 0
     * \pre \a offset + \a count must be <= the size of this span
     */
    constexpr
    ByteArraySpan subSpan(int64_t offset, int64_t count) const noexcept
    {
      assert( !isNull() );
      assert( offset >= 0 );
      assert( count > 0 );
      assert( (offset + count) <= size );

      ByteArraySpan span;

      span.data = data + offset;
      span.size = count;

      return span;
    }

    /*! \brief Get a span that is a view over the remainding elements of this span starting at \a offset
     *
     * \pre this span must not be null
     * \pre \a offset must be >= 0
     * \pre \a offset must be < the size of this span
     */
    constexpr
    ByteArraySpan subSpan(int64_t offset) const noexcept
    {
      assert( !isNull() );
      assert( offset >= 0 );
      assert( offset < size );

      return subSpan(offset, size-offset);
    }

  };

}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_BYTE_ARRAY_SPAN_H
