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

#include "FileReader.h"

#include "DynamicSection.h"
#include "FileHeader.h"
#include "SectionHeader.h"
#include "FileWriter.h"
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

  /*! \internal Set the dynamic section
   *
   * \pre \a map must not be null
   * \pre \a fileHeader must be valid
   * \pre \a map must be big enough to read all section headers
   * \pre \a sectionNamesStringTableSectionHeader must be the section header names string table
   * \exception DynamicSectionReadError
   * 
   * \exception DynamicSectionWriteError ?
   * 
   * \exception StringTableError
   */
  inline
  void setDynamicSection(ByteArraySpan & map, const DynamicSection & dynamicSection,
                                       const FileHeader & fileHeader,
                                       const SectionHeader & sectionNamesStringTableSectionHeader)
  {
    assert( !map.isNull() );
    assert( fileHeader.seemsValid() );
    assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );
    assert( sectionNamesStringTableSectionHeader.sectionType() == SectionType::StringTable );

    
    /// shift all data above the new dynamic section / related string table
    
    /// patch all related headers
    
    // find dynamic section - must be present, otherwise we don't have a dynamic linked file
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_DYNAMIC_SECTION_WRITER_H
