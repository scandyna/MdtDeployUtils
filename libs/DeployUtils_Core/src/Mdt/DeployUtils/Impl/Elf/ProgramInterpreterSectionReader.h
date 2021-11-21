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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_INTERPRETER_SECTION_READER_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_INTERPRETER_SECTION_READER_H

#include "ProgramInterpreterSection.h"
#include "FileHeader.h"
#include "SectionHeader.h"
#include "Mdt/DeployUtils/Impl/ByteArraySpan.h"
#include "Mdt/DeployUtils/Impl/ExecutableFileReaderUtils.h"
#include <string>
#include <cstdint>
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  ProgramInterpreterSection extractProgramInterpreterSection(const ByteArraySpan & map, const SectionHeader & sectionHeader) noexcept
  {
    assert( !map.isNull() );
//     assert( fileHeader.seemsValid() );
    assert( map.size >= sectionHeader.minimumSizeToReadSection() );
    assert( sectionHeader.isProgramInterpreterSectionHeader() );

    ProgramInterpreterSection section;

    const int64_t offset = static_cast<int64_t>(sectionHeader.offset);
    const int64_t size = static_cast<int64_t>(sectionHeader.size);
    section.path = stringFromBoundedUnsignedCharArray( map.subSpan(offset, size) );

    return section;
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_PROGRAM_INTERPRETER_SECTION_READER_H
