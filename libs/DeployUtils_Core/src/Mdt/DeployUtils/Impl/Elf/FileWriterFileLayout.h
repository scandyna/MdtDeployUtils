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
    }

    uint64_t dynamicSectionSize() const noexcept
    {
      return mDynamicSectionOffsetRange.byteCount();
    }

    OffsetRange dynamicStringTableOffsetRange() const noexcept
    {
      return mDynamicStringTableOffsetRange;
    }

    uint64_t dynamicStringTableOffset() const noexcept
    {
      return mDynamicStringTableOffsetRange.begin();
    }

    uint64_t dynamicStringTableSize() const noexcept
    {
      return mDynamicStringTableOffsetRange.byteCount();
    }

    OffsetRange globalOffsetRange() const noexcept
    {
      return mGlobalOffsetRange;
    }

    /*! \brief Get a file layout from a file
     *
     * \pre \a headers must be valid
     * \pre \a headers must contain the dynamic program header
     * \pre \a headers must contain the dynamic section header
     */
    static
    FileWriterFileLayout fromFile(const FileAllHeaders & headers) noexcept
    {
      assert( headers.seemsValid() );
      assert( headers.containsDynamicProgramHeader() );
      assert( headers.containsDynamicSectionHeader() );
      assert( headers.containsDynamicStringTableSectionHeader() );

      FileWriterFileLayout layout;
      layout.mDynamicSectionOffsetRange = OffsetRange::fromProgrameHeader( headers.dynamicProgramHeader() );
      layout.mDynamicStringTableOffsetRange = OffsetRange::fromSectionHeader( headers.dynamicStringTableSectionHeader() );
      layout.mGlobalOffsetRange = headers.globalFileOffsetRange();

      return layout;
    }

  private:

    OffsetRange mDynamicSectionOffsetRange;
    OffsetRange mDynamicStringTableOffsetRange;
    OffsetRange mGlobalOffsetRange;
  };

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_FILE_WRITER_FILE_LAYOUT_H
