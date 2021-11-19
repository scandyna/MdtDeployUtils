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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GNU_HASH_TABLE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_GNU_HASH_TABLE_H

#include "Ident.h"
#include <cstdint>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \code
   * struct GnuHashTable
   * {
   *   uint32_t nbuckets;
   *   uint32_t symoffset;
   *   uint32_t bloomSize;
   *   uint32_t bloomShift;
   *   uint64_t bloom[bloom_size]; // uint32_t for 32-bit binaries
   *   uint32_t buckets[nbuckets];
   *   uint32_t chain[];
   * };
   * \endcode
   *
   * \sa https://flapenguin.me/elf-dt-gnu-hash
   */
  struct GnuHashTable
  {
    uint32_t symoffset;
    uint32_t bloomShift;
    std::vector<uint64_t> bloom;
    std::vector<uint32_t> buckets;
    std::vector<uint32_t> chain;

    /*! \brief get the count of buckets (nbuckets)
     */
    uint32_t bucketCount() const noexcept
    {
      return static_cast<uint32_t>( buckets.size() );
    }

    /*! \brief get the size of the bloom
     */
    uint32_t bloomSize() const noexcept
    {
      return static_cast<uint32_t>( bloom.size() );
    }

    /*! \brief Get the size, in bytes, of this hash table
     */
    int64_t byteCount(Class _class) const noexcept
    {
      assert(_class != Class::ClassNone);

      const int64_t bloomEntrySize = bloomEntryByteCount(_class);
      const int64_t bloomEntryCount = static_cast<int64_t>( bloom.size() );
      const int64_t bucketsEntryCount = static_cast<int64_t>( buckets.size() );
      const int64_t chainEntryCount = static_cast<int64_t>( chain.size() );

      return 16 + bloomEntrySize * bloomEntryCount + 4 * bucketsEntryCount + 4 * chainEntryCount;
    }

    /*! \brief Get the size (in bytes) for a bloom entry
     */
    static
    int64_t bloomEntryByteCount(Class _class) noexcept
    {
      if(_class == Class::Class64){
        return 8;
      }
      assert(_class == Class::Class32);

      return 4;
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GNU_HASH_TABLE_H
