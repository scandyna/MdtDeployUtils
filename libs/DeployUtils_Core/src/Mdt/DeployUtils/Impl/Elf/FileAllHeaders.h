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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_H

#include "FileHeader.h"
#include "ProgramHeader.h"
#include "ProgramHeaderTable.h"
#include "SectionHeader.h"
#include "SectionHeaderTable.h"
#include "SectionIndexChangeMap.h"
#include "OffsetRange.h"
#include "Algorithm.h"
#include <vector>
#include <cstdint>
#include <limits>
#include <iterator>
#include <algorithm>
#include <cassert>

// #include <iostream>
// #include <QDebug>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  enum class MoveSectionAlignment
  {
    SectionAlignment, /*!< Take the alignment of the section (if any is required) */
    NextPage          /*!< Align to the next page */
  };

  /*! \internal
   */
  class FileAllHeaders
  {
   public:

    /*! \brief Set the file header
     *
     * \pre \a header must be valid
     * \pre the program header table must not allready been set
     * \pre the section header table must not allready been set
     */
    void setFileHeader(const FileHeader & header) noexcept
    {
      assert( header.seemsValid() );
      assert( !containsProgramHeaderTable() );
      assert( !containsSectionHeaderTable() );

      mFileHeader = header;
    }

    /*! \brief Check if the file header seems valid
     */
    bool fileHeaderSeemsValid() const noexcept
    {
      return mFileHeader.seemsValid();
    }

    /*! \brief Get the file header
     */
    const FileHeader & fileHeader() const noexcept
    {
      return mFileHeader;
    }

    /*! \brief Check if the program header table exists
     */
    bool containsProgramHeaderTable() const noexcept
    {
      return !mProgramHeaderTable.isEmpty();
    }

    /*! \brief Set the program header table
     */
    void setProgramHeaderTable(const ProgramHeaderTable & table) noexcept
    {
      assert( table.headerCount() <= std::numeric_limits<uint16_t>::max() );

      mProgramHeaderTable = table;
      mFileHeader.phnum = static_cast<uint16_t>( table.headerCount() );
    }

    /*! \brief Add a program header to this table
     */
    void addProgramHeader(const ProgramHeader & header) noexcept
    {
      mProgramHeaderTable.addHeader(header, fileHeader().phentsize);
      ++mFileHeader.phnum;
    }

    /*! \brief Get the program header table
     */
    const ProgramHeaderTable & programHeaderTable() const noexcept
    {
      return mProgramHeaderTable;
    }

    /*! \brief Check if the program header for the program header table exists (PT_PHDR)
     */
    bool containsProgramHeaderTableProgramHeader() const noexcept
    {
      return mProgramHeaderTable.containsProgramHeaderTableHeader();
    }

    /*! \brief Get the program header table program header (PT_PHDR)
     *
     * \pre the program header table program header must exist
     * \sa containsProgramHeaderTableProgramHeader()
     */
    const ProgramHeader & programHeaderTableProgramHeader() const noexcept
    {
      assert( containsProgramHeaderTableProgramHeader() );

      return mProgramHeaderTable.programHeaderTableHeader();
    }

    /*! \brief Check if the section header table exists
     */
    bool containsSectionHeaderTable() const noexcept
    {
      return !mSectionHeaderTable.empty();
    }

    /*! \brief Set the section header table
     */
    void setSectionHeaderTable(const std::vector<SectionHeader> & table) noexcept
    {
      assert( table.size() <= std::numeric_limits<uint16_t>::max() );

      mSectionHeaderTable = table;
      mFileHeader.shnum = static_cast<uint16_t>( table.size() );

      indexKnownSectionHeaders();
    }

    /*! \brief Set the offset of the section header table
     */
    void setSectionHeaderTableOffset(uint64_t offset) noexcept
    {
      mFileHeader.shoff = offset;
    }

    /*! \brief Get the section header table
     */
    const std::vector<SectionHeader> & sectionHeaderTable() const noexcept
    {
      return mSectionHeaderTable;
    }

    /*! \brief Check if the section header table is sorted by file offset
     */
    bool sectionHeaderTableIsSortedByFileOffset() const noexcept
    {
      return sectionHeadersAreSortedByFileOffset(mSectionHeaderTable);
    }

    /*! \brief Sort the section header table by file offset
     */
    SectionIndexChangeMap sortSectionHeaderTableByFileOffset() noexcept
    {
      const uint64_t shtStringTableOffset = mSectionHeaderTable[mFileHeader.shstrndx].offset;

      const SectionIndexChangeMap map = sortSectionHeadersByFileOffset(mSectionHeaderTable);
      indexKnownSectionHeaders();

      mFileHeader.shstrndx = findIndexOfSectionHeaderAtOffset(mSectionHeaderTable, shtStringTableOffset);

      return map;
    }

    /*! \brief Check if the .got section header exists
     */
    bool containsGotSectionHeader() const noexcept
    {
      return mIndexOfGotSectionHeader < mSectionHeaderTable.size();
    }

    /*! \brief Get the .got section header
     *
     * \pre the .got section header must exist
     * \sa containsGotSectionHeader()
     */
    const SectionHeader & gotSectionHeader() const noexcept
    {
      assert( containsGotSectionHeader() );

      return mSectionHeaderTable[mIndexOfGotSectionHeader];
    }

    /*! \brief Check if the .got.plt section header exists
     */
    bool containsGotPltSectionHeader() const noexcept
    {
      return mIndexOfGotPltSectionHeader < mSectionHeaderTable.size();
    }

    /*! \brief Get the .got.plt section header
     *
     * \pre the .got.plt section header must exist
     * \sa containsGotPltSectionHeader()
     */
    const SectionHeader & gotPltSectionHeader() const noexcept
    {
      assert( containsGotPltSectionHeader() );

      return mSectionHeaderTable[mIndexOfGotPltSectionHeader];
    }

    /*! \brief Check if the .interp program header exists
     */
    bool containsProgramInterpreterProgramHeader() const noexcept
    {
      return mProgramHeaderTable.containsProgramInterpreterProgramHeader();
    }

    /*! \brief Get the .interp program header
     *
     * \pre the .interp program header must exist
     * \sa containsProgramInterpreterProgramHeader()
     */
    const ProgramHeader & programInterpreterProgramHeader() const noexcept
    {
      assert( containsProgramInterpreterProgramHeader() );

      return mProgramHeaderTable.programInterpreterProgramHeader();
    }

    /*! \brief Check if the .interp section header exists
     */
    bool containsProgramInterpreterSectionHeader() const noexcept
    {
      return mIndexOfProgramInterpreterSectionHeader < mSectionHeaderTable.size();
    }

    /*! \brief Get the .interp section header
     *
     * \pre the .interp interp header must exist
     * \sa containsProgramInterpreterSectionHeader()
     */
    const SectionHeader & programInterpreterSectionHeader() const noexcept
    {
      assert( containsProgramInterpreterSectionHeader() );

      return mSectionHeaderTable[mIndexOfProgramInterpreterSectionHeader];
    }

    /*! \brief Check if the PT_NOTE program header exists
     */
    bool containsNoteProgramHeader() const noexcept
    {
      return mProgramHeaderTable.containsNoteProgramHeader();
    }

    /*! \brief Check if the .gnu.hash section header exists
     */
    bool containsGnuHashTableSectionHeader() const noexcept
    {
      return mIndexOfGnuHashTableSectionHeader < mSectionHeaderTable.size();
    }

    /*! \brief Get the .gnu.hash section header
     *
     * \pre the .gnu.hash section header must exist
     * \sa containsGnuHashTableSectionHeader()
     */
    const SectionHeader & gnuHashTableSectionHeader() const noexcept
    {
      assert( containsGnuHashTableSectionHeader() );

      return mSectionHeaderTable[mIndexOfGnuHashTableSectionHeader];
    }

    /*! \brief Get the PT_NOTE program header
     *
     * \pre the PT_NOTE program header must exist
     * \sa containsNoteProgramHeader()
     */
    const ProgramHeader & noteProgramHeader() const noexcept
    {
      assert( containsNoteProgramHeader() );

      return mProgramHeaderTable.noteProgramHeader();
    }

    /*! \brief Get the note section headers
     */
    std::vector<SectionHeader> getNoteSectionHeaders() const noexcept
    {
      std::vector<SectionHeader> noteSectionHeaders;

      for(const SectionHeader & header : mSectionHeaderTable){
        if(header.sectionType() == SectionType::Note){
          noteSectionHeaders.push_back(header);
        }
      }

      return noteSectionHeaders;
    }

    /*! \brief Check if the PT_GNU_RELRO program header exists
     */
    bool containsGnuRelRoProgramHeader() const noexcept
    {
      return mProgramHeaderTable.containsGnuRelRoHeader();
    }

    /*! \brief Get the PT_GNU_RELRO program header
     *
     * \pre the PT_GNU_RELRO program header must exist
     * \sa containsGnuRelRoProgramHeader()
     */
    const ProgramHeader & gnuRelRoProgramHeader() const noexcept
    {
      assert( containsGnuRelRoProgramHeader() );

      return mProgramHeaderTable.gnuRelRoHeader();
    }

    /*! \brief Get the PT_GNU_RELRO program header
     *
     * \pre the PT_GNU_RELRO program header must exist
     * \sa containsGnuRelRoProgramHeader()
     */
    ProgramHeader & gnuRelRoProgramHeaderMutable() noexcept
    {
      assert( containsGnuRelRoProgramHeader() );

      return mProgramHeaderTable.gnuRelRoHeaderMutable();
    }

    /*! \brief Set the size for the PT_GNU_RELRO program header
     *
     * \pre the PT_GNU_RELRO program header must exist
     * \sa containsGnuRelRoProgramHeader()
     */
    void setGnuRelRoProgramHeaderSize(uint64_t size) noexcept
    {
      assert( containsGnuRelRoProgramHeader() );

      mProgramHeaderTable.setGnuRelRoHeaderSize(size);
    }

    /*! \brief Check if the dynamic program header exists
     */
    bool containsDynamicProgramHeader() const noexcept
    {
      return mProgramHeaderTable.containsDynamicSectionHeader();
    }

    /*! \brief Get the dynamic program header
     *
     * \pre the dynamic program header must exist
     * \sa containsDynamicProgramHeader()
     */
    const ProgramHeader & dynamicProgramHeader() const noexcept
    {
      assert( containsDynamicProgramHeader() );

      return mProgramHeaderTable.dynamicSectionHeader();
    }

    /*! \brief Check if the section name string table header exists
     */
    bool containsSectionNameStringTableHeader() const noexcept
    {
      return mFileHeader.shstrndx > 0;
    }

    /*! \brief Set the offset of the section name string table header
     *
     * \pre the section name string table header must exist
     */
    void setSectionNameStringTableOffset(uint64_t offset) noexcept
    {
      assert( containsSectionNameStringTableHeader() );

      const size_t index = mFileHeader.shstrndx;
      assert( index < mSectionHeaderTable.size() );
      assert( mSectionHeaderTable[index].sectionType() == SectionType::StringTable );

      mSectionHeaderTable[index].offset = offset;
    }

    /*! \brief Get the section name string table header
     *
     * \pre the section name string table header must exist
     * \sa containsSectionNameStringTableHeader()
     */
    const SectionHeader & sectionNameStringTableHeader() const noexcept
    {
      assert( containsSectionNameStringTableHeader() );

      const size_t index = mFileHeader.shstrndx;
      assert( index < mSectionHeaderTable.size() );
      assert( mSectionHeaderTable[index].sectionType() == SectionType::StringTable );

      return mSectionHeaderTable[index];
    }

    /*! \brief Check if the dynamic section header exists
     */
    bool containsDynamicSectionHeader() const noexcept
    {
      return mIndexOfDynamicSectionHeader < mSectionHeaderTable.size();
    }

    /*! \brief Get the index of the dynamic section in the section header table
     *
     * \pre the dynamic section header must exist
     * \sa containsDynamicSectionHeader()
     */
    uint16_t dynamicSectionHeaderIndex() const noexcept
    {
      assert( containsDynamicSectionHeader() );

      return static_cast<uint16_t>(mIndexOfDynamicSectionHeader);
    }

    /*! \brief Get the dynamic section header
     *
     * \pre the dynamic section header must exist
     * \sa containsDynamicSectionHeader()
     */
    const SectionHeader & dynamicSectionHeader() const noexcept
    {
      assert( containsDynamicSectionHeader() );

      return mSectionHeaderTable[mIndexOfDynamicSectionHeader];
    }

    /*! \brief Check if the dynamic string table section header exists
     */
    bool containsDynamicStringTableSectionHeader() const noexcept
    {
      return mIndexOfDynamicStringTableSectionHeader < mSectionHeaderTable.size();
    }

    /*! \brief Get the index of the dynamic string table section in the section header table
     *
     * \pre the dynamic string table section header must exist
     * \sa containsDynamicStringTableSectionHeader()
     */
    uint16_t dynamicStringTableSectionHeaderIndex() const noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      return static_cast<uint16_t>(mIndexOfDynamicStringTableSectionHeader);
    }

    /*! \brief Get the dynamic string table section header
     *
     * \pre the dynamic string table section header must exist
     * \sa containsDynamicStringTableSectionHeader()
     */
    const SectionHeader & dynamicStringTableSectionHeader() const noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      const size_t index = mIndexOfDynamicStringTableSectionHeader;
      assert( index < mSectionHeaderTable.size() );
      assert( mSectionHeaderTable[index].sectionType() == SectionType::StringTable );

      return mSectionHeaderTable[index];
    }

    /*! \brief Set the size of the dynamic section
     *
     * \pre the dynamic program header must exist
     * \sa containsDynamicProgramHeader()
     * \pre the dynamic section header must exist
     * \sa containsDynamicSectionHeader()
     */
    void setDynamicSectionSize(uint64_t size) noexcept
    {
      assert( containsDynamicProgramHeader() );
      assert( containsDynamicSectionHeader() );

      mProgramHeaderTable.setDynamicSectionSize(size);
      mSectionHeaderTable[mIndexOfDynamicSectionHeader].size = size;
    }

    /*! \brief Move the dynamic section to the end
     *
     * \pre the file header must be valid
     * \pre the dynamic program header must exist
     * \sa containsDynamicProgramHeader()
     * \pre the dynamic section header must exist
     * \sa containsDynamicSectionHeader()
     */
    void moveDynamicSectionToEnd(MoveSectionAlignment alignmentMode) noexcept
    {
      assert( fileHeaderSeemsValid() );
      assert( containsDynamicProgramHeader() );
      assert( containsDynamicSectionHeader() );

      const uint64_t alignment = sectionAlignemnt(dynamicSectionHeader().addralign, alignmentMode);

      const uint64_t virtualAddess = findNextAlignedAddress(findGlobalVirtualAddressEnd(), alignment);
      const uint64_t fileOffset = findNextFileOffset( findGlobalFileOffsetEnd(), virtualAddess, fileHeader().pageSize() );

      mProgramHeaderTable.setDynamicSectionVirtualAddressAndFileOffset(virtualAddess, fileOffset);
      mSectionHeaderTable[mIndexOfDynamicSectionHeader].addr = virtualAddess;
      mSectionHeaderTable[mIndexOfDynamicSectionHeader].offset = fileOffset;
    }

    /*! \brief Set the size of the dynamic string table
     *
     * \pre the dynamic string table section header must exist
     * \sa containsDynamicStringTableSectionHeader()
     */
    void setDynamicStringTableSize(uint64_t size) noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      mSectionHeaderTable[mIndexOfDynamicStringTableSectionHeader].size = size;
    }

    /*! \brief Move the dynamic string table to the end
     *
     * \pre the dynamic string table section header must exist
     * \sa containsDynamicStringTableSectionHeader()
     */
    void moveDynamicStringTableToEnd(MoveSectionAlignment alignmentMode) noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      const uint64_t alignment = sectionAlignemnt(dynamicStringTableSectionHeader().addralign, alignmentMode);

      uint64_t virtualAddess;
      uint64_t fileOffset;
      if(alignmentMode == MoveSectionAlignment::NextPage){
        assert(alignment > 1);
        virtualAddess = findNextAlignedAddress(findGlobalVirtualAddressEnd(), alignment);
        fileOffset = findNextFileOffset( findGlobalFileOffsetEnd(), virtualAddess, fileHeader().pageSize() );
      }else{
        virtualAddess = findGlobalVirtualAddressEnd();
        if( (virtualAddess % 2) != 0){
          ++virtualAddess;
        }
        fileOffset = findGlobalFileOffsetEnd();
      }

      mSectionHeaderTable[mIndexOfDynamicStringTableSectionHeader].addr = virtualAddess;
      mSectionHeaderTable[mIndexOfDynamicStringTableSectionHeader].offset = fileOffset;
    }

    /*! \brief Move the program header table to the end
     *
     * The virtual address of the program header table
     * will be aligned to the next page past the end.
     * Its file offset will also be updated.
     *
     * \pre the file header must be valid
     * \pre the program header table, and also its related segment, must exist
     * \sa containsProgramHeaderTable()
     * \sa containsProgramHeaderTableProgramHeader()
     */
    void moveProgramHeaderTableToNextPageAfterEnd() noexcept
    {
      assert( fileHeaderSeemsValid() );
      assert( containsProgramHeaderTable() );
      assert( containsProgramHeaderTableProgramHeader() );

      /// \todo check if this should be as the other PT_LOAD ? f.ex. 0x200000 maybe a argument ??
//       const uint64_t pageSize = 0x200000;
      const uint64_t pageSize = mFileHeader.pageSize();

      const uint64_t lastVirtualAddress = findGlobalVirtualAddressEnd();
      const uint64_t lastFileOffset = findGlobalFileOffsetEnd();

      const uint64_t virtualAddess = findAddressOfNextPage(std::max(lastVirtualAddress, lastFileOffset), pageSize);
      /*
       * We could place the program header table to the end of the file,
       * to a offset that is congruent to the virtual addres modulo page size:
       * const uint64_t fileOffset = findNextFileOffset(findGlobalFileOffsetEnd(), virtualAddess, pageSize);
       *
       * Sadly, this will not work.
       * The file offset must be the same as the virtual address for the program header table.
       *
       * See also:
       * - https://github.com/NixOS/patchelf/blob/master/BUGS
       * - https://github.com/NixOS/patchelf/pull/117
       */
      const uint64_t fileOffset = virtualAddess;

      mProgramHeaderTable.setProgramHeaderTableHeaderVirtualAddressAndFileOffset(virtualAddess, fileOffset);
      mFileHeader.phoff = fileOffset;
    }

    /*! \brief Move the program interpreter section to the end
     *
     * \pre the file header must be valid
     * \pre the .interp section header must exist
     * \pre the PT_INTERP programe header must exist
     */
    void moveProgramInterpreterSectionToEnd(MoveSectionAlignment alignmentMode) noexcept
    {
      assert( fileHeaderSeemsValid() );
      assert( containsProgramInterpreterSectionHeader() );
      assert( containsProgramInterpreterProgramHeader() );

      const uint64_t alignment = sectionAlignemnt(programInterpreterSectionHeader().addralign, alignmentMode);

      const uint64_t lastVirtualAddress = findGlobalVirtualAddressEnd();
      const uint64_t lastFileOffset = findGlobalFileOffsetEnd();

      const uint64_t virtualAddess = findNextAlignedAddress(lastVirtualAddress, alignment);
      const uint64_t fileOffset = findNextFileOffset( lastFileOffset, virtualAddess, mFileHeader.pageSize() );

      mSectionHeaderTable[mIndexOfProgramInterpreterSectionHeader].addr = virtualAddess;
      mSectionHeaderTable[mIndexOfProgramInterpreterSectionHeader].offset = fileOffset;

      mProgramHeaderTable.setProgramInterpreterHeaderVirtualAddressAndFileOffset(virtualAddess, fileOffset);
    }

    /*! \brief Move the note sections to the end
     *
     * \pre the file header must be valid
     * \pre the PT_NOTE programe header must exist
     */
    void moveNoteSectionsToEnd(MoveSectionAlignment alignmentMode) noexcept
    {
      assert( fileHeaderSeemsValid() );
      assert( containsNoteProgramHeader() );

      const uint64_t alignment = sectionAlignemnt(noteProgramHeader().align, alignmentMode);

      const uint64_t firstVirtualAddess = findNextAlignedAddress(findGlobalVirtualAddressEnd(), alignment);
      const uint64_t firstFileOffset = findNextFileOffset( findGlobalFileOffsetEnd(), firstVirtualAddess, fileHeader().pageSize() );

      uint64_t virtualAddess = firstVirtualAddess;
      uint64_t fileOffset = firstFileOffset;

      for(SectionHeader & header : mSectionHeaderTable){
        if(header.sectionType() == SectionType::Note){
          header.addr = virtualAddess;
          header.offset = fileOffset;
          virtualAddess += header.size;
          fileOffset += header.size;
        }
      }

      mProgramHeaderTable.setNoteProgramHeaderVirtualAddressAndFileOffset(firstVirtualAddess, firstFileOffset);
    }

    /*! \brief Move the .gnu.hash section to the end
     *
     * \pre the file header must be valid
     * \pre the .gnu.hash section header must exist
     * \sa containsGnuHashTableSectionHeader()
     */
    void moveGnuHashTableToEnd(MoveSectionAlignment alignmentMode) noexcept
    {
      assert( fileHeaderSeemsValid() );
      assert( containsGnuHashTableSectionHeader() );

      const uint64_t alignment = sectionAlignemnt(gnuHashTableSectionHeader().addralign, alignmentMode);

      const uint64_t virtualAddess = findNextAlignedAddress(findGlobalVirtualAddressEnd(), alignment);
      const uint64_t fileOffset = findNextFileOffset( findGlobalFileOffsetEnd(), virtualAddess, fileHeader().pageSize() );

      mSectionHeaderTable[mIndexOfGnuHashTableSectionHeader].addr = virtualAddess;
      mSectionHeaderTable[mIndexOfGnuHashTableSectionHeader].offset = fileOffset;
    }

    /*! \brief Find the global virtual address end
     */
    uint64_t findGlobalVirtualAddressEnd() const noexcept
    {
      assert( fileHeaderSeemsValid() );

      uint64_t lastSegmentVirtalAddressEnd = 0;
      if( !mProgramHeaderTable.isEmpty() ){
        lastSegmentVirtalAddressEnd = mProgramHeaderTable.findLastSegmentVirtualAddressEnd();
      }

      uint64_t lastSectionVirtualAddressEnd = 0;
      if( !mSectionHeaderTable.empty() ){
        const auto cmp = [](const SectionHeader & a, const SectionHeader & b){
          return a.virtualAddressEnd() < b.virtualAddressEnd();
        };

        const auto it = std::max_element(mSectionHeaderTable.cbegin(), mSectionHeaderTable.cend(), cmp);
        assert( it != mSectionHeaderTable.cend() );

        lastSectionVirtualAddressEnd = it->virtualAddressEnd();
      }

      return std::max(lastSegmentVirtalAddressEnd, lastSectionVirtualAddressEnd);
    }

    /*! \brief Get the global file offset end
     *
     * \pre the file header must be valid
     */
    uint64_t findGlobalFileOffsetEnd() const noexcept
    {
      assert( fileHeaderSeemsValid() );

      uint64_t segmentsOffsetEnd = 0;
      if( !mProgramHeaderTable.isEmpty() ){
        segmentsOffsetEnd = mProgramHeaderTable.findLastSegmentFileOffsetEnd();
      }

      uint64_t sectionsOffsetEnd = 0;
      if( !mSectionHeaderTable.empty() ){
        const auto cmp = [](const SectionHeader & a, const SectionHeader & b){
          return a.fileOffsetEnd() < b.fileOffsetEnd();
        };

        const auto it = std::max_element(mSectionHeaderTable.cbegin(), mSectionHeaderTable.cend(), cmp);
        assert( it != mSectionHeaderTable.cend() );

        sectionsOffsetEnd = it->fileOffsetEnd();
      }

      const uint64_t lastHeaderEnd = static_cast<uint64_t>( minimumSizeToAccessAllHeaders() );

      return std::max({segmentsOffsetEnd, sectionsOffsetEnd, lastHeaderEnd});
    }

    /*! \brief Get the global file offsets range
     *
     * \pre the file header must be valid
     */
    OffsetRange globalFileOffsetRange() const noexcept
    {
      assert( fileHeaderSeemsValid() );

      const uint64_t fileEnd  = findGlobalFileOffsetEnd();

      return OffsetRange::fromBeginAndEndOffsets(0, fileEnd);
    }

    /*! \brief Get the minimum size to access all headers
     *
     * \pre the file header must be valid
     */
    int64_t minimumSizeToAccessAllHeaders() const noexcept
    {
      assert( fileHeaderSeemsValid() );

      return std::max( mFileHeader.minimumSizeToReadAllProgramHeaders(), mFileHeader.minimumSizeToReadAllSectionHeaders() );
    }

    /*! \brief Check about validity
     */
    bool seemsValid() const noexcept
    {
      if( !fileHeaderSeemsValid() ){
        return false;
      }
      if( mFileHeader.phnum != mProgramHeaderTable.headerCount() ){
        return false;
      }
      if( mFileHeader.shnum != mSectionHeaderTable.size() ){
        return false;
      }
      if( containsDynamicSectionHeader() && !containsDynamicProgramHeader() ){
        return false;
      }

      return true;
    }

   private:

    uint64_t sectionAlignemnt(uint64_t alignment, MoveSectionAlignment alignmentMode) const noexcept
    {
      switch(alignmentMode){
        case MoveSectionAlignment::SectionAlignment:
          break;
        case MoveSectionAlignment::NextPage:
          alignment = mFileHeader.pageSize();
          break;
      }

      if(alignment == 0){
        alignment = 1;
      }

      return alignment;
    }

    static
    constexpr size_t invalidSectionHeaderIndex() noexcept
    {
      return std::numeric_limits<size_t>::max();
    }

    static
    bool isGotSectionHeader(const SectionHeader & header) noexcept
    {
      if(header.sectionType() != SectionType::ProgramData){
        return false;
      }

      return header.name == ".got";
    }

    static
    bool isGotPltSectionHeader(const SectionHeader & header) noexcept
    {
      if(header.sectionType() != SectionType::ProgramData){
        return false;
      }

      return header.name == ".got.plt";
    }

    static
    bool isNoteSectionHeaderWithName(const SectionHeader & header, const std::string & name) noexcept
    {
      if(header.sectionType() != SectionType::Note){
        return false;
      }

      return header.name == name;
    }

    static
    bool isNoteAbiTagSectionHeader(const SectionHeader & header) noexcept
    {
      return isNoteSectionHeaderWithName(header, ".note.ABI-tag");
    }

    static
    bool isNoteGnuBuildIdSectionHeader(const SectionHeader & header) noexcept
    {
      return isNoteSectionHeaderWithName(header, ".note.gnu.build-id");
    }

    static
    bool isDynamicSectionHeader(const SectionHeader & header) noexcept
    {
      return header.sectionType() == SectionType::Dynamic;
    }

    static
    bool isDynamicStringTableSectionHeader(const SectionHeader & header) noexcept
    {
      if( header.sectionType() != SectionType::StringTable ){
        return false;
      }

      return header.name == ".dynstr";
    }

    void indexKnownSectionHeaders() noexcept
    {
      for(size_t i=1; i < mSectionHeaderTable.size(); ++i){
        const SectionHeader & header = mSectionHeaderTable[i];

        if( isGotSectionHeader(header) ){
          mIndexOfGotSectionHeader = i;
        }else if( isGotPltSectionHeader(header) ){
          mIndexOfGotPltSectionHeader = i;
        }else if( isDynamicSectionHeader(header) ){
          mIndexOfDynamicSectionHeader = i;
        }else if( header.isProgramInterpreterSectionHeader() ){
          mIndexOfProgramInterpreterSectionHeader = i;
        }else if( header.isGnuHashTableSectionHeader() ){
          mIndexOfGnuHashTableSectionHeader = i;
        }else if( isDynamicStringTableSectionHeader(header) ){
          mIndexOfDynamicStringTableSectionHeader = i;
        }
      }

      /** \todo when implementing section header table,
       * it should be possible to check (by the user),
       * if the .dynamic section links properly to the .dynstr section.
       * AND: avoid current double implementation
       * of indexing .dynstr
       */
      setIndexOfDynamicStringTableSectionHeader();
    }

    void setIndexOfDynamicStringTableSectionHeader() noexcept
    {
      if( containsDynamicSectionHeader() && ( dynamicSectionHeader().linkIsIndexInSectionHeaderTable() ) ){
        mIndexOfDynamicStringTableSectionHeader = dynamicSectionHeader().link;
        assert( mIndexOfDynamicStringTableSectionHeader < mSectionHeaderTable.size() );
        assert( isDynamicStringTableSectionHeader( mSectionHeaderTable[mIndexOfDynamicStringTableSectionHeader] ) );
      }
    }

    size_t mIndexOfDynamicSectionHeader = invalidSectionHeaderIndex();
    size_t mIndexOfDynamicStringTableSectionHeader = invalidSectionHeaderIndex();
    size_t mIndexOfGotSectionHeader = invalidSectionHeaderIndex();
    size_t mIndexOfGotPltSectionHeader = invalidSectionHeaderIndex();
    size_t mIndexOfProgramInterpreterSectionHeader = invalidSectionHeaderIndex();
    size_t mIndexOfGnuHashTableSectionHeader = invalidSectionHeaderIndex();
    FileHeader mFileHeader;
    ProgramHeaderTable mProgramHeaderTable;
    std::vector<SectionHeader> mSectionHeaderTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_H
