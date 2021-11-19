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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_WRITER_H

#include "SectionHeader.h"
#include "SectionHeaderReaderWriterCommon.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "FileWriterUtils.h"
#include "FileHeader.h"
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array must be big enough to hold a section header
   * 
   * \todo some preconditions on \a sectionHeader ?
   * 
   * \pre \a fileHeader must be valid
   * \sa sectionHeaderArraySizeIsBigEnough()
   *
   * \todo what about section names in string table ?
   */
  inline
  void sectionHeaderToArray(ByteArraySpan array, const SectionHeader & sectionHeader, const FileHeader & fileHeader) noexcept
  {
    assert( !array.isNull() );
    assert( fileHeader.seemsValid() );
    
    assert( sectionHeaderArraySizeIsBigEnough(array, fileHeader) );

    const Ident ident = fileHeader.ident;

    set32BitWord(array.subSpan(0, 4), sectionHeader.nameIndex, ident.dataFormat);
    set32BitWord(array.subSpan(0x04, 4), sectionHeader.type, ident.dataFormat);

    if(ident._class == Class::Class32){
      setNWord(array.subSpan(0x08, 4), sectionHeader.flags, ident);
      setAddress(array.subSpan(0x0C, 4), sectionHeader.addr, ident);
      setOffset(array.subSpan(0x10, 4), sectionHeader.offset, ident);
      setNWord(array.subSpan(0x14, 4), sectionHeader.size, ident);
      set32BitWord(array.subSpan(0x18, 4), sectionHeader.link, ident.dataFormat);
      set32BitWord(array.subSpan(0x1C, 4), sectionHeader.info, ident.dataFormat);
      setNWord(array.subSpan(0x20, 4), sectionHeader.addralign, ident);
      setNWord(array.subSpan(0x24, 4), sectionHeader.entsize, ident);
    }else{
      assert( ident._class == Class::Class64 );
      setNWord(array.subSpan(0x08, 8), sectionHeader.flags, ident);
      setAddress(array.subSpan(0x10, 8), sectionHeader.addr, ident);
      setOffset(array.subSpan(0x18, 8), sectionHeader.offset, ident);
      setNWord(array.subSpan(0x20, 8), sectionHeader.size, ident);
      set32BitWord(array.subSpan(0x28, 4), sectionHeader.link, ident.dataFormat);
      set32BitWord(array.subSpan(0x2C, 4), sectionHeader.info, ident.dataFormat);
      setNWord(array.subSpan(0x30, 8), sectionHeader.addralign, ident);
      setNWord(array.subSpan(0x38, 8), sectionHeader.entsize, ident);
    }
  }

  /*! \internal Check that the count of headers matches the one set in \a fileHeader
   */
  inline
  bool fileHeaderMatchesSectionHeadersCounts(const FileHeader & fileHeader, const std::vector<SectionHeader> & sectionHeaders) noexcept
  {
    assert( fileHeader.seemsValid() );

    return fileHeader.shnum == sectionHeaders.size();
  }

  /*! \internal
   */
  inline
  void setSectionHeadersToMap(ByteArraySpan map, const std::vector<SectionHeader> & sectionHeaders, const FileHeader & fileHeader) noexcept
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( fileHeaderMatchesSectionHeadersCounts(fileHeader, sectionHeaders) );
    assert( map.size >= fileHeader.minimumSizeToReadAllProgramHeaders() );

    const uint16_t sectionHeaderCount = fileHeader.shnum;
    const int64_t start = static_cast<int64_t>(fileHeader.shoff);

    for(uint16_t i = 0; i < sectionHeaderCount; ++i){
      const int64_t offset = start + i * fileHeader.shentsize;
      const int64_t size = fileHeader.shentsize;
      sectionHeaderToArray(map.subSpan(offset, size), sectionHeaders[i], fileHeader);
    }
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_SECTION_HEADER_WRITER_H
