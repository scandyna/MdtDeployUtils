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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_STRING_TABLE_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_STRING_TABLE_WRITER_H

#include "FileHeader.h"
#include "SectionHeader.h"
#include "StringTable.h"
#include "FileWriterUtils.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include <cstdint>
#include <algorithm>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  void stringTableToArray(ByteArraySpan map, const StringTable & stringTable) noexcept
  {
    assert( !map.isNull() );
    assert( map.size >= stringTable.byteCount() );

    std::copy( stringTable.cbegin(), stringTable.cend(), map.data );
  }

  /*! \internal
   */
  inline
  int64_t minimumSizeToAccessStringTable(const SectionHeader & sectionHeader) noexcept
  {
    assert( sectionHeader.sectionType() == SectionType::StringTable );

    return sectionHeader.offset + sectionHeader.size;
  }

  /*! \internal
   */
  inline
  void setStringTableToMap(ByteArraySpan map, const SectionHeader & sectionHeader, const StringTable & stringTable) noexcept
  {
    assert( !map.isNull() );
    assert( sectionHeader.sectionType() == SectionType::StringTable );
    assert( static_cast<int64_t>(sectionHeader.size) == stringTable.byteCount() );
    assert( map.size >= minimumSizeToAccessStringTable(sectionHeader) );

    const int64_t offset = sectionHeader.offset;
    const int64_t size = sectionHeader.size;

    stringTableToArray( map.subSpan(offset, size), stringTable );
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_STRING_TABLE_WRITER_H
