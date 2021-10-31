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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_WRITER_H

#include "NoteSection.h"
#include "NoteSectionTable.h"
#include "FileHeader.h"
#include "SectionHeader.h"
#include "FileWriterUtils.h"
#include "Algorithm.h"
#include "Exceptions.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class MDT_DEPLOYUTILSCORE_EXPORT NoteSectionWriter : public QObject
  {
    Q_OBJECT

   public:

    /*! \brief
     */
    static
    void setNoteSectionToArray(ByteArraySpan array, const NoteSection & noteSection, const Ident & ident) noexcept
    {
      assert( !array.isNull() );
      assert( !noteSection.isNull() );
      assert( ident.isValid() );
      assert( array.size == noteSection.byteCountAligned() );

      const uint32_t nameSize = static_cast<uint32_t>( noteSection.name.size() ) + 1;
      set32BitWord(array.subSpan(0, 4), nameSize, ident.dataFormat);
      set32BitWord(array.subSpan(4, 4), noteSection.descriptionSize, ident.dataFormat);
      set32BitWord(array.subSpan(8, 4), noteSection.type, ident.dataFormat);
      setStringToUnsignedCharArray(array.subSpan(12, static_cast<int64_t>(nameSize)), noteSection.name);

      const int64_t nameEndOffset = 12 + static_cast<int64_t>(nameSize);
      const int64_t alignedNameSize = static_cast<int64_t>( findAlignedSize(nameSize, 4) );
      assert( nameEndOffset > 0 );
      assert( alignedNameSize > 0 );
      const int64_t afterNamePaddingSize = alignedNameSize - static_cast<int64_t>(nameSize);
      assert( afterNamePaddingSize >= 0 );
      if(afterNamePaddingSize > 0){
        replaceBytesInArray(array.subSpan(nameEndOffset, afterNamePaddingSize), 0);
      }

      int64_t descriptionOffset = 12 + alignedNameSize;
      for(uint32_t word : noteSection.description){
        set32BitWord(array.subSpan(descriptionOffset, 4), word, ident.dataFormat);
        descriptionOffset += 4;
      }
    }

    /*! \brief
     */
    static
    void setNoteSectionToMap(ByteArraySpan map, const SectionHeader & sectionHeader,
                             const NoteSection & noteSection, const FileHeader & fileHeader) noexcept
    {
      assert( !map.isNull() );
      assert( !noteSection.isNull() );
      assert( fileHeader.seemsValid() );
      assert( isNoteSectionHeader(sectionHeader) );
      assert( map.size >= sectionHeader.minimumSizeToWriteSection() );

      const int64_t offset = static_cast<int64_t>(sectionHeader.offset);
      const int64_t size = static_cast<int64_t>(sectionHeader.size);
      setNoteSectionToArray(map.subSpan(offset, size), noteSection, fileHeader.ident);
    }

    /*! \brief
     */
    static
    void setNoteSectionTableToMap(ByteArraySpan map, const NoteSectionTable & table, const FileHeader & fileHeader) noexcept
    {
      assert( !map.isNull() );
//       assert( !table.isEmpty() );
      assert( fileHeader.seemsValid() );
      assert( map.size >= table.findMinimumSizeToWriteTable() );

      for(size_t i=0; i < table.sectionCount(); ++i){
        setNoteSectionToMap(map, table.sectionHeaderAt(i), table.sectionAt(i), fileHeader);
      }
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_WRITER_H
