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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_HEADER_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_HEADER_WRITER_H

#include "ProgramHeaderReaderWriterCommon.h"
#include "ProgramHeader.h"
#include "FileHeader.h"
#include "FileWriterUtils.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cstdint>
#include <vector>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   *
   * \pre \a array must not be null
   * \pre \a array must be big enough to hold a program header
   * 
   * \todo some preconditions on \a programHeader ?
   * 
   * \pre \a fileHeader must be valid
   * \sa programHeaderArraySizeIsBigEnough()
   */
  inline
  void programHeaderToArray(ByteArraySpan array, const ProgramHeader & programHeader, const FileHeader & fileHeader) noexcept
  {
    assert( !array.isNull() );
    assert( fileHeader.seemsValid() );
    
    assert( programHeaderArraySizeIsBigEnough(array, fileHeader) );

    const Ident ident = fileHeader.ident;

    set32BitWord(array.subSpan(0, 4), programHeader.type, ident.dataFormat);

    if(ident._class == Class::Class32){
      setOffset(array.subSpan(0x04, 4), programHeader.offset, ident);
      setAddress(array.subSpan(0x08, 4), programHeader.vaddr, ident);
      setAddress(array.subSpan(0x0C, 4), programHeader.paddr, ident);
      setNWord(array.subSpan(0x10, 4), programHeader.filesz, ident);
      setNWord(array.subSpan(0x14, 4), programHeader.memsz, ident);
      setNWord(array.subSpan(0x18, 4), programHeader.flags, ident);
      setNWord(array.subSpan(0x1C, 4), programHeader.align, ident);
    }else{
      assert( ident._class == Class::Class64 );
      set32BitWord(array.subSpan(0x04, 4), programHeader.flags, ident.dataFormat);
      setOffset(array.subSpan(0x08, 8), programHeader.offset, ident);
      setAddress(array.subSpan(0x10, 8), programHeader.vaddr, ident);
      setAddress(array.subSpan(0x18, 8), programHeader.paddr, ident);
      setNWord(array.subSpan(0x20, 8), programHeader.filesz, ident);
      setNWord(array.subSpan(0x28, 8), programHeader.memsz, ident);
      setNWord(array.subSpan(0x30, 8), programHeader.align, ident);
    }
  }

  /*! \internal Check that the count of headers matches the one set in \a fileHeader
   */
  inline
  bool fileHeaderMatchesProgramHeadersCounts(const FileHeader & fileHeader, const std::vector<ProgramHeader> & programHeaders) noexcept
  {
    assert( fileHeader.seemsValid() );

    return fileHeader.phnum == programHeaders.size();
  }

  /*! \internal
   */
  inline
  void setProgramHeadersToMap(ByteArraySpan map, const std::vector<ProgramHeader> & programHeaders, const FileHeader & fileHeader) noexcept
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( fileHeaderMatchesProgramHeadersCounts(fileHeader, programHeaders) );
    assert( map.size >= fileHeader.minimumSizeToReadAllProgramHeaders() );

    const uint16_t programHeaderCount = fileHeader.phnum;
    const int64_t start = fileHeader.phoff;

    for(uint16_t i = 0; i < programHeaderCount; ++i){
      const int64_t offset = start + i * fileHeader.phentsize;
      const int64_t size = fileHeader.phentsize;
      programHeaderToArray(map.subSpan(offset, size), programHeaders[i], fileHeader);
    }
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_HEADER_WRITER_H
