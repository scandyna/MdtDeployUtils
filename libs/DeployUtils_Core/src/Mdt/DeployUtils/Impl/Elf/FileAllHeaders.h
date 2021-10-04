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
#include "OffsetRange.h"
#include <vector>
#include <cstdint>
#include <limits>
#include <iterator>
#include <algorithm>
#include <cassert>

// #include <QDebug>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal Check if a segment can only hold sections allocating memory
   *
   * \todo missing type >= PT_GNU_MBIND_LO and type <= PT_GNU_MBIND_HI
   */
  inline
  bool segmentOnlyContainsSectionsAllocatingMemory(SegmentType segmentType) noexcept
  {
    switch(segmentType){
      case SegmentType::Load:
      case SegmentType::Dynamic:
      case SegmentType::GnuEhFrame:
      case SegmentType::GnuStack:
      case SegmentType::GnuRelo:
        return true;
      default:
        return false;
    }
//     return false;
  }

  /*! \internal
   *
   * \sa sectionIsInSegmentStrict
   */
  inline
  bool segmentCanContainSection(const ProgramHeader & programHeader, const SectionHeader & sectionHeader) noexcept
  {
    const SegmentType segmentType = programHeader.segmentType();

    // Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain SHF_TLS sections.
    if( sectionHeader.holdsTls() ){
      switch(segmentType){
        case SegmentType::Load:
///         case SegmentType::Relro:
        case SegmentType::Tls:
          break;
        default:
          return false;
      }
    }

    // PT_TLS segment contains only SHF_TLS sections.
    if(segmentType == SegmentType::Tls){
      if( !sectionHeader.holdsTls() ){
        return false;
      }
    }

    // PT_PHDR contains no section
    if(segmentType == SegmentType::ProgramHeaderTable){
      return false;
    }

    // PT_LOAD and similar segments only have SHF_ALLOC sections.
    if( segmentOnlyContainsSectionsAllocatingMemory( programHeader.segmentType() ) ){
      if( !sectionHeader.allocatesMemory() ){
        return false;
      }
    }

    return true;
  }

  /*! \internal
   *
   * .tbss is special.
   * It doesn't contribute memory space to normal segments
   * and it doesn't take file space in normal segments.
   */
  inline
  bool isSpecialTbss(const SectionHeader & sectionHeader, const ProgramHeader & programHeader) noexcept
  {
    if( !sectionHeader.holdsTls() ){
      return false;
    }
    if(sectionHeader.sectionType() != SectionType::NoBits){
      return false;
    }
    if(programHeader.segmentType() == SegmentType::Tls){
      return false;
    }

    return true;
  }

  /*! \internal
   */
  inline
  uint64_t elfSectionSize(const SectionHeader & sectionHeader, const ProgramHeader & programHeader) noexcept
  {
    if( isSpecialTbss(sectionHeader, programHeader) ){
      return 0;
    }

    return sectionHeader.size;
  }

  /*! \internal
   *
   * This code comes from the ELF_SECTION_IN_SEGMENT_1 macro,
   * part commented: "Any section besides one of type SHT_NOBITS must have file offsets within the segment.",
   * but:
   * - it is not checked that the section is not SHT_NOBITS
   * - strict mode is ON
   * - check VMA is ON
   *
   * The original macro is here: binutils-gdb/include/elf/internal.h
   */
  inline
  bool fileOffsetsAreWithinSegment(const SectionHeader & sectionHeader, const ProgramHeader & programHeader) noexcept
  {
    if(sectionHeader.offset < programHeader.offset){
      return false;
    }
    assert(sectionHeader.offset >= programHeader.offset);

    if( (sectionHeader.offset - programHeader.offset) >= programHeader.filesz ){
      return false;
    }

    if( (sectionHeader.offset - programHeader.offset + elfSectionSize(sectionHeader, programHeader)) > programHeader.filesz ){
      return false;
    }

    return true;
  }

    /* Any section besides one of type SHT_NOBITS must have file offsets within the segment. */
//     sh_type == SHT_NOBITS
//     || (sh_offset >= p_offset
//         && (sh_offset - p_offset < p_filesz)
//         && ( (sh_offset - p_offset + ELF_SECTION_SIZE(sec_hdr, segment) ) <= p_filesz) )

  /*! \internal
   *
   * This code comes from the ELF_SECTION_IN_SEGMENT_1 macro,
   * part commented: "SHF_ALLOC sections must have VMAs within the segment.",
   * but:
   * - it is not checked that the section has the SHF_ALLOC flag
   * - strict mode is ON
   * - check VMA is ON
   *
   * The original macro is here: binutils-gdb/include/elf/internal.h
   */
  inline
  bool vmasAreWithinSegment(const SectionHeader & sectionHeader, const ProgramHeader & programHeader) noexcept
  {
    if(sectionHeader.addr < programHeader.vaddr){
      return false;
    }
    assert(sectionHeader.addr >= programHeader.vaddr);

    if( (sectionHeader.addr - programHeader.vaddr) >= programHeader.memsz ){
      return false;
    }

    if( (sectionHeader.addr - programHeader.vaddr + elfSectionSize(sectionHeader, programHeader)) > programHeader.memsz ){
      return false;
    }

    return true;
  }

  /* SHF_ALLOC sections must have VMAs within the segment.  */
//   (sh_flags & SHF_ALLOC) == 0
//   || ( sh_addr >= p_vaddr && (sh_addr - p_vaddr < p_memsz) )
//     && ( (sh_addr - p_vaddr + ELF_SECTION_SIZE(sec_hdr, segment)) <= p_memsz)

  /*! \internal Check if section (described by \a sectionHeader) is in a segment (described by \a programHeader)
   *
   * This code comes from the ELF_SECTION_IN_SEGMENT_STRICT macro
   * from the binutils-gdb/include/elf/internal.h
   *
   * \todo move somewhere else ?
   */
  inline
  bool sectionIsInSegmentStrict(const SectionHeader & sectionHeader, const ProgramHeader & programHeader) noexcept
  {
    if( !segmentCanContainSection(programHeader, sectionHeader) ){
      return false;
    }

    // Any section besides one of type SHT_NOBITS must have file offsets within the segment
    if(sectionHeader.sectionType() != SectionType::NoBits){
      if( !fileOffsetsAreWithinSegment(sectionHeader, programHeader) ){
        return false;
      }
    }

    // SHF_ALLOC sections must have VMAs within the segment.
    if( sectionHeader.allocatesMemory() ){
      if( !vmasAreWithinSegment(sectionHeader, programHeader) ){
        return false;
      }
    }

    const SegmentType segmentType = programHeader.segmentType();

    // No zero size sections at start or end of PT_DYNAMIC nor PT_NOTE
    if( (segmentType == SegmentType::Dynamic) || (segmentType == SegmentType::Note) ){
      if(sectionHeader.size == 0){
        return false;
      }
      if(programHeader.memsz == 0){
        return true;
      }
      if(sectionHeader.sectionType() != SectionType::NoBits){
        if( !fileOffsetsAreWithinSegment(sectionHeader, programHeader) ){
          return false;
        }
      }
      if( sectionHeader.allocatesMemory() ){
        if( !vmasAreWithinSegment(sectionHeader, programHeader) ){
          return false;
        }
      }
    }

    /* No zero size sections at start or end of PT_DYNAMIC nor PT_NOTE */
//     (p_type != PT_DYNAMIC	&& p_type != PT_NOTE)
//        || sh_size != 0
//        || p_memsz == 0
//        || ( (sh_type == SHT_NOBITS
//             || (sh_offset > p_offset && (sh_offset - p_offset < p_filesz)) )
//             && ( (sh_flags & SHF_ALLOC) == 0 || (sh_addr > p_vaddr && (sh_addr - p_vaddr < p_memsz))) )

    return true;
  }

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
//       setIndexOfDynamicSectionProgramHeader();
    }

    /*! \brief Get the program header table
     */
    const ProgramHeaderTable & programHeaderTable() const noexcept
    {
      return mProgramHeaderTable;
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
      setIndexOfDynamicSectionHeader();
      setIndexOfDynamicStringTableSectionHeader();
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
//       const size_t index = mIndexOfDynamicSectionProgramHeader;
//       assert( index < mProgramHeaderTable.size() );
//       assert( mProgramHeaderTable[index].segmentType() == SegmentType::Dynamic );
// 
//       return mProgramHeaderTable[index];
    }

    /*! \brief Set the dynamic program header
     *
     * Because current implementation does not support creating
     * correct program headers,
     * it must be readen from the file first and be set before.
     * \todo what ? and it is replaced in the implementation..
     *
     * \pre the dynamic program header must exist
     * \sa containsDynamicProgramHeader()
     */
//     [[deprecated]]
//     void setDynamicProgramHeader(const ProgramHeader & header) noexcept
//     {
//       assert( containsDynamicProgramHeader() );
//       assert( header.segmentType() == SegmentType::Dynamic );
// 
//       /**
//       const size_t index = mIndexOfDynamicSectionProgramHeader;
//       assert( index < mProgramHeaderTable.size() );
//       assert( mProgramHeaderTable[index].segmentType() == SegmentType::Dynamic );
// 
//       mProgramHeaderTable[index] = header;
//       */
//     }

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
      return mIndexOfDynamicSectionHeader > 0;
    }

    /*! \brief Get the dynamic section header
     *
     * \pre the dynamic section header must exist
     * \sa containsDynamicSectionHeader()
     */
    const SectionHeader & dynamicSectionHeader() const noexcept
    {
      assert( containsDynamicSectionHeader() );

      const size_t index = mIndexOfDynamicSectionHeader;
      assert( index < mSectionHeaderTable.size() );
      assert( mSectionHeaderTable[index].sectionType() == SectionType::Dynamic );

      return mSectionHeaderTable[index];
    }

    /*! \brief Set the dynamic section header
     *
     * Because current implementation does not support creating
     * correct section headers,
     * it must be readen from the file first and be set before.
     *
     * \pre the dynamic section header must exist
     * \sa containsDynamicSectionHeader()
     * \pre the link must not change
     */
    void setDynamicSectionHeader(const SectionHeader & header) noexcept
    {
      assert( containsDynamicSectionHeader() );
      assert( header.sectionType() == SectionType::Dynamic );
      assert( header.link == dynamicSectionHeader().link );

      const size_t index = mIndexOfDynamicSectionHeader;
      assert( index < mSectionHeaderTable.size() );
      assert( mSectionHeaderTable[index].sectionType() == SectionType::Dynamic );

      mSectionHeaderTable[index] = header;
    }

    /*! \brief Check if the dynamic string table section header exists
     */
    bool containsDynamicStringTableSectionHeader() const noexcept
    {
      return mIndexOfDynamicStringTableSectionHeader > 0;
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

    /*! \brief Set the dynamic string table section header
     *
     * Because current implementation does not support creating
     * correct section headers,
     * it must be readen from the file first and be set before.
     *
     * \pre the dynamic string table section header must exist
     * \sa containsDynamicStringTableSectionHeader()
     */
    void setDynamicStringTableSectionHeader(const SectionHeader & header) noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );
      assert( header.sectionType() == SectionType::StringTable );

      const size_t index = mIndexOfDynamicStringTableSectionHeader;
      assert( index < mSectionHeaderTable.size() );
      assert( mSectionHeaderTable[index].sectionType() == SectionType::StringTable );

      mSectionHeaderTable[index] = header;
    }

    /*! \brief Set the size of the dynamic section
     *
     * \pre the dynamic program header must exist
     * \sa containsDynamicProgramHeader()
     * \pre the dynamic section header must exist
     * \sa containsDynamicSectionHeader()
     */
    void setDynamicSectionFileSize(uint64_t size) noexcept
    {
      assert( containsDynamicProgramHeader() );
      assert( containsDynamicSectionHeader() );

      mProgramHeaderTable.setDynamicSectionFileSize(size);
      mSectionHeaderTable[mIndexOfDynamicSectionHeader].size = size;
    }

    /*! \brief Set the file size of the dynamic string table
     *
     * \pre the dynamic string table section header must exist
     * \sa containsDynamicStringTableSectionHeader()
     */
    void setDynamicStringTableFileSize(uint64_t size) noexcept
    {
      assert( containsDynamicStringTableSectionHeader() );

      mSectionHeaderTable[mIndexOfDynamicStringTableSectionHeader].size = size;
    }

    /*! \brief Shift the offset of program hedares after \a refOffset by \a offset
     *
     * \todo use OffsetChange
     */
    void shiftProgramHeadersOffsetAfter(uint64_t refOffset, int64_t offset) noexcept
    {
      mProgramHeaderTable.shiftHeadersOffsetAfter(refOffset, offset);
//       qDebug() << "shift program headers by " << offset;
//       for(auto & header : mProgramHeaderTable){
//         if(header.offset > refOffset){
// //           qDebug() << " shift " << header.offset;
//           header.offset += offset;
// //           qDebug() << "  new: " << header.offset;
//         }
//       }
    }

    /*! \brief Shift the offset of program hedares after \a ref by \a offset
     *
     * \todo use OffsetChange
     */
    void shiftProgramHeadersOffsetAfter(const ProgramHeader & ref, int64_t offset) noexcept
    {
      shiftProgramHeadersOffsetAfter(ref.offset, offset);
//       qDebug() << "shift program headers by " << offset;
//       
//       for(auto & header : mProgramHeaderTable){
//         if(header.offset > ref.offset){
//           qDebug() << " shift " << header.offset;
//           header.offset += offset;
//           qDebug() << "  new: " << header.offset;
//         }
//       }
    }

    /*! \brief Shift the offset of section hedares after \a ref by \a offset
     *
     * \todo use OffsetChange
     */
    void shiftSectionHeadersOffsetAfter(const SectionHeader & ref, int64_t offset) noexcept
    {
//       qDebug() << "shift section headers by " << offset;
      
      for(auto & header : mSectionHeaderTable){
        if(header.offset > ref.offset){
//           qDebug() << " shift " << header.offset << " (" << QString::fromStdString(header.name) << ")";
          header.offset += offset;
//           qDebug() << "  new: " << header.offset;
        }
      }
      
//       for(const auto & header : mSectionHeaderTable){
//         qDebug() << QString::fromStdString(header.name) << ": " << header.offset;
//       }
    }

    /*! \brief Get the global file offsets range
     *
     * \pre the file header must be valid
     */
    OffsetRange globalFileOffsetRange() const noexcept
    {
      assert( fileHeaderSeemsValid() );

      uint64_t dynamicSectionEnd = 0;
      if( containsDynamicProgramHeader() ){
        dynamicSectionEnd = dynamicProgramHeader().offset + dynamicProgramHeader().filesz;
      }

      uint64_t dynamicStringTableEnd = 0;
      if( containsDynamicStringTableSectionHeader() ){
        dynamicStringTableEnd = dynamicStringTableSectionHeader().offset + dynamicStringTableSectionHeader().size;
      }

      uint64_t lastHeaderEnd = static_cast<uint64_t>( minimumSizeToAccessAllHeaders() );

      uint64_t fileEnd = std::max({dynamicSectionEnd, dynamicStringTableEnd, lastHeaderEnd});

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

//     void setIndexOfDynamicSectionProgramHeader() noexcept
//     {
//       const auto pred = [](const ProgramHeader & header){
//         return header.segmentType() == SegmentType::Dynamic;
//       };
// 
//       const auto it = std::find_if(mProgramHeaderTable.cbegin(), mProgramHeaderTable.cend(), pred);
//       if( it == mProgramHeaderTable.cend() ){
//         mIndexOfDynamicSectionProgramHeader = -1;
//       }else{
//         mIndexOfDynamicSectionProgramHeader = static_cast<int>( std::distance(mProgramHeaderTable.cbegin(), it) );
//       }
//     }

    void setIndexOfDynamicSectionHeader() noexcept
    {
      const auto pred = [](const SectionHeader & header){
        return header.sectionType() == SectionType::Dynamic;
      };

      const auto it = std::find_if(mSectionHeaderTable.cbegin(), mSectionHeaderTable.cend(), pred);
      if( it == mSectionHeaderTable.cend() ){
        mIndexOfDynamicSectionHeader = 0;
      }else{
        mIndexOfDynamicSectionHeader = static_cast<int>( std::distance(mSectionHeaderTable.cbegin(), it) );
      }
    }

    void setIndexOfDynamicStringTableSectionHeader() noexcept
    {
      if( containsDynamicSectionHeader() ){
        mIndexOfDynamicStringTableSectionHeader = dynamicSectionHeader().link;
      }
    }

//     int mIndexOfDynamicSectionProgramHeader = -1;
    int mIndexOfDynamicSectionHeader = 0;
    int mIndexOfDynamicStringTableSectionHeader = 0;
    FileHeader mFileHeader;
    ProgramHeaderTable mProgramHeaderTable;
    std::vector<SectionHeader> mSectionHeaderTable;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_ALL_HEADERS_H
