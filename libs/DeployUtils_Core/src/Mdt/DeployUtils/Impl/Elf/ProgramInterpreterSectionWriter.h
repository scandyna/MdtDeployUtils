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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_INTERPRETER_SECTION_WRITER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_INTERPRETER_SECTION_WRITER_H

#include "ProgramInterpreterSection.h"
#include "FileHeader.h"
#include "SectionHeader.h"
#include "FileWriterUtils.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.h"
#include <string>
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  void setProgramInterpreterSectionToArray(ByteArraySpan array, const ProgramInterpreterSection & section) noexcept
  {
    assert( !array.isNull() );
    assert( array.size == static_cast<int64_t>( section.path.size() + 1 ) );

    setStringToUnsignedCharArray(array, section.path);
  }

  /*! \internal
   */
  inline
  void setProgramInterpreterSectionToMap(ByteArraySpan map, const SectionHeader & sectionHeader,
                                         const ProgramInterpreterSection & section) noexcept
  {
    assert( !map.isNull() );
//     assert( !table.isEmpty() );
    assert( sectionHeader.isProgramInterpreterSectionHeader() );
    assert( map.size >= sectionHeader.minimumSizeToWriteSection() );

    const int64_t offset = static_cast<int64_t>(sectionHeader.offset);
    const int64_t size = static_cast<int64_t>(sectionHeader.size);
    setProgramInterpreterSectionToArray(map.subSpan(offset, size), section);
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_INTERPRETER_SECTION_WRITER_H
