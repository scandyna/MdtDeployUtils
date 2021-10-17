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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_HEADER_TABLE_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_HEADER_TABLE_H

#include "ProgramHeader.h"
#include <vector>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal Represents the program header table in a ELF file
   */
  class ProgramHeaderTable
  {
   public:

    /*! \brief STL const iterator
     */
    using const_iterator = std::vector<ProgramHeader>::const_iterator;

    /*! \brief STL iterator
     */
    using iterator = std::vector<ProgramHeader>::iterator;

    /*! \brief Get the count of headers in this table
     */
    size_t headerCount() const noexcept
    {
      return mTable.size();
    }

    /*! \brief Check if this table is empty
     */
    bool isEmpty() const noexcept
    {
      return mTable.empty();
    }

    /*! \brief Get the program header at \a index
     *
     * \pre \a index must be in valid range ( \a index < headerCount() )
     */
    const ProgramHeader & headerAt(size_t index) const noexcept
    {
      assert( index < headerCount() );

      return mTable[index];
    }

    /*! \brief Add \a header readen from a file
     *
     * This method simply adds \a header,
     * id does not adjust anything in this table
     * (except setting the internal index of a known segment).
     */
    void addHeaderFromFile(const ProgramHeader & header) noexcept
    {
      switch( header.segmentType() ){
        case SegmentType::Dynamic:
          mDynamicSectionHeaderIndex = mTable.size();
          break;
        case SegmentType::ProgramHeaderTable:
          mProgramHeaderTableHeaderIndex = mTable.size();
          break;
        default:
          break;
      }

      mTable.push_back(header);
    }

    /*! \brief Add \a header to this table
     *
     * \sa addHeaderFromFile()
     */
    void addHeader(const ProgramHeader & header, uint16_t programHeaderEntrySize) noexcept
    {
      addHeaderFromFile(header);

      if( containsProgramHeaderTableHeader() ){
        const uint64_t size = mTable.size() * programHeaderEntrySize;
        mTable[mProgramHeaderTableHeaderIndex].memsz = size;
        mTable[mProgramHeaderTableHeaderIndex].filesz = size;
      }
    }

    /*! \brief Add a new load segment to the end of the program header table
     */
    ProgramHeader & appendNullLoadSegment(uint16_t programHeaderEntrySize) noexcept
    {
      ProgramHeader header;

      header.setSegmentType(SegmentType::Load);
      header.flags = 0;
      header.offset = 0;
      header.vaddr = 0;
      header.paddr = 0;
      header.filesz = 0;
      header.memsz = 0;
      header.align = 0;

      addHeader(header, programHeaderEntrySize);

      return mTable[mTable.size()-1];
    }

    /*! \brief Check if the program header for the program header table exists (PT_PHDR)
     */
    bool containsProgramHeaderTableHeader() const noexcept
    {
      return mProgramHeaderTableHeaderIndex < std::numeric_limits<size_t>::max();
    }

    /*! \brief Get the program header table program header (PT_PHDR)
     *
     * \pre the program header table program header must exist
     * \sa containsProgramHeaderTableProgramHeader()
     */
    const ProgramHeader & programHeaderTableHeader() const noexcept
    {
      assert( containsProgramHeaderTableHeader() );

      return mTable[mProgramHeaderTableHeaderIndex];
    }

    /*! \brief Set the virtual address and offset of the program header table program header (PT_PHDR)
     */
    void setProgramHeaderTableHeaderVirtualAddressAndFileOffset(uint64_t virtualAddress, uint64_t fileOffset) noexcept
    {
      assert( containsProgramHeaderTableHeader() );

      mTable[mProgramHeaderTableHeaderIndex].vaddr = virtualAddress;
      mTable[mProgramHeaderTableHeaderIndex].paddr = virtualAddress;
      mTable[mProgramHeaderTableHeaderIndex].offset = fileOffset;
    }

    /*! \brief Shift the offset of program hedares after \a refOffset by \a offset
     *
     * \todo use OffsetChange
     */
    void shiftHeadersOffsetAfter(uint64_t refOffset, int64_t offset) noexcept
    {
      for(auto & header : mTable){
        if(header.offset > refOffset){
          header.shiftOffset(offset);
        }
      }
    }

    /*! \brief Check if this table contains the program header of the dynamic section
     */
    bool containsDynamicSectionHeader() const noexcept
    {
      return mDynamicSectionHeaderIndex < std::numeric_limits<size_t>::max();
    }

    /*! \brief Get the program header of the dynamic section
     *
     * \pre the program header of the dynamic section must exist in this table
     */
    const ProgramHeader & dynamicSectionHeader() const noexcept
    {
      assert( containsDynamicSectionHeader() );

      return mTable[mDynamicSectionHeaderIndex];
    }

    /*! \brief Set the file size of the dynamic section
     *
     * \pre the program header of the dynamic section must exist in this table
     * \sa containsDynamicSectionHeader()
     */
    void setDynamicSectionFileSize(uint64_t size) noexcept
    {
      assert( containsDynamicSectionHeader() );

      mTable[mDynamicSectionHeaderIndex].filesz = size;
    }

    /*! \brief Set the virtual address and file offset of the dynamic section
     *
     * \pre the program header of the dynamic section must exist in this table
     * \sa containsDynamicSectionHeader()
     */
    void setDynamicSectionVirtualAddressAndFileOffset(uint64_t virtualAddress, uint64_t fileOffset) noexcept
    {
      assert( containsDynamicSectionHeader() );

      mTable[mDynamicSectionHeaderIndex].vaddr = virtualAddress;
      mTable[mDynamicSectionHeaderIndex].paddr = virtualAddress;
      mTable[mDynamicSectionHeaderIndex].offset = fileOffset;
    }

    /*! \brief Get the virtual address of the end of the last segment of this table
     *
     * \note the returned address is 1 byte past the last virtual address of the last segment
     * \pre this table must not be empty
     */
    uint64_t findLastSegmentVirtualAddressEnd() const noexcept
    {
      assert( !isEmpty() );

      const auto cmp = [](const ProgramHeader & a, const ProgramHeader & b){
        return a.segmentVirtualAddressEnd() < b.segmentVirtualAddressEnd();
      };

      const auto it = std::max_element(cbegin(), cend(), cmp);
      assert( it != cend() );

      return it->segmentVirtualAddressEnd();
    }

    /*! \brief Get the file offset of the last segment of this table
     *
     * \note the returned offset is 1 byte past the last offset of the last segment
     * \pre this table must not be empty
     */
    uint64_t findLastSegmentFileOffsetEnd() const noexcept
    {
      assert( !isEmpty() );

      const auto cmp = [](const ProgramHeader & a, const ProgramHeader & b){
        return a.fileOffsetEnd() < b.fileOffsetEnd();
      };

      const auto it = std::max_element(cbegin(), cend(), cmp);
      assert( it != cend() );

      return it->fileOffsetEnd();
    }

    /*! \brief get the begin iterator
     */
    const_iterator cbegin() const noexcept
    {
      return mTable.cbegin();
    }

    /*! \brief get the begin iterator
     */
    const_iterator begin() const noexcept
    {
      return mTable.cbegin();
    }

    /*! \brief get the begin iterator
     */
    iterator begin() noexcept
    {
      return mTable.begin();
    }

    /*! \brief get the begin iterator
     */
    const_iterator cend() const noexcept
    {
      return mTable.cend();
    }

    /*! \brief get the begin iterator
     */
    const_iterator end() const noexcept
    {
      return mTable.cend();
    }

    /*! \brief get the begin iterator
     */
    iterator end() noexcept
    {
      return mTable.end();
    }

   private:

    size_t mDynamicSectionHeaderIndex = std::numeric_limits<size_t>::max();
    size_t mProgramHeaderTableHeaderIndex = std::numeric_limits<size_t>::max();
    std::vector<ProgramHeader> mTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_HEADER_TABLE_H
