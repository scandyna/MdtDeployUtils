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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_OFFSET_RANGE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_OFFSET_RANGE_H

#include "ProgramHeader.h"
#include "SectionHeader.h"
#include <cstdint>
#include <limits>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class OffsetRange
  {
   public:

    /*! \brief Get the offset of the beginning of this range
     */
    uint64_t begin() const noexcept
    {
      return mBegin;
    }

    /*! \brief Get the offset past the last element of this range
     *
     * \sa lastElement()
     */
    uint64_t end() const noexcept
    {
      return mBegin + mSize;
    }

    /*! \brief Get the size (in bytes) of this range
     */
    uint64_t byteCount() const noexcept
    {
      return mSize;
    }

    /*! \brief Check if this range is empty
     */
    bool isEmpty() const noexcept
    {
      return mSize == 0;
    }

    /*! \brief Get the offset to the last element
     *
     * \pre this range must not be empty
     * \sa end()
     */
    uint64_t lastOffset() const noexcept
    {
      assert( !isEmpty() );

      return end() - 1;
    }

    /*! \brief Get the minimum size (in bytes) to access this range
     */
    int64_t minimumSizeToAccessRange() const noexcept
    {
      /*
       * Hope we never have to read/write so big files
       * At least, catch it in debug mode
       */
      assert( end() <= static_cast<uint64_t>( std::numeric_limits<int64_t>::max() ) );

      return static_cast<int64_t>( end() );
    }

    /*! \brief Get a range from \a begin to \a end offsets
     *
     * Note that \a end is a offset past the end of a range (like STL ranges),
     * which permit to create e a empty range.
     *
     * \pre \a begin must be <= \a end
     */
    static
    OffsetRange fromBeginAndEndOffsets(uint64_t begin, uint64_t end) noexcept
    {
      assert( begin <= end );

      OffsetRange range;

      range.mBegin = begin;
      range.mSize = end - begin;

      return range;
    }

    /*! \brief Construct a offset range from \a header
     */
    static
    OffsetRange fromProgrameHeader(const ProgramHeader & header) noexcept
    {
      OffsetRange range;

      range.mBegin = header.offset;
      range.mSize = header.filesz;

      return range;
    }

    /*! \brief Construct a offset range from \a header
     */
    static
    OffsetRange fromSectionHeader(const SectionHeader & header) noexcept
    {
      OffsetRange range;

      range.mBegin = header.offset;
      range.mSize = header.size;

      return range;
    }

   private:

    uint64_t mBegin = 0;
    uint64_t mSize = 0;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_OFFSET_RANGE_H
