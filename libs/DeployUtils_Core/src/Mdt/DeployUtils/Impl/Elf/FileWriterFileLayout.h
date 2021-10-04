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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_LAYOUT_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_LAYOUT_H

#include "OffsetRange.h"
#include "FileAllHeaders.h"
#include "DynamicSection.h"
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  class FileWriterFileLayout
  {
  public:

    OffsetRange dynamicSectionOffsetRange() const noexcept
    {
      return mDynamicSectionOffsetRange;
    }

    uint64_t dynamicSectionOffset() const noexcept
    {
      return mDynamicSectionOffsetRange.begin();
//       return mDynamicSectionOffset;
    }

//     uint64_t dynamicSectionEndOffset() const noexcept
//     {
//       if(mDynamicSectionSize == 0){
//         return mDynamicSectionOffset;
//       }
//       return mDynamicSectionOffset + mDynamicSectionSize - 1;
//     }

    uint64_t dynamicSectionSize() const noexcept
    {
      return mDynamicSectionOffsetRange.byteCount();
//       return mDynamicSectionSize;
    }

    OffsetRange dynamicStringTableOffsetRange() const noexcept
    {
      return mDynamicStringTableOffsetRange;
    }

    uint64_t dynamicStringTableOffset() const noexcept
    {
      return mDynamicStringTableOffsetRange.begin();
//       return mDynamicStringTableOffset;
    }

//     uint64_t dynamicStringTableEndOffset() const noexcept
//     {
//       if(mDynamicStringTableSize == 0){
//         return mDynamicStringTableOffset;
//       }
//       return mDynamicStringTableOffset + mDynamicStringTableSize - 1;
//     }

    uint64_t dynamicStringTableSize() const noexcept
    {
      return mDynamicStringTableOffsetRange.byteCount();
//       return mDynamicStringTableSize;
    }

    OffsetRange globalOffsetRange() const noexcept
    {
      return mGlobalOffsetRange;
    }

    /*! \brief Get a file layout from a file
     *
     * \pre \a headers must be valid
     * \pre \a dynamicSection must not be null
     * \pre \a headers must contain the dynamic program header
     * \pre \a headers must contain the dynamic section header
     */
    static
    FileWriterFileLayout fromFile(const FileAllHeaders & headers, const DynamicSection & dynamicSection) noexcept
    {
      assert( headers.seemsValid() );
      assert( !dynamicSection.isNull() );
      assert( headers.containsDynamicProgramHeader() );
      assert( headers.containsDynamicSectionHeader() );
      assert( headers.containsDynamicStringTableSectionHeader() );

      FileWriterFileLayout layout;
      layout.mDynamicSectionOffsetRange = OffsetRange::fromProgrameHeader( headers.dynamicProgramHeader() );
      layout.mDynamicStringTableOffsetRange = OffsetRange::fromSectionHeader( headers.dynamicStringTableSectionHeader() );
      layout.mGlobalOffsetRange = headers.globalFileOffsetRange();
//       layout.mDynamicSectionOffset = headers.dynamicProgramHeader().offset;
//       layout.mDynamicSectionSize = headers.dynamicProgramHeader().filesz;
//       layout.mDynamicStringTableOffset = headers.dynamicStringTableSectionHeader().offset;
//       layout.mDynamicStringTableSize = headers.dynamicStringTableSectionHeader().size;

      return layout;
    }

  private:

    OffsetRange mDynamicSectionOffsetRange;
    OffsetRange mDynamicStringTableOffsetRange;
    OffsetRange mGlobalOffsetRange;
//     uint64_t mDynamicSectionOffset = 0;
//     uint64_t mDynamicSectionSize = 0;
//     uint64_t mDynamicStringTableOffset = 0;
//     uint64_t mDynamicStringTableSize = 0;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_LAYOUT_H
