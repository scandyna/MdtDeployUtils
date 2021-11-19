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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_H

#include "FileHeader.h"
#include "OffsetRange.h"
#include "FileWriterFile.h"
#include "FileAllHeaders.h"
#include "FileAllHeadersWriter.h"
#include "DynamicSection.h"
#include "DynamicSectionWriter.h"
#include "StringTable.h"
#include "StringTableWriter.h"
#include "SymbolTableWriter.h"
#include "GlobalOffsetTableWriter.h"
#include "ProgramInterpreterSectionWriter.h"
#include "GnuHashTableWriter.h"
#include "NoteSectionWriter.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <QtEndian>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cassert>

// #include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal Shift bytes starting from \a startOffset towards the beginning for \a count bytes
   *
   * Example:
   * \code
   * // Initial map
   * {s,t,r,\0,o,l,d,\0,1,2,3}
   *
   * shiftBytesToBegin(map, 8, 4);
   *
   * // map after shift
   * {s,t,r,\0,1,2,3}
   * \endcode
   */
  inline
  void shiftBytesToBegin(ByteArraySpan & map, uint64_t startOffset, int64_t count) noexcept
  {
    assert( !map.isNull() );
    assert( static_cast<uint64_t>(map.size) > startOffset );
    assert( map.size >= count );
    assert( count >= 0 );

    /*
     * 1 element:
     * f.ex. {A}
     * we have nothing to move
     */
    if(map.size <= 1){
      map.size = 0;
      return;
    }

    const auto first = map.begin() + startOffset;
    const auto last = map.end();
    const auto dFirst = first - count;

    std::move(first, last, dFirst);

    map.size -= count;
    assert( map.size >= 0 );
  }

  /*! \internal Shift bytes starting from \a startOffset towards the end for \a count bytes
   *
   * Example:
   * \code
   * // Initial map
   * {s,t,r,\0,1,2,3,0,0,0}
   *
   * shiftBytesToEnd(map, 4, 3);
   *
   * // map after shift
   * {s,t,r,\0,?,?,?,1,2,3}
   * \endcode
   *
   * Note that \a map should have enought space to shift the byte by \a count ,
   * otherwise some data can be lost:
   * \code
   * {s,t,r,\0,1,2,3,0}
   *
   * shiftBytesToEnd(map, 4, 3);
   *
   * // map after shift
   * {s,t,r,\0,?,?,?,1}
   * \endcode
   */
  inline
  void shiftBytesToEnd(ByteArraySpan & map, uint64_t startOffset, int64_t count) noexcept
  {
    assert( !map.isNull() );
    assert( count >= 0 );
    assert( static_cast<uint64_t>(map.size) > startOffset );
    assert( map.size >= count );

    auto first = map.begin() + startOffset;
    auto last = map.end() - count;
    auto dFirst = map.end();

    std::move_backward(first, last, dFirst);
  }


  /*! \internal
   */
  inline
  void replaceBytes(ByteArraySpan map, const OffsetRange & range, unsigned char c) noexcept
  {
    assert( !map.isNull() );
    assert( map.size >= range.minimumSizeToAccessRange() );

    auto first = map.begin() + range.begin();
    auto last =  map.begin() + range.end();

    while(first != last){
      *first = c;
      ++first;
    }
  }

  /*! \internal
   *
   * Old string table:
   * \code
   * \0libA.so\0/home/me/lib\0
   * \endcode
   *
   * New string table:
   * \code
   * \0libA.so\0
   * \endcode
   *
   * Because we don't move any section or segment after the string table,
   * there will be a hole that contains the old strings,
   * despite they are not referenced anymore:
   * \code
   * // Old strings remains
   * \0libA.so\0/home/me/lib\0
   *
   * // replace them
   * \0libA.so\0\0\0\0\0\0\0\0\0\0\0\0\0\0
   * \endcode
   */
  inline
  void setBytesAfterOldDynamicStringTableNull(ByteArraySpan map, const FileWriterFile & file) noexcept
  {
    assert( !map.isNull() );

    const uint64_t begin = file.dynamicStringTableOffsetRange().end();
    const uint64_t end = file.originalDynamicStringTableOffsetRange().end();

    if(begin < end){
      const auto range = OffsetRange::fromBeginAndEndOffsets(begin, end);
      replaceBytes(map, range, '\0');
    }
  }

  /*! \internal
   *
   * We cannot simply move anything from the file.
   * If we wish to, we would have to be a linker,
   * and support everything that exists in a ELF file,
   * including compiler specific stuff.
   *
   * patchelf itself seems to not move stuff it does not know about.
   *
   * If a entry is removed from the dynamic section:
   * - update dynamic section (responsability: DynamicSection)
   * - update sizes of the related program header
   * - update sizes of the related section header
   * This will let a little hole in the file.
   *
   * If a bytes are removed from the dynamic string table:
   * - remove them from the string table (responsability: StringTable)
   * - update dynamic section (responsability: DynamicSection)
   * - update sizes of the related section header
   * - fill the created hole with null bytes (avoid having unwanted personal paths left in the file)
   *
   * If a entry is added to the dynamic section:
   * - update dynamic section (responsability: DynamicSection)
   * - move the dynamic section to the end of the file
   * - update the related program header (offset, address, size)
   * - update the related section header (offset, address, size)
   * - make a new PT_LOAD segment that references the dynamic section
   *   hmm.. implies putting program header table to the end of the file..
   * - update file header
   *
   * If bytes are added to the dynamic string table:
   * - update dynamic section (responsability: DynamicSection)
   * - move the dynamic section to the end of the file
   * - update the related program header (offset, address, size)
   * - update the related section header (offset, address, size)
   * - make a new PT_LOAD segment that references the dynamic section
   *   hmm.. implies putting program header table to the end of the file..
   * - update file header
   *
   * Note: if both of the dynamic section and dynamic string table grows,
   * make a single PT_LOAD segment that references both
   */
  inline
  void setFileToMap(ByteArraySpan map, const FileWriterFile & file) noexcept
  {
    assert( !map.isNull() );
    assert( file.seemsValid() );
    assert( map.size >= file.minimumSizeToWriteFile() );

    if( file.dynamicStringTableMovesToEnd() ){
      replaceBytes(map, file.originalDynamicStringTableOffsetRange(), '\0');
      ///setSymbolTableToMap(map, file.sectionSymbolTable(), file.fileHeader().ident);
    }else{
      setBytesAfterOldDynamicStringTableNull(map, file);
    }

    if( file.dynamicSectionMovesToEnd() ){
      ///setSymbolTableToMap(map, file.sectionSymbolTable(), file.fileHeader().ident);
      if( !file.gotSection().isEmpty() && file.headers().containsGotSectionHeader() ){
        setGlobalOffsetTableToMap( map, file.headers().gotSectionHeader(), file.gotSection(), file.fileHeader() );
      }
      if( !file.gotPltSection().isEmpty() && file.headers().containsGotPltSectionHeader() ){
        setGlobalOffsetTableToMap( map, file.headers().gotPltSectionHeader(), file.gotPltSection(), file.fileHeader() );
      }
    }

    if( file.headers().containsProgramInterpreterSectionHeader() ){
      setProgramInterpreterSectionToMap( map, file.headers().programInterpreterSectionHeader(), file.programInterpreterSection() );
    }

    if( file.headers().containsGnuHashTableSectionHeader() ){
      GnuHashTableWriter::setGnuHashTableToMap( map, file.headers().gnuHashTableSectionHeader(), file.gnuHashTableSection(), file.fileHeader() );
    }

    /// \todo should only write if changes

    NoteSectionWriter::setNoteSectionTableToMap( map, file.noteSectionTable(), file.fileHeader() );

    setSymbolTableToMap(map, file.symTab(), file.fileHeader().ident);
    setSymbolTableToMap(map, file.dynSym(), file.fileHeader().ident);

    setDynamicSectionToMap( map, file.dynamicSectionHeader(), file.dynamicSection(), file.fileHeader() );
    setStringTableToMap( map, file.headers().dynamicStringTableSectionHeader(), file.dynamicSection().stringTable() );
    setAllHeadersToMap( map, file.headers() );
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_H
