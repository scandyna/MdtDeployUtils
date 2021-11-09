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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_INDEX_CHANGE_MAP_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_INDEX_CHANGE_MAP_H

#include <cstdint>
#include <vector>
#include <numeric>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class SectionIndexChangeMap
  {
   public:

    /*! \brief Construct a empty map
     */
    SectionIndexChangeMap() noexcept = default;

    /*! \brief Construct a map with \a indexCount indexes
     */
    SectionIndexChangeMap(uint16_t indexCount) noexcept
     : mMap(indexCount)
    {
      std::iota(mMap.begin(), mMap.end(), 0);
    }

    /*! \brief Copy construct a map from \a other
     */
    SectionIndexChangeMap(const SectionIndexChangeMap & other) = default;

    /*! \brief Copy assign \a other to this map
     */
    SectionIndexChangeMap & operator=(const SectionIndexChangeMap & other) = default;

    /*! \brief Move construct a map from \a other
     */
    SectionIndexChangeMap(SectionIndexChangeMap && other) noexcept = default;

    /*! \brief Move assign \a other to this map
     */
    SectionIndexChangeMap & operator=(SectionIndexChangeMap && other) noexcept = default;

    /*! \brief Swap 2 indexes
     *
     * The new index for the old one, \a a , will become \a b , and,
     * The new index for the old one, \a b , will become \a a .
     */
    void swapIndexes(uint16_t a, uint16_t b) noexcept
    {
      assert( a < mMap.size() );
      assert( b < mMap.size() );

      mMap[a] = b;
      mMap[b] = a;
    }

    /*! \brief Get the index for given \a oldIndex
     */
    uint16_t indexForOldIndex(uint16_t oldIndex) const noexcept
    {
      assert( oldIndex < mMap.size() );

      return mMap[oldIndex];
    }

    /*! \brief Get the count of entries in this map
     */
    size_t entriesCount() const noexcept
    {
      return mMap.size();
    }

    /*! \brief Check if this map is empty
     */
    bool isEmpty() const noexcept
    {
      return mMap.empty();
    }

    /*! \brief Get the change at \a entryIndex
     */
//     const SectionIndexChange & entryAt(size_t entryIndex) const noexcept
//     {
//       assert( entryIndex < entriesCount() );
// 
//       return mMap[entryIndex];
//     }

   private:

    std::vector<uint16_t> mMap;
//     std::vector<SectionIndexChange> mMap;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_INDEX_CHANGE_MAP_H
