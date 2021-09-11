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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_WRITER_H

// #include "FileReader.h"

#include "DynamicSection.h"
#include "FileHeader.h"
#include "SectionHeader.h"
#include "FileWriterUtils.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"

// #include "Debug.h"
// #include <iostream>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal Set the runtime path (DT_RUNPATH)
   *
   * \pre \a map must not be null
   * \pre \a fileHeader must be valid
   * \pre \a dynamicSectionHeader must be a Dynamic section type and be named ".dynamic"
   * \pre \a dynamicSectionHeader must have a valid index to a dynamic string table
   * \pre \a map must be big enough to read the section referenced by \a dynamicSectionHeader
   * 
   * \exception ExecutableFileReadError
   * 
   * \todo Maybe have to update the section header (size f.ex.) ?
   *    Also, should rewrite the entiere string table ?
   *    What about things after string table ?
   *    See patchelf source code..
   */
//   inline
//   void setRunPathToMap(ByteArraySpan & map, const QString & rPath, const FileHeader & fileHeader, const SectionHeader & dynamicSectionHeader)
//   {
//     assert( !map.isNull() );
//     assert( fileHeader.seemsValid() );
//     assert( headerIsDynamicSection(dynamicSectionHeader) );
//     assert( sectionHeaderHasValidIndexToDynamicStringTable(fileHeader, dynamicSectionHeader) );
//     assert( map.size >= dynamicSectionHeader.minimumSizeToReadSection() );
// 
// //     const SectionHeader dynamicStringTableSectionHeader = extractSectionHeaderAt( map, fileHeader, static_cast<uint16_t>(dynamicSectionHeader.link) );
// //     const std::vector<DynamicStruct> dynamicSection = extractDynamicSectionForTag(map, fileHeader.ident, dynamicSectionHeader, DynamicSectionTagType::Runpath);
// 
// //     std::cout << "dynamic section header: " << toDebugString(dynamicSectionHeader).toStdString() << std::endl;
// //     std::cout << "dynamic string table section header: " << toDebugString(dynamicStringTableSectionHeader).toStdString() << std::endl;
// //     std::cout << "dynamic section: " << toDebugString(dynamicSection).toStdString() << std::endl;
//     
// //     if( dynamicSection.empty() ){
// //       return QString();
// //     }
// //     if( dynamicSection.size() != 1 ){
// //       /// \todo Error
// //       return QString();
// //     }
// 
// //     const ByteArraySpan charArray = charArrayFromDynamicStringTable(map, dynamicStringTableSectionHeader, dynamicSection[0]);
// // 
// //     return qStringFromUft8ByteArraySpan(charArray);
//   }

  /*! \internal
   */
  inline
  int64_t dynamicSectionByteCount(const DynamicSection & dynamicSection, const Ident & ident) noexcept
  {
    assert( ident.isValid() );

    if(ident._class == Class::Class32){
      return 2 * sizeof(uint32_t) * dynamicSection.entriesCount();
    }
    assert(ident._class == Class::Class64);

    return 2 * sizeof(uint64_t) * dynamicSection.entriesCount();
  }

  /*! \internal
   */
  inline
  bool dynamicSectionArraySizeIsBigEnough(const ByteArraySpan & array, const DynamicSection & dynamicSection, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );

    return array.size >= dynamicSectionByteCount(dynamicSection, ident);
  }

  /*! \internal
   */
  inline
  void dynamicSectionToArray(ByteArraySpan array, const DynamicSection & dynamicSection, const Ident & ident) noexcept
  {
    assert( !array.isNull() );
    assert( ident.isValid() );
    assert( dynamicSectionArraySizeIsBigEnough(array, dynamicSection, ident) );

    int64_t offset = 0;

    if(ident._class == Class::Class32){
      for(const DynamicStruct & entry : dynamicSection){
        setSignedNWord( array.subSpan(offset, 4), entry.tag, ident );
        offset += 4;
        setNWord( array.subSpan(offset, 4), entry.val_or_ptr, ident );
        offset += 4;
      }
    }else{
      assert(ident._class == Class::Class64);
      for(const DynamicStruct & entry : dynamicSection){
        setSignedNWord( array.subSpan(offset, 8), entry.tag, ident );
        offset += 8;
        setNWord( array.subSpan(offset, 8), entry.val_or_ptr, ident );
        offset += 8;
      }
    }
  }

  /*! \internal
   */
  inline
  bool dynamicSectionHeaderSizeMatchesDynamicSectionSize(const SectionHeader & dynamicSectionHeader, const DynamicSection & dynamicSection, const Ident & ident) noexcept
  {
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );
    assert( ident.isValid() );

    return static_cast<int64_t>(dynamicSectionHeader.size) == dynamicSectionByteCount(dynamicSection, ident);
  }

  /*! \internal
   */
  inline
  int64_t minimumSizeToAccessDynamicSection(const SectionHeader & dynamicSectionHeader) noexcept
  {
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );

    return dynamicSectionHeader.offset + dynamicSectionHeader.size;
  }

  /*! \internal Set the dynamic section to \a map
   *
   * \exception DynamicSectionReadError
   * 
   * \exception DynamicSectionWriteError ?
   * 
   * \exception StringTableError
   *
   * \note this function will not set any header related to \a dynamicSection to \a map
   *
   * \todo this function should only write the dynamic section
   *   other stuff, like write string table + shif file should be done elsewhere
   *   Maybe also write the string table, but calling a dedicated function
   */
  inline
  void setDynamicSectionToMap(ByteArraySpan map, const SectionHeader & dynamicSectionHeader, const DynamicSection & dynamicSection, const FileHeader & fileHeader)
  {
    assert( !map.isNull() );
    assert( dynamicSectionHeader.sectionType() == SectionType::Dynamic );
    assert( fileHeader.seemsValid() );
    assert( dynamicSectionHeaderSizeMatchesDynamicSectionSize(dynamicSectionHeader, dynamicSection, fileHeader.ident) );
    assert( map.size >= minimumSizeToAccessDynamicSection(dynamicSectionHeader) );
//     assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );

    const int64_t offset = dynamicSectionHeader.offset;
    const int64_t size = dynamicSectionHeader.size;

    dynamicSectionToArray( map.subSpan(offset, size), dynamicSection, fileHeader.ident );

    /// shift all data above the new dynamic section / related string table
    /// \todo No
    
    /// patch all related headers
    /// \todo No
    
    // find dynamic section - must be present, otherwise we don't have a dynamic linked file
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_WRITER_H
