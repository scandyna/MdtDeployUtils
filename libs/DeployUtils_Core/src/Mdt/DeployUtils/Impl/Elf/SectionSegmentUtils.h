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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_SEGMENT_UTILS_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_SEGMENT_UTILS_H

#include "ProgramHeader.h"
#include "ProgramHeaderTable.h"
#include "SectionHeader.h"
#include "SectionHeaderTable.h"
#include <vector>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <cassert>

// #include <iostream>

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
      case SegmentType::GnuRelRo:
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

  /*! \internal Set the offset, addresses and sizes to a program header so that it covers given section headers
   *
   * \pre \a sectionHeaders must contain at least 1 section header
   */
  inline
  void setProgramHeaderCoveringSections(ProgramHeader & programHeader, const std::vector<SectionHeader> & sectionHeaders) noexcept
  {
    assert( !sectionHeaders.empty() );

    constexpr uint64_t maxValue = std::numeric_limits<uint64_t>::max();

    uint64_t startOffset = maxValue;
    uint64_t startAddress = maxValue;
    uint64_t lastSectionOffset = 0;
    uint64_t lastSectionAddress = 0;
    uint64_t lastSectionSize = 0;
    bool containsWritableSection = false;

    for(const SectionHeader & header : sectionHeaders){
      startOffset = std::min(startOffset, header.offset);
      startAddress = std::min(startAddress, header.addr);
      if(header.offset > lastSectionOffset){
        assert(header.addr > lastSectionAddress);
        lastSectionOffset = header.offset;
        lastSectionAddress = header.addr;
        lastSectionSize = header.size;
      }
      if( header.isWritable() ){
        containsWritableSection = true;
      }
    }
    assert(startOffset < maxValue);
    assert(startAddress < maxValue);
    assert(lastSectionOffset >= startOffset);
    assert(lastSectionAddress >= startAddress);

    const uint64_t fileSize = lastSectionOffset + lastSectionSize - startOffset;
    const uint64_t memorySize = lastSectionAddress + lastSectionSize - startAddress;

    programHeader.offset = startOffset;
    programHeader.vaddr = startAddress;
    programHeader.paddr = startAddress;
    programHeader.filesz = fileSize;
    programHeader.memsz = memorySize;
    if(containsWritableSection){
      programHeader.setPermissions(SegmentPermission::Read | SegmentPermission::Write);
    }else{
      programHeader.setPermissions(SegmentPermission::Read);
    }
  }

  /*! \internal Set the offset, addresses and sizes to a program header so that it covers given section headers
   *
   * \pre \a sectionHeadersIndexes must contain at least 1 index to the section header table
   */
  inline
  void setProgramHeaderCoveringSections(ProgramHeader & programHeader, const std::vector<uint16_t> sectionHeadersIndexes,
                                        const std::vector<SectionHeader> & sectionHeaderTable) noexcept
  {
    assert( !sectionHeadersIndexes.empty() );

    std::vector<SectionHeader> sectionHeaders;
    sectionHeaders.reserve( sectionHeadersIndexes.size() );
    for(const uint16_t index : sectionHeadersIndexes){
      assert( index < sectionHeaderTable.size() );
      sectionHeaders.push_back(sectionHeaderTable[index]);
    }

    setProgramHeaderCoveringSections(programHeader, sectionHeaders);
  }

  /*! \internal Set the sizes to a program header so that it also covers given sections
   *
   * \pre \a sectionHeaders must contain at least 1 section header
   * \pre each section defined in \a sectionHeaders must start at least at offset and address defined by \a programHeader
   */
  inline
  void extendProgramHeaderSizeToCoverSections(ProgramHeader & programHeader, const std::vector<SectionHeader> & sectionHeaders) noexcept
  {
    assert( !sectionHeaders.empty() );

    const auto aLessThanB_ByAddress = [](const SectionHeader & a, const SectionHeader & b){
      return a.addr < b.addr;
    };
    const auto lastSectionIt_ByAddress = std::max_element(sectionHeaders.cbegin(), sectionHeaders.cend(), aLessThanB_ByAddress);

    const auto aLessThanB_ByOffset = [](const SectionHeader & a, const SectionHeader & b){
      return a.offset < b.offset;
    };
    const auto lastSectionIt_ByOffset = std::max_element(sectionHeaders.cbegin(), sectionHeaders.cend(), aLessThanB_ByOffset);

    assert( lastSectionIt_ByAddress != sectionHeaders.cend() );
    assert( lastSectionIt_ByOffset != sectionHeaders.cend() );

    assert( lastSectionIt_ByAddress->addr >= programHeader.vaddr );
    const uint64_t segmentMemorySize = lastSectionIt_ByAddress->virtualAddressEnd() - programHeader.vaddr;

    assert( lastSectionIt_ByOffset->offset >= programHeader.offset );
    const uint64_t segmentFileSize = lastSectionIt_ByOffset->fileOffsetEnd() - programHeader.offset;

    programHeader.memsz = segmentMemorySize;
    programHeader.filesz = segmentFileSize;
  }

  /*! \todo Add a function that extends a program header for given section
   *
   * Could be used for PT_GNU_RELRO when moving .dynamic section
   */
  

  /*! \internal Make a PT_NOTE program header that covers given note section headers
   *
   * \pre \a noteSectionHeaders must contain at least 1 section header
   * \pre each header in \a noteSectionHeaders must be a note section
   */
  inline
  ProgramHeader makeNoteProgramHeaderCoveringSections(const std::vector<SectionHeader> & noteSectionHeaders) noexcept
  {
    assert( !noteSectionHeaders.empty() );

    ProgramHeader programHeader;

    programHeader.setSegmentType(SegmentType::Note);
    programHeader.flags = 0x04;
    // Each note section should have the same alignment
    programHeader.align = noteSectionHeaders[0].addralign;

    setProgramHeaderCoveringSections(programHeader, noteSectionHeaders);

    return programHeader;

//     constexpr uint64_t maxValue = std::numeric_limits<uint64_t>::max();
// 
//     uint64_t offset = maxValue;
//     uint64_t address = maxValue;
//     uint64_t size = 0;
// 
//     for(const SectionHeader & header : noteSectionHeaders){
//       assert(header.sectionType() == SectionType::Note);
//       offset = std::min(offset, header.offset);
//       address = std::min(address, header.addr);
//       size += header.size;
//     }
//     assert( offset < maxValue );
//     assert( address < maxValue );
//     assert( size > 0 );
// 
//     programHeader.setSegmentType(SegmentType::Note);
//     programHeader.offset = offset;
//     programHeader.vaddr = address;
//     programHeader.paddr = address;
//     programHeader.filesz = size;
//     programHeader.memsz = size;
//     programHeader.flags = 0x04;
//     // Each note section should have the same alignment
//     programHeader.align = noteSectionHeaders[0].addralign;
// 
//     return programHeader;
  }

  /*! \internal Make a PT_LOAD program header that covers given section headers
   *
   * \pre \a sectionHeaders must contain at least 1 section header
   */
  inline
  ProgramHeader makeLoadProgramHeaderCoveringSections(const std::vector<SectionHeader> & sectionHeaders, uint64_t alignment) noexcept
  {
    assert( !sectionHeaders.empty() );

    ProgramHeader programHeader;

    programHeader.setSegmentType(SegmentType::Load);
    programHeader.align = alignment;

    setProgramHeaderCoveringSections(programHeader, sectionHeaders);

    return programHeader;
  }

  /*! \internal Make a PT_LOAD program header that covers given section headers
   *
   * \pre \a sectionHeadersIndexes must contain at least 1 index to the section header table
   */
  inline
  ProgramHeader makeLoadProgramHeaderCoveringSections(const std::vector<uint16_t> sectionHeadersIndexes,
                                                      const std::vector<SectionHeader> & sectionHeaderTable, uint64_t alignment) noexcept
  {
    assert( !sectionHeadersIndexes.empty() );

    ProgramHeader programHeader;

    programHeader.setSegmentType(SegmentType::Load);
    programHeader.align = alignment;

    setProgramHeaderCoveringSections(programHeader, sectionHeadersIndexes, sectionHeaderTable);

    return programHeader;
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_SEGMENT_UTILS_H
