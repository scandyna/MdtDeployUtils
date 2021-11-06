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
#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <cstdint>
#include <cassert>
#include <map>

#include <iostream>

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
  void sortSectionHeadersByFileOffset(std::vector<SectionHeader> & headers) noexcept
  {
    if( sectionHeadersAreSortedByFileOffset(headers) ){
      return;
    }

    /*
     * Some sections have a index to a other.
     * This can be represented by sh_link, bust also by sh_info .
     * Sorting the headers will invalid them,
     * so map them so we can restore after sort
     */
    std::map<std::string,std::string> sectionLinkMap;
    std::map<std::string,std::string> sectionInfoMap;

    for(const SectionHeader & header : headers){
      if( header.linkIsIndexInSectionHeaderTable() ){
        assert( header.link < headers.size() );
        sectionLinkMap[header.name] = headers[header.link].name;
      }
      if( header.infoIsIndexInSectionHeaderTable() ){
        assert( header.info < headers.size() );
        sectionInfoMap[header.name] = headers[header.info].name;
      }
    }

    const auto cmp = [](const SectionHeader & a, const SectionHeader & b){
      return a.offset < b.offset;
    };

    std::sort(headers.begin(), headers.end(), cmp);

    /*
     * Restore the links
     */
    for(SectionHeader & header : headers){
      if( header.linkIsIndexInSectionHeaderTable() ){
        header.link = findIndexOfFirstSectionHeader( headers, sectionLinkMap[header.name] );
      }
      if( header.infoIsIndexInSectionHeaderTable() ){
        header.info = findIndexOfFirstSectionHeader( headers, sectionInfoMap[header.name] );
      }
    }
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

//     if( headers.empty() ){
//       return 1;
//     }

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
//     auto sectionIt = headers.cbegin();
//     size_t sectionCount = 0;
    size_t sectionCount = static_cast<size_t>( std::distance(headers.cbegin(), sectionIt) ) + 1;
    uint64_t totalSize = 0;

    std::cout << "requested size: " << requestedSize << std::endl;

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
      std::cout << " current section offset: " << sectionIt->offset << " , size: " << sectionIt->size << std::endl;
      // Accumulate the hole (if any)
      std::cout << " accumalating hole...\n";
//       const uint64_t holeSize = sectionIt->offset - previousSectionEnd;
//       std::cout << "  hole size: " << holeSize << std::endl;
//       if(holeSize > 0){
//         totalSize += holeSize;
//         if(requestedSize <= totalSize){
//           return sectionCount - 1;
//         }
//       }
      totalSize += sectionIt->offset - previousSectionEnd;
      std::cout << " total size: " << totalSize << " , section count: " << sectionCount << std::endl;
      if(requestedSize <= totalSize){
//         return sectionCount - 1;
        return sectionCount;
      }
//       ++sectionCount;
      // Accumulate section size
      std::cout << " accumalating section size...\n";
      totalSize += sectionIt->size;
      std::cout << " total size: " << totalSize << " , section count: " << sectionCount << std::endl;
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
