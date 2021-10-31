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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_READER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_READER_H

#include "NoteSection.h"
#include "NoteSectionTable.h"
#include "FileHeader.h"
#include "FileReader.h"
#include "SectionHeader.h"
#include "SectionHeaderTable.h"
#include "Algorithm.h"
#include "Exceptions.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.h"
#include "mdt_deployutilscore_export.h"
#include <QObject>
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class MDT_DEPLOYUTILSCORE_EXPORT NoteSectionReader : public QObject
  {
    Q_OBJECT

   public:

    /*! \internal
     *
     * \exception NoteSectionReadError
     */
    static
    NoteSection noteSectionFromArray(const ByteArraySpan & array, const Ident & ident)
    {
      assert( !array.isNull() );
      assert( ident.isValid() );
      assert( array.size >= NoteSection::minimumByteBount() );

      NoteSection section;

      const int64_t nameSize = static_cast<int64_t>( getWord(array.data, ident.dataFormat) );
      if(nameSize == 0){
        const QString msg = tr("name size is 0");
        throw NoteSectionReadError(msg);
      }
      if( nameSize > NoteSection::maximumNameSize(array.size) ){
        const QString msg = tr("name size %1 is to large").arg(nameSize);
        throw NoteSectionReadError(msg);
      }
      assert(nameSize < array.size);

      section.descriptionSize = getWord(array.subSpan(4, 4).data, ident.dataFormat);
      section.type = getWord(array.subSpan(8, 4).data, ident.dataFormat);

      section.name = stringFromBoundedUnsignedCharArray( array.subSpan(12, nameSize) );

      const int64_t descriptionStart = 12 + static_cast<int64_t>( findAlignedSize(nameSize, 4) );
      const int64_t descriptionEnd = descriptionStart + static_cast<int64_t>(section.descriptionSize);

      if(descriptionEnd > array.size){
        const QString msg = tr("section name size and/or description size is to large");
        throw NoteSectionReadError(msg);
      }

      for(int64_t offset = descriptionStart; offset < descriptionEnd; offset += 4){
        section.description.push_back( getWord(array.subSpan(offset, 4).data, ident.dataFormat) );
      }

      return section;
    }

    /*! \internal
     *
     * \exception NoteSectionReadError
     */
    static
    NoteSection extractNoteSection(const ByteArraySpan & map, const FileHeader & fileHeader, const SectionHeader & sectionHeader)
    {
      assert( !map.isNull() );
      assert( fileHeader.seemsValid() );
      assert( map.size >= sectionHeader.minimumSizeToReadSection() );
      assert( isNoteSectionHeader(sectionHeader) );

      const int64_t offset = static_cast<int64_t>(sectionHeader.offset);
      const int64_t size = static_cast<int64_t>(sectionHeader.size);

      try{
        return noteSectionFromArray(map.subSpan(offset, size), fileHeader.ident);
      }catch(const NoteSectionReadError & error){
        const QString msg = tr("note section %1 is corrupted: %2")
                            .arg( QString::fromStdString(sectionHeader.name), error.whatQString() );
        throw NoteSectionReadError(msg);
      }
    }

    /*! \internal
     *
     * \exception NoteSectionReadError
     */
    static
    NoteSectionTable extractNoteSectionTable(const ByteArraySpan & map, const FileHeader & fileHeader,
                                            const std::vector<SectionHeader> & sectionHeaderTable)
    {
      assert( !map.isNull() );
      assert( fileHeader.seemsValid() );
      assert( map.size >= fileHeader.minimumSizeToReadAllSectionHeaders() );

      NoteSectionTable table;

      for(const SectionHeader & header : sectionHeaderTable){
        if( isNoteSectionHeader(header) ){
          table.addSectionFromFile( header, extractNoteSection(map, fileHeader, header) );
        }
      }

      return table;
    }
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_NOTE_SECTION_READER_H
