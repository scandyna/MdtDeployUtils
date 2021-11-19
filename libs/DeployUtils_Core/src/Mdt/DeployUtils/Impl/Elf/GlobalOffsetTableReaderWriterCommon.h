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
#ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_READER_WRITER_COMMON_H
#define MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_READER_WRITER_COMMON_H

#include "SectionHeader.h"
#include <cassert>

namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

  /*! \internal
   */
  inline
  bool isGlobalOffsetTableSection(const SectionHeader & header) noexcept
  {
    assert( !header.name.empty() );

    if(header.sectionType() != SectionType::ProgramData){
      return false;
    }
    if(header.name == ".got"){
      return true;
    }
    if(header.name == ".got.plt"){
      return true;
    }

    return false;
  }

}}}} // namespace Mdt{ namespace DeployUtils{ namespace Impl{ namespace Elf{

#endif // #ifndef MDT_DEPLOY_UTILS_IMPL_ELF_GLOBAL_OFFSET_TABLE_READER_WRITER_COMMON_H
