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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_TABLE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_TABLE_H

#include "SectionHeader.h"
#include "SectionIndexChangeMap.h"
#include <vector>
#include <string>
#include <iterator>
#include <utility>
#include <algorithm>
#include <cstdint>
#include <cassert>

// #include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal Find the first section header matching \a type and \a name
   */
  inline
  std::vector<SectionHeader>::const_iterator
  findFirstSectionHeader(const std::vector<SectionHeader> & sectionHeaderTable, SectionType type, const std::string & name) noexcept
  {
    const auto pred = [type, &name](const SectionHeader & header) -> bool{
      return (header.sectionType() == type)&&(header.name == name);
    };

    return std::find_if(sectionHeaderTable.cbegin(), sectionHeaderTable.cend(), pred);
  }

  /*! \internal Find the index of the first section header matching \a type and \a name in the section header table
   *
   * If not section matching \a type and \a name exit in the table,
   * 0 is returned (which the corresponds to a null section header)
   */
  inline
  uint16_t findIndexOfFirstSectionHeader(const std::vector<SectionHeader> & sectionHeaderTable, SectionType type, const std::string & name) noexcept
  {
    const auto it = findFirstSectionHeader(sectionHeaderTable, type, name);
    if( it == sectionHeaderTable.cend() ){
      return 0;
    }

    return static_cast<uint16_t>( std::distance(sectionHeaderTable.cbegin(), it) );
  }

  /*! \internal Find the index of the section header at \a offset
   *
   * If not section exists at \a offset ,
   * 0 is returned (which the corresponds to a null section header)
   */
  inline
  uint16_t findIndexOfSectionHeaderAtOffset(const std::vector<SectionHeader> & sectionHeaderTable, uint64_t offset) noexcept
  {
    const auto pred = [offset](const SectionHeader & header) -> bool{
      return header.offset == offset;
    };

    const auto it = std::find_if(sectionHeaderTable.cbegin(), sectionHeaderTable.cend(), pred);
    if( it == sectionHeaderTable.cend() ){
      return 0;
    }

    return static_cast<uint16_t>( std::distance(sectionHeaderTable.cbegin(), it) );
  }

  /*! \internal Find the index of the first section header matching \a name in the section header table
   *
   * If not section matching \a name exit in the table,
   * 0 is returned (which the corresponds to a null section header)
   */
  inline
  uint16_t findIndexOfFirstSectionHeader(const std::vector<SectionHeader> & sectionHeaderTable, const std::string & name) noexcept
  {
    const auto pred = [&name](const SectionHeader & header) -> bool{
      return header.name == name;
    };

    const auto it = std::find_if(sectionHeaderTable.cbegin(), sectionHeaderTable.cend(), pred);
    if( it == sectionHeaderTable.cend() ){
      return 0;
    }

    return static_cast<uint16_t>( std::distance(sectionHeaderTable.cbegin(), it) );
  }

  /*! \internal
   */
  inline
  SectionIndexChangeMap makeSectionIndexChangeMap(const std::vector<SectionHeader> & headers) noexcept
  {
    return SectionIndexChangeMap( static_cast<uint16_t>( headers.size() ) );
  }

  /*! \internal Check if \a headers are sorted by file offset
   */
  inline
  bool sectionHeadersAreSortedByFileOffset(const std::vector<SectionHeader> & headers) noexcept
  {
    const auto cmp = [](const SectionHeader & a, const SectionHeader & b){
      return a.offset < b.offset;
    };

    return std::is_sorted(headers.begin(), headers.end(), cmp);
  }

  /*! \internal Sort a collection of section headers by file offset
   */
  inline
  SectionIndexChangeMap sortSectionHeadersByFileOffset(std::vector<SectionHeader> & headers) noexcept
  {
    SectionIndexChangeMap indexChangeMap = makeSectionIndexChangeMap(headers);

    if( sectionHeadersAreSortedByFileOffset(headers) ){
      return indexChangeMap;
    }

    const auto aLessThanB = [](const SectionHeader & a, const SectionHeader & b){
      return a.offset < b.offset;
    };
    /*
     * If we use std::sort(), we loose track of index changes.
     * A section header table has about 50 entries,
     * so a O(N^2) sort should be ok.
     */
    for(auto it = headers.begin(); it != headers.end(); ++it){
      const auto selectionIt = std::min_element(it, headers.end(), aLessThanB);
      assert( selectionIt != headers.end() );

      if( aLessThanB(*selectionIt, *it) ){
        const uint16_t itIndex = static_cast<uint16_t>( std::distance(headers.begin(), selectionIt) );
        const uint16_t selectionIndex = static_cast<uint16_t>( std::distance(headers.begin(), it) );
        indexChangeMap.swapIndexes(itIndex, selectionIndex);

        std::swap(*it, *selectionIt);
      }
    }

    /*
     * Some sections have a index to a other.
     * This can be represented by sh_link, but also by sh_info .
     * Sorting the headers will invalid them,
     * so we have to restore them.
     */
    for(SectionHeader & header : headers){
      if( header.linkIsIndexInSectionHeaderTable() ){
        header.link = indexChangeMap.indexForOldIndex(header.link);
      }
      if( header.infoIsIndexInSectionHeaderTable() ){
        header.info = indexChangeMap.indexForOldIndex(header.info);
      }
    }

    return indexChangeMap;
  }

  /*! \internal Find the count of sections to move to free given \a size in \a headers
   *
   * If the requested size is greater than the total size represented in \a headers ,
   * the returned count will also be greater than the count of sections represented by \a headers .
   *
   * \pre \a size must be > 0
   * \pre \a headers must be sorted by file offsets
   * \sa sectionHeadersAreSortedByFileOffset()
   */
  inline
  size_t findCountOfSectionsToMoveToFreeSize(const std::vector<SectionHeader> & headers, uint16_t size) noexcept
  {
    assert(size > 0);
    assert( sectionHeadersAreSortedByFileOffset(headers) );

    /*
     * Sections should start after file header
     * and, most of the case, after the program header table.
     * The sections header table contains a null section (SHT_NULL),
     * that declares a offset of 0, which has no meaning.
     * If the section header table contains other sections
     * that have a offset of 0, thei will be at the beginning of the table
     * (section headers must be sorted by file offsets).
     */
    const auto nonNullOffsetPred = [](const SectionHeader & header){
      return header.offset > 0;
    };
    auto sectionIt = std::find_if(headers.cbegin(), headers.cend(), nonNullOffsetPred);

    if( sectionIt == headers.cend() ){
      return headers.size() + 1;
    }

    /*
     * size:      20    30    10
     * section: |  A  |     |  B  |
     * offset:   50    70    100   110
     */

    const uint64_t requestedSize = size;
    size_t sectionCount = static_cast<size_t>( std::distance(headers.cbegin(), sectionIt) ) + 1;
    uint64_t totalSize = 0;

    /*
     * There is a gap between the beginning of the file
     * and the first section.
     * This is not a hole, but other stuff than sections
     * (file header, ...).
     *
     * So, point to the end of a virtual section
     * starting at 0 and with end at the first section.
     */
    uint64_t previousSectionEnd = sectionIt->offset;

    while( sectionIt != headers.cend() ){
      // Accumulate the hole (if any)
      totalSize += sectionIt->offset - previousSectionEnd;
      if(requestedSize <= totalSize){
        return sectionCount - 1;
      }
      // Accumulate section size
      totalSize += sectionIt->size;
      if(requestedSize <= totalSize){
        return sectionCount;
      }
      previousSectionEnd = sectionIt->offset + sectionIt->size;
      ++sectionCount;
      ++sectionIt;
    }

    return sectionCount;
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_TABLE_H
